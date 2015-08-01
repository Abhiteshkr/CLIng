CC=g++
CFLAGS=-c -Wall -std=c++11
LDFLAGS=-pthread -lstdc++
EXECUTABLE=batsim
SOURCE_DIR=src
INCLUDES=-I includes

SOURCES=\
	$(SOURCE_DIR)/UIMgr.cpp



OBJECTS=$(SOURCES:.cpp=.o)

all: clean build

build: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@ $(LINKFLAGS)

clean:
	rm -fr *.o $(EXECUTABLE)
