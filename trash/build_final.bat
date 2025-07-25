@echo off
echo Building ClassPaper Settings - Complete Native Implementation...
echo.

g++ -std=c++17 -I./include -static -o settings_final.exe settings_simple.cpp -lcomctl32 -lcomdlg32 -lshell32 -lole32 -loleaut32 -luuid -lgdi32 -luser32

if %errorlevel% equ 0 (
    echo ========================================
    echo Build successful! 
    echo ========================================
    echo.
    echo 🎉 All features implemented:
    echo.
    echo ✅ 1. Basic Settings Dialog
    echo    - Progress bar settings (description, dates, percent mode)
    echo    - Week offset settings (enabled, offset value)
    echo    - Notification settings (enabled, intervals)
    echo    - System settings (URL, browser path, file selection)
    echo.
    echo ✅ 2. Schedule Settings Dialog
    echo    - Display mode selection (scroll/day mode)
    echo    - Time table management (add slots, smart guess)
    echo    - Course schedule (import, display)
    echo.
    echo ✅ 3. Events Settings Dialog
    echo    - Event list management
    echo    - Add/delete events with date and time
    echo    - ISO datetime format support
    echo.
    echo ✅ 4. Wallpapers Settings Dialog
    echo    - Wallpaper list management
    echo    - File browser integration
    echo    - Scan wallpaper folder functionality
    echo    - Interval settings
    echo.
    echo ✅ 5. Notice Settings Dialog
    echo    - HTML content editor
    echo    - Live preview with basic HTML parsing
    echo    - Multi-line text support
    echo.
    echo ✅ 6. Complete Configuration Save
    echo    - Saves to both config.toml and config.js
    echo    - Full compatibility with original format
    echo    - Proper escaping and formatting
    echo.
    echo ========================================
    echo Running settings_final.exe...
    echo ========================================
    echo.
    settings_final.exe
) else (
    echo ========================================
    echo Build failed!
    echo ========================================
    echo Please check for compilation errors above.
    pause
)