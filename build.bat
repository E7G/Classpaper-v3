@echo off
set CFLAGS=-O3 -finput-charset=UTF-8 -fexec-charset=GBK -std=c++17 -Wall -I"include" -L"lib"
set LIBS_STATIC=-lwebui-2-static -luser32 -lole32 -lws2_32 -lAdvapi32 -lShell32 -static
set LIBS_DYNAMIC="lib/webui-2.dll" -luser32 -lole32 -lws2_32

chcp 65001 > nul

:: 新增设置界面编译目标
if "%1" == "settings" (
    echo ===== 正在编译设置界面应用 =====
    g++ %CFLAGS% settings.cpp -o settings.exe %LIBS_STATIC%  -lcomdlg32 -Wl,-subsystem=windows
    rem    -Wl,-subsystem=windows
    if exist settings.exe (
        echo 编译成功！
    ) else (
        echo 编译失败
    )
    goto end
)

rem 静态链接构建
g++ %CFLAGS% main.cpp -o main.exe %LIBS_STATIC% -Wl,-subsystem=windows || goto error

rem 动态链接构建
g++ %CFLAGS% main.cpp -o main-dyn.exe %LIBS_DYNAMIC% -Wl,-subsystem=windows || goto error

rem 调试版构建
g++ -O0 -g -DDEBUG -finput-charset=UTF-8 -fexec-charset=GBK -std=c++17 -Wall -I"include" -L"lib" main.cpp -o main-debug.exe %LIBS_STATIC% || goto error

:success
del *.o >nul 2>&1
echo 编译成功！
pause
exit /b 0

:error
echo 编译失败，请检查错误信息！
pause
exit /b 1

:end