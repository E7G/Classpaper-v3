g++ -O3 -finput-charset=UTF-8 -fexec-charset=GBK -std=c++17 -lstdc++ main.cpp -lole32 -I"include/" -L"lib/" -lwebui-2-static -lws2_32 -Wall -Wl,-subsystem=windows -luser32 -static -lws2_32 -o main.exe
g++ -O3 -finput-charset=UTF-8 -fexec-charset=GBK -std=c++17 -lstdc++ main.cpp -lole32 -I"include/" -L"lib/" "lib/webui-2.dll" -lws2_32 -Wall -Wl,-subsystem=windows -luser32 -lws2_32 -o main-dyn.exe
@REM g++ -Os -std=c++17 -lstdc++ main.cpp -lole32 -I"include/" -L"lib/" -lwebui-2-static -lws2_32 -Wall -Wl,-subsystem=windows -luser32 -static -lws2_32 -o main-min.exe
@REM g++ -Os -std=c++17 -lstdc++ main.cpp -lole32 -I"include/" -L"lib/" "lib/webui-2.dll" -lws2_32 -Wall -Wl,-subsystem=windows -luser32 -lws2_32 -o main-min-dyn.exe
@echo off
del *.o >nul 2>&1
echo "Done."