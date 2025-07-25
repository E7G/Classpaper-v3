@echo off
title ClassPaper Fluent Design Settings - Final Version
echo.
echo ========================================
echo   ClassPaper Fluent Design 设置程序
echo ========================================
echo.

echo 正在编译最终版本...
g++ -std=c++17 -I./include -static -finput-charset=UTF-8 -fexec-charset=UTF-8 -o settings_fluent.exe settings_fluent_simple.cpp -lcomctl32 -lcomdlg32 -lshell32 -luser32 -lgdi32 -lole32

if %errorlevel% equ 0 (
    echo.
    echo ========================================
    echo ✅ 编译成功！Fluent Design UI 已就绪
    echo ========================================
    echo.
    echo 🎨 Fluent Design 特性:
    echo ✅ Windows 11 现代化界面风格
    echo ✅ 可调整大小的对话框
    echo ✅ 完美的中文显示支持
    echo ✅ 原生 Windows API 实现
    echo.
    echo 🔧 完整功能模块:
    echo ✅ 基本设置 - 进度条、周数、提示音、系统配置
    echo ✅ 课程表设置 - 显示模式、时间安排、课程管理
    echo ✅ 事件管理 - 添加、编辑、删除重要事件
    echo ✅ 壁纸管理 - 列表管理、文件夹扫描、间隔设置
    echo ✅ 告示牌设置 - HTML编辑、预览、快速插入
    echo.
    echo 🚀 配置管理功能:
    echo ✅ 启动时自动加载本地配置
    echo ✅ 运行时配置重载功能
    echo ✅ 配置状态显示和概览
    echo ✅ 自动创建默认配置文件
    echo.
    echo ========================================
    echo 正在启动 Fluent Design 设置程序...
    echo ========================================
    echo.
    
    REM 创建配置文件（如果不存在）
    if not exist "config.toml" (
        echo 创建默认配置文件...
        echo [default] > config.toml
        echo url = "./res/index.html" >> config.toml
        echo browser_path = "" >> config.toml
        echo. >> config.toml
        echo [progress] >> config.toml
        echo description = "高三剩余" >> config.toml
        echo begin = "2024-09-01T00:00:00" >> config.toml
        echo end = "2025-06-07T23:59:59" >> config.toml
        echo mode = "left" >> config.toml
        echo. >> config.toml
        echo [weekOffset] >> config.toml
        echo enabled = true >> config.toml
        echo offset = 0 >> config.toml
        echo. >> config.toml
        echo [notifications] >> config.toml
        echo enabled = true >> config.toml
        echo interval = 45 >> config.toml
        echo endingTime = 5 >> config.toml
        echo. >> config.toml
        echo [lessons] >> config.toml
        echo mode = "scroll" >> config.toml
        echo timeSlots = ["07:30-08:10", "08:20-09:00", "09:10-09:50", "10:10-10:50", "11:00-11:40", "14:00-14:40", "14:50-15:30", "15:50-16:30"] >> config.toml
        echo schedule = [["语文", "数学", "英语", "物理"], ["数学", "语文", "物理", "化学"], ["英语", "物理", "数学", "语文"], ["物理", "化学", "语文", "数学"], ["化学", "英语", "生物", "物理"]] >> config.toml
        echo. >> config.toml
        echo [wallpapers] >> config.toml
        echo list = ["wallpaper/bg1.jpg", "wallpaper/bg2.jpg"] >> config.toml
        echo interval = 30 >> config.toml
        echo. >> config.toml
        echo [[events]] >> config.toml
        echo name = "期中考试" >> config.toml
        echo date = "2024-11-15" >> config.toml
        echo time = "09:00" >> config.toml
        echo. >> config.toml
        echo [[events]] >> config.toml
        echo name = "家长会" >> config.toml
        echo date = "2024-12-01" >> config.toml
        echo time = "14:00" >> config.toml
        echo. >> config.toml
        echo notice = "^<h3^>欢迎使用 ClassPaper^</h3^>^<p^>这是一个现代化的课程表壁纸程序。^</p^>^<ul^>^<li^>支持自定义课程表^</li^>^<li^>支持壁纸轮播^</li^>^<li^>支持事件提醒^</li^>^</ul^>" >> config.toml
        echo.
        echo ✅ 默认配置文件创建完成
        echo.
    )
    
    settings_fluent.exe
    
    echo.
    echo ========================================
    echo 感谢使用 ClassPaper Fluent Design UI！
    echo ========================================
    
) else (
    echo.
    echo ========================================
    echo ❌ 编译失败！
    echo ========================================
    echo.
    echo 请检查以下问题:
    echo 1. 确保 include/toml.hpp 文件存在
    echo 2. 检查编译器是否支持 C++17
    echo 3. 确认所有依赖库已安装
    echo.
    pause
)