#include <UIMgr.hpp>

#define MAX_CHAR MAX_COMMAND_SIZE

#define ID_KEY_TAB			9
#define ID_KEY_ENTER		10
#define ID_KEY_ESC			27
#define ID_KEY_INS			50
#define ID_KEY_DEL			51
#define ID_KEY_PGUP			53
#define ID_KEY_PGDN			54
#define ID_KEY_UP			65
#define ID_KEY_DOWN			66
#define ID_KEY_RIGHT		67
#define ID_KEY_LEFT			68
#define ID_KEY_END			70
#define ID_KEY_HOME			72
#define ID_KEY_BACKSPACE	127


UIMgr::UIMgr()
{
    Prompt = strdup("CLIng>>");
    for (int i=0;i<MAX_KEY;i++)
        CallBack[i] = NULL;
    UI_State.unlock();
    Runner = NULL;
    tcgetattr( STDIN_FILENO, &oldt );
    newt = oldt;
    newt.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newt );
}

UIMgr::~UIMgr()
{
    if(NULL != Prompt)
        free(Prompt);
    UI_Stop();
    if(NULL != Runner)
        free(Runner);
    Prompt = NULL;
    Runner = NULL;
    tcsetattr( STDIN_FILENO, TCSANOW,&oldt );
}

bool UIMgr::SetPrompt(const char* prompt)
{
    if(NULL == prompt)
        return false;
    if(NULL != Prompt)
        free(Prompt);
    Prompt = strdup(prompt);
    if(NULL == Prompt)
        return false;
    return true;
}

bool UIMgr::RegisterKeyHandler(int uKey, CB_KeyHandler keyHandler)
{
    if(uKey < 0 || uKey >= MAX_KEY)
        return false;

    if(CallBack[uKey] != NULL)
        return false;

    CallBack[uKey] = keyHandler;
    return true;
}

bool UIMgr::UI_isRunning()
{
    if(UI_State.try_lock())
    {
        UI_State.unlock();
        return false;
    }
    return true;
}

void UIMgr::UI_Engine()
{
	bool nextCommand;
	char *cBuff = (char*)malloc(MAX_CHAR);
	char *resp = NULL;
	memset(cBuff,0,MAX_CHAR);
	int i, pos = 0, len = 0;
	char ch, prev,nxt;
	bool ins = true;
	while(UI_isRunning())
	{
		printf("%s",Prompt);
		nextCommand = false;
		while(!nextCommand)
		{
			ch = getchar();
			//printf("%c::%d\n",ch,ch);
			switch (ch)
			{
				case ID_KEY_TAB:
					if(NULL != CallBack[KEY_TAB])                                                                    
						CallBack[KEY_TAB](KEY_TAB, cBuff,&resp);
					if(resp)
					{
						strncpy(cBuff,resp,MAX_CHAR);
						printf("\033[%dD\033[K%s",pos,cBuff);
						len = strlen(cBuff);
						pos = len;
						free(resp);
						resp = NULL;
					}
				break;
				case ID_KEY_ENTER:
					cBuff[len] = '\0';
					if(NULL != CallBack[KEY_ENTER])
						CallBack[KEY_ENTER](KEY_ENTER, cBuff,&resp);
					pos = 0; len = 0;
					memset(cBuff,0,MAX_CHAR);
					if(resp)
					{
						printf("\n%s\n",resp);
						free(resp);
						resp = NULL;
					}
					else
						printf("\n");
					nextCommand = true;
				break;
				case ID_KEY_ESC:
					ch = getchar(); //get the '[' character and discard
					ch = getchar(); //get the significant character from the escape sequence and process it
					//printf("%c::%d\n",ch,ch);
					switch (ch)
					{
						case ID_KEY_UP:
							if(NULL != CallBack[KEY_UP])
								CallBack[KEY_UP](KEY_UP, cBuff,&resp);
							if(resp)
							{
								free(resp);
								resp = NULL;
							}
						break;
						case ID_KEY_DOWN:
							if(NULL != CallBack[KEY_DOWN])
								CallBack[KEY_DOWN](KEY_DOWN, cBuff,&resp);
							if(resp)
							{
								free(resp);
								resp = NULL;
							}
						break;
						case ID_KEY_RIGHT:
							if(NULL != CallBack[KEY_RIGHT])
							{
								CallBack[KEY_RIGHT](KEY_RIGHT, cBuff,&resp);
								if(resp)
								{
									free(resp);
									resp = NULL;
								}
							}	
							else
							{
								if(pos >= len) break;
								printf("\033[C");
								pos ++;
							}
						break;
						case ID_KEY_LEFT:
							if(NULL != CallBack[KEY_LEFT])
							{
								CallBack[KEY_LEFT](KEY_LEFT, cBuff,&resp);
								if(resp)
								{
									free(resp);
									resp = NULL;
								}
							}	
							else
							{
								if(pos <= 0 ) break;
								printf("\033[D");
								pos --;
							}
						break;
						case ID_KEY_END:
							if(NULL != CallBack[KEY_END])
							{
								CallBack[KEY_END](KEY_END, cBuff,&resp);
								if(resp)
								{
									free(resp);
									resp = NULL;
								}
							}	
							else
							{
								printf("\033[%dC",len-pos);
								pos = len;
							}
						break;
						case ID_KEY_HOME:
							if(NULL != CallBack[KEY_HOME])
							{
								CallBack[KEY_HOME](KEY_HOME, cBuff,&resp);
								if(resp)
								{
									free(resp);
									resp = NULL;
								}
							}	
							else
							{
								printf("\033[%dD",pos);
								pos = 0;
							}
						break;
						case ID_KEY_INS:
							if(NULL != CallBack[KEY_INS])
							{
								CallBack[KEY_INS](KEY_INS, cBuff,&resp);
								if(resp)
								{
									free(resp);
									resp = NULL;
								}
							}	
							else
								ins = ins? false : true; //toggle insert mode
							getchar(); //get the closing character form escape sequence and discard
						break;
						case ID_KEY_DEL:
							if(NULL != CallBack[KEY_DEL])
							{
								CallBack[KEY_DEL](KEY_DEL, cBuff,&resp);
								if(resp)
								{
									free(resp);
									resp = NULL;
								}
							}	
							else
							{
								if(pos <len)
								{
									i = pos;
									while(i<len-1)
									{
										cBuff[i]=cBuff[i+1];
										i++;
									}
									cBuff[i]='\0';
									printf("\033[s");
									if(pos>0)
										printf("\033[%dD",pos);
									printf("\033[K%s\033[u",cBuff);
									len --;
								}
							}
							getchar(); //get the closing character form escape sequence and discard
						break;
						case ID_KEY_PGUP:
							if(NULL != CallBack[KEY_PGUP])
							{
								CallBack[KEY_PGUP](KEY_PGUP, cBuff,&resp);
								if(resp)
								{
									free(resp);
									resp = NULL;
								}
							}	
							getchar(); //get the closing character form escape sequence and discard
						break;
						case ID_KEY_PGDN:
							if(NULL != CallBack[KEY_PGDN])
							{
								CallBack[KEY_PGDN](KEY_PGDN, cBuff,&resp);
								if(resp)
								{
									free(resp);
									resp = NULL;
								}
							}
							getchar(); //get the closing character form escape sequence and discard
						break;
						default:
							continue;
					}
				break;
				case ID_KEY_BACKSPACE:
					if(NULL != CallBack[KEY_BACKSPACE])
					{
						CallBack[KEY_BACKSPACE](KEY_BACKSPACE, cBuff,&resp);
						if(resp)
						{
							free(resp);
							resp = NULL;
						}
					}
					else
					{
						if(pos > 0)
						{
							i = pos-1;
							while(i<len-1)
							{
								cBuff[i]=cBuff[i+1];
								i++;
							}
							cBuff[i]='\0';
							pos --;
							//printf("\b\033[s\033[%dD\033[K%s\033[u",pos,cBuff);
							printf("\b\033[s");
							if(pos>0)
								printf("\033[%dD",pos);
							printf("\033[K%s\033[u",cBuff);
							len --;
						}
					}
				break;
				default: //normal char insert
					if(len < MAX_CHAR)
					{
						if(ins)
						{
							i=pos;
							prev = cBuff[i];
							while(i<len)
							{
								nxt = cBuff[i+1];
								cBuff[i+1]=prev;
								prev = nxt;
								i++;
							}
						}
						cBuff[pos] = ch;
						pos ++;
						//printf("%d::%s\n",pos,cBuff);
						printf("\033[C\033[s\033[%dD\033[K%s\033[u",pos,cBuff);
						if(ins || pos >= len)
							len ++;  
					}
				break;
			}
			//printf("\033[s\033[0;0H\033[Kpos:%d::len:%d::buff:%s\033[u",pos,len,cBuff);
		}
	}
}

bool UIMgr::UI_Start()
{
    if(UI_isRunning())
        return false;
    UI_State.lock();
    Runner = new std::thread(&UIMgr::UI_Engine, this);
    return true;
}

bool UIMgr::UI_Stop()
{
    if(UI_isRunning())
    {
        UI_State.unlock();
        Runner->join();
        return true;
    }
    return false;
}
