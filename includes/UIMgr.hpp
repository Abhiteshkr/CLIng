#include <cstdlib>
#include <iostream>
#include <mutex>
#include <thread>
#include <cstring>
#include <termios.h>
#include <unistd.h>

#define MAX_COMMAND_SIZE 50

#define KEY_TAB 		0
#define KEY_ENTER 		1
#define KEY_ESC			2
#define KEY_INS			3
#define KEY_DEL			4
#define KEY_PGUP		5
#define KEY_PGDN		6
#define KEY_UP			7
#define KEY_DOWN		8
#define KEY_RIGHT		9
#define KEY_LEFT		10
#define KEY_END			11
#define KEY_HOME		12
#define KEY_BACKSPACE	13
#define MAX_KEY			14

typedef void (*CB_KeyHandler)(int key, const char *payload, char **response);

class UIMgr
{
    public:
        UIMgr();
        ~UIMgr();
        bool UI_Start();
        bool UI_Stop();
        bool RegisterKeyHandler(int uKey,CB_KeyHandler keyHandler);
        bool SetPrompt(const char* prompt);
    private:
        std::thread* Runner;
        std::mutex UI_State;
        void UI_Engine();
        CB_KeyHandler CallBack[MAX_KEY];
        bool UI_isRunning();
        char* Prompt;
		struct termios oldt,newt;
};
