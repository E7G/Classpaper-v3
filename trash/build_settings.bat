@echo off
echo Building ClassPaper Settings Program...

REM Check if compiler exists
where g++ >nul 2>nul
if %errorlevel% neq 0 (
    echo Error: g++ compiler not found. Please install MinGW-w64 or similar.
    pause
    exit /b 1
)

REM Create build directory if it doesn't exist
if not exist "build" mkdir build

REM Compile the settings program
echo Compiling settings.cpp...
g++ -std=c++17 ^
    -I./include ^
    -L./lib ^
    -static ^
    -o settings.exe ^
    settings.cpp ^
    -lwebui-2-static ^
    -lws2_32 ^
    -lwinmm ^
    -lgdi32 ^
    -lole32 ^
    -loleaut32 ^
    -luuid ^
    -lcomctl32 ^
    -lcomdlg32

if %errorlevel% equ 0 (
    echo.
    echo ✓ Settings program compiled successfully!
    echo ✓ Output: settings.exe
    echo.
    echo You can now run the settings program by executing: settings.exe
) else (
    echo.
    echo ✗ Compilation failed!
    echo Please check the error messages above.
)

pause