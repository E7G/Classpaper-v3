CC = g++
CFLAGS = -O3 -finput-charset=UTF-8 -fexec-charset=GBK -std=c++17 -Wall -Iinclude
LDFLAGS = -Llib -Wl,-subsystem=windows
LIBS_STATIC = -lwebui-2-static -luser32 -lole32 -lws2_32 -static
LIBS_DYNAMIC = lib/webui-2.dll -luser32 -lole32 -lws2_32

.PHONY: all clean settings

all: main.exe main-dyn.exe main-debug.exe

settings: settings.exe settings.exe

main.exe: main.cpp
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) $(LIBS_STATIC)

main-dyn.exe: main.cpp
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) $(LIBS_DYNAMIC)

main-debug.exe: CFLAGS += -O0 -g -DDEBUG
main-debug.exe: main.cpp
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) $(LIBS_STATIC)

settings.exe: settings.cpp
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) $(LIBS_STATIC)

clean:
	del /F /Q *.exe *.o 2>nul