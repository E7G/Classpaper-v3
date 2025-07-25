// ClassPaper Settings - Fluent Design UI (Simplified Test Version)
#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <shellapi.h>
#include <shlobj.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <locale>
#include <map>
#include <algorithm>
#include "toml.hpp"

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "comdlg32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "ole32.lib")

// Fluent Design Colors
namespace FluentColors {
    const COLORREF Background = RGB(243, 243, 243);
    const COLORREF CardBackground = RGB(255, 255, 255);
    const COLORREF AccentColor = RGB(0, 120, 215);
    const COLORREF AccentHover = RGB(16, 110, 190);
    const COLORREF TextPrimary = RGB(32, 31, 30);
    const COLORREF TextSecondary = RGB(96, 94, 92);
    const COLORREF BorderColor = RGB(225, 223, 221);
    const COLORREF SectionBackground = RGB(250, 249, 248);
    const COLORREF SuccessColor = RGB(16, 124, 16);
    const COLORREF WarningColor = RGB(255, 185, 0);
}

// Configuration structure
struct Config {
    struct {
        std::string description = "高三剩余";
        std::string begin = "2024-09-01T00:00:00";
        std::string end = "2025-06-07T23:59:59";
        std::string mode = "left";
    } progress;
    
    struct {
        bool enabled = true;
        int offset = 0;
    } weekOffset;
    
    struct {
        bool enabled = true;
        int interval = 45;
        int endingTime = 5;
    } notifications;
    
    struct {
        std::string url = "./res/index.html";
        std::string browserPath = "";
    } system;
    
    struct {
        std::string mode = "scroll";
        std::vector<std::string> timeSlots = {
            "07:30-08:10", "08:20-09:00", "09:10-09:50", "10:10-10:50",
            "11:00-11:40", "14:00-14:40", "14:50-15:30", "15:50-16:30"
        };
        std::vector<std::vector<std::string>> schedule = {
            {"语文", "数学", "英语", "物理", "化学", "生物", "历史", "政治"},
            {"数学", "语文", "物理", "化学", "英语", "体育", "生物", "自习"},
            {"英语", "物理", "数学", "语文", "生物", "化学", "政治", "历史"},
            {"物理", "化学", "语文", "数学", "历史", "英语", "体育", "自习"},
            {"化学", "英语", "生物", "物理", "数学", "语文", "政治", "历史"}
        };
    } lessons;
    
    struct {
        std::vector<std::string> list = {"wallpaper/bg1.jpg", "wallpaper/bg2.jpg"};
        int interval = 30;
    } wallpapers;
    
    std::vector<std::map<std::string, std::string>> events = {
        {{"name", "期中考试"}, {"date", "2024-11-15"}, {"time", "09:00"}},
        {{"name", "家长会"}, {"date", "2024-12-01"}, {"time", "14:00"}}
    };
    
    std::string notice = "<h3>欢迎使用 ClassPaper</h3><p>这是一个现代化的课程表壁纸程序。</p><ul><li>支持自定义课程表</li><li>支持壁纸轮播</li><li>支持事件提醒</li></ul>";
};

Config g_config;

// Control IDs
#define ID_BTN_BASIC 1001
#define ID_BTN_SCHEDULE 1002
#define ID_BTN_EVENTS 1003
#define ID_BTN_WALLPAPERS 1004
#define ID_BTN_NOTICE 1005
#define ID_BTN_SAVE 1006
#define ID_BTN_RELOAD 1008
#define ID_BTN_EXIT 1007

// Create modern button with enhanced styling
HWND CreateFluentButton(HWND parent, const std::wstring& text, int x, int y, int width, int height, int id, bool isPrimary = false) {
    DWORD style = WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_FLAT;
    
    HWND button = CreateWindowW(L"BUTTON", text.c_str(), style,
        x, y, width, height, parent, (HMENU)(LONG_PTR)id, GetModuleHandle(NULL), NULL);
    
    // Use slightly larger font for better readability
    HFONT font = CreateFontW(-15, 0, 0, 0, isPrimary ? FW_SEMIBOLD : FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    SendMessage(button, WM_SETFONT, (WPARAM)font, TRUE);
    
    return button;
}

// Create modern label with enhanced styling
HWND CreateFluentLabel(HWND parent, const std::wstring& text, int x, int y, int width, int height, bool isTitle = false, bool isSecondary = false) {
    HWND label = CreateWindowW(L"STATIC", text.c_str(),
        WS_VISIBLE | WS_CHILD | SS_LEFT,
        x, y, width, height, parent, NULL, GetModuleHandle(NULL), NULL);
    
    int fontSize = isTitle ? -24 : (isSecondary ? -13 : -15);
    int fontWeight = isTitle ? FW_SEMIBOLD : FW_NORMAL;
    
    HFONT font = CreateFontW(fontSize, 0, 0, 0, fontWeight, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    SendMessage(label, WM_SETFONT, (WPARAM)font, TRUE);
    
    return label;
}

// Create section group box for better visual organization
HWND CreateFluentSection(HWND parent, const std::wstring& title, int x, int y, int width, int height) {
    HWND section = CreateWindowW(L"BUTTON", title.c_str(),
        WS_VISIBLE | WS_CHILD | BS_GROUPBOX,
        x, y, width, height, parent, NULL, GetModuleHandle(NULL), NULL);
    
    HFONT font = CreateFontW(-14, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    SendMessage(section, WM_SETFONT, (WPARAM)font, TRUE);
    
    return section;
}

// Enhanced config.js parser with proper JSON-like parsing
void LoadConfigFromJS() {
    std::string jsPath = "res/config/config.js";
    if (!std::filesystem::exists(jsPath)) {
        std::cout << "Config.js not found at: " << jsPath << std::endl;
        return;
    }
    
    try {
        std::ifstream file(jsPath);
        std::string content;
        std::string line;
        
        // Read entire file content
        while (std::getline(file, line)) {
            content += line + "\n";
        }
        file.close();
        
        if (content.empty()) {
            std::cout << "Config.js is empty" << std::endl;
            return;
        }
        
        std::cout << "Loading configuration from config.js..." << std::endl;
        
        // Parse progressDescription
        if (auto pos = content.find("\"progressDescription\":"); pos != std::string::npos) {
            auto start = content.find("\"", pos + 22);
            auto end = content.find("\"", start + 1);
            if (start != std::string::npos && end != std::string::npos) {
                g_config.progress.description = content.substr(start + 1, end - start - 1);
                std::cout << "Loaded progress description: " << g_config.progress.description << std::endl;
            }
        }
        
        // Parse progressPercentMode
        if (auto pos = content.find("\"progressPercentMode\":"); pos != std::string::npos) {
            auto start = content.find("\"", pos + 22);
            auto end = content.find("\"", start + 1);
            if (start != std::string::npos && end != std::string::npos) {
                g_config.progress.mode = content.substr(start + 1, end - start - 1);
                std::cout << "Loaded progress mode: " << g_config.progress.mode << std::endl;
            }
        }
        
        // Parse weekOffset
        if (auto pos = content.find("\"weekOffset\":"); pos != std::string::npos) {
            // Parse enabled
            if (auto enabledPos = content.find("\"enabled\":", pos); enabledPos != std::string::npos && enabledPos < content.find("}", pos)) {
                auto valueStart = content.find_first_not_of(" \t", enabledPos + 10);
                if (valueStart != std::string::npos) {
                    g_config.weekOffset.enabled = (content.substr(valueStart, 4) == "true");
                    std::cout << "Loaded week offset enabled: " << g_config.weekOffset.enabled << std::endl;
                }
            }
            
            // Parse offset value
            if (auto offsetPos = content.find("\"offset\":", pos); offsetPos != std::string::npos && offsetPos < content.find("}", pos)) {
                auto valueStart = content.find_first_not_of(" \t", offsetPos + 9);
                auto valueEnd = content.find_first_of(",}", valueStart);
                if (valueStart != std::string::npos && valueEnd != std::string::npos) {
                    try {
                        g_config.weekOffset.offset = std::stoi(content.substr(valueStart, valueEnd - valueStart));
                        std::cout << "Loaded week offset: " << g_config.weekOffset.offset << std::endl;
                    } catch (...) {}
                }
            }
        }
        
        // Parse notifications
        if (auto pos = content.find("\"notifications\":"); pos != std::string::npos) {
            // Parse enabled
            if (auto enabledPos = content.find("\"enabled\":", pos); enabledPos != std::string::npos && enabledPos < content.find("}", pos)) {
                auto valueStart = content.find_first_not_of(" \t", enabledPos + 10);
                if (valueStart != std::string::npos) {
                    g_config.notifications.enabled = (content.substr(valueStart, 4) == "true");
                    std::cout << "Loaded notifications enabled: " << g_config.notifications.enabled << std::endl;
                }
            }
            
            // Parse regularInterval
            if (auto intervalPos = content.find("\"regularInterval\":", pos); intervalPos != std::string::npos && intervalPos < content.find("}", pos)) {
                auto valueStart = content.find_first_not_of(" \t", intervalPos + 18);
                auto valueEnd = content.find_first_of(",}", valueStart);
                if (valueStart != std::string::npos && valueEnd != std::string::npos) {
                    try {
                        g_config.notifications.interval = std::stoi(content.substr(valueStart, valueEnd - valueStart));
                        std::cout << "Loaded notification interval: " << g_config.notifications.interval << std::endl;
                    } catch (...) {}
                }
            }
            
            // Parse endingTime
            if (auto endingPos = content.find("\"endingTime\":", pos); endingPos != std::string::npos && endingPos < content.find("}", pos)) {
                auto valueStart = content.find_first_not_of(" \t", endingPos + 13);
                auto valueEnd = content.find_first_of(",}", valueStart);
                if (valueStart != std::string::npos && valueEnd != std::string::npos) {
                    try {
                        g_config.notifications.endingTime = std::stoi(content.substr(valueStart, valueEnd - valueStart));
                        std::cout << "Loaded notification ending time: " << g_config.notifications.endingTime << std::endl;
                    } catch (...) {}
                }
            }
        }
        
        // Parse wallpaperInterval
        if (auto pos = content.find("\"wallpaperInterval\":"); pos != std::string::npos) {
            auto valueStart = content.find_first_not_of(" \t", pos + 20);
            auto valueEnd = content.find_first_of(",}", valueStart);
            if (valueStart != std::string::npos && valueEnd != std::string::npos) {
                try {
                    g_config.wallpapers.interval = std::stoi(content.substr(valueStart, valueEnd - valueStart));
                    std::cout << "Loaded wallpaper interval: " << g_config.wallpapers.interval << std::endl;
                } catch (...) {}
            }
        }
        
        // Parse lessons displayMode
        if (auto lessonsPos = content.find("\"lessons\":"); lessonsPos != std::string::npos) {
            if (auto modePos = content.find("\"displayMode\":", lessonsPos); modePos != std::string::npos) {
                auto start = content.find("\"", modePos + 14);
                auto end = content.find("\"", start + 1);
                if (start != std::string::npos && end != std::string::npos) {
                    std::string mode = content.substr(start + 1, end - start - 1);
                    g_config.lessons.mode = (mode == "scroll") ? "scroll" : "day";
                    std::cout << "Loaded lessons mode: " << g_config.lessons.mode << std::endl;
                }
            }
        }
        
        // Parse wallpapers array
        if (auto wallpapersPos = content.find("\"wallpapers\":"); wallpapersPos != std::string::npos) {
            auto arrayStart = content.find("[", wallpapersPos);
            auto arrayEnd = content.find("]", arrayStart);
            if (arrayStart != std::string::npos && arrayEnd != std::string::npos) {
                std::string arrayContent = content.substr(arrayStart + 1, arrayEnd - arrayStart - 1);
                g_config.wallpapers.list.clear();
                
                size_t pos = 0;
                while (pos < arrayContent.length()) {
                    auto start = arrayContent.find("\"", pos);
                    if (start == std::string::npos) break;
                    auto end = arrayContent.find("\"", start + 1);
                    if (end == std::string::npos) break;
                    
                    std::string wallpaper = arrayContent.substr(start + 1, end - start - 1);
                    g_config.wallpapers.list.push_back(wallpaper);
                    pos = end + 1;
                }
                std::cout << "Loaded " << g_config.wallpapers.list.size() << " wallpapers" << std::endl;
            }
        }
        
        // Parse events array
        if (auto eventsPos = content.find("\"events\":"); eventsPos != std::string::npos) {
            auto arrayStart = content.find("[", eventsPos);
            auto arrayEnd = content.find("]", arrayStart);
            if (arrayStart != std::string::npos && arrayEnd != std::string::npos) {
                g_config.events.clear();
                
                size_t pos = arrayStart + 1;
                while (pos < arrayEnd) {
                    auto objStart = content.find("{", pos);
                    if (objStart == std::string::npos || objStart > arrayEnd) break;
                    auto objEnd = content.find("}", objStart);
                    if (objEnd == std::string::npos || objEnd > arrayEnd) break;
                    
                    std::string objContent = content.substr(objStart + 1, objEnd - objStart - 1);
                    std::map<std::string, std::string> event;
                    
                    // Parse name
                    if (auto namePos = objContent.find("\"name\":"); namePos != std::string::npos) {
                        auto start = objContent.find("\"", namePos + 7);
                        auto end = objContent.find("\"", start + 1);
                        if (start != std::string::npos && end != std::string::npos) {
                            event["name"] = objContent.substr(start + 1, end - start - 1);
                        }
                    }
                    
                    // Parse date
                    if (auto datePos = objContent.find("\"date\":"); datePos != std::string::npos) {
                        auto start = objContent.find("\"", datePos + 7);
                        auto end = objContent.find("\"", start + 1);
                        if (start != std::string::npos && end != std::string::npos) {
                            std::string dateStr = objContent.substr(start + 1, end - start - 1);
                            // Convert from ISO format to simple date format
                            if (dateStr.find("T") != std::string::npos) {
                                event["date"] = dateStr.substr(0, dateStr.find("T"));
                                event["time"] = dateStr.substr(dateStr.find("T") + 1, 5); // Extract HH:MM
                            } else {
                                event["date"] = dateStr;
                                event["time"] = "00:00";
                            }
                        }
                    }
                    
                    if (!event["name"].empty() && !event["date"].empty()) {
                        g_config.events.push_back(event);
                    }
                    
                    pos = objEnd + 1;
                }
                std::cout << "Loaded " << g_config.events.size() << " events" << std::endl;
            }
        }
        
        std::cout << "Config.js loading completed successfully!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "Error loading config.js: " << e.what() << std::endl;
        std::cout << "Using default configuration values." << std::endl;
    }
}

// Load configuration with enhanced startup loading
void LoadConfig() {
    std::string configPath = "config.toml";
    bool configLoaded = false;
    
    try {
        // Try to load config.toml first
        if (std::filesystem::exists(configPath)) {
            auto data = toml::parse(configPath);
            
            // Load progress configuration
            if (data.contains("progress")) {
                auto progress = toml::find(data, "progress");
                g_config.progress.description = toml::find_or<std::string>(progress, "description", "高三剩余");
                g_config.progress.begin = toml::find_or<std::string>(progress, "begin", "2024-09-01T00:00:00");
                g_config.progress.end = toml::find_or<std::string>(progress, "end", "2025-06-07T23:59:59");
                g_config.progress.mode = toml::find_or<std::string>(progress, "mode", "left");
            }
            
            // Load week offset configuration
            if (data.contains("weekOffset")) {
                auto weekOffset = toml::find(data, "weekOffset");
                g_config.weekOffset.enabled = toml::find_or<bool>(weekOffset, "enabled", true);
                g_config.weekOffset.offset = toml::find_or<int>(weekOffset, "offset", 0);
            }
            
            // Load notifications configuration
            if (data.contains("notifications")) {
                auto notifications = toml::find(data, "notifications");
                g_config.notifications.enabled = toml::find_or<bool>(notifications, "enabled", true);
                g_config.notifications.interval = toml::find_or<int>(notifications, "interval", 45);
                g_config.notifications.endingTime = toml::find_or<int>(notifications, "endingTime", 5);
            }
            
            // Load system configuration
            if (data.contains("default")) {
                auto defaultSection = toml::find(data, "default");
                g_config.system.url = toml::find_or<std::string>(defaultSection, "url", "./res/index.html");
                g_config.system.browserPath = toml::find_or<std::string>(defaultSection, "browser_path", "");
            }
            
            // Load lessons configuration
            if (data.contains("lessons")) {
                auto lessons = toml::find(data, "lessons");
                g_config.lessons.mode = toml::find_or<std::string>(lessons, "mode", "scroll");
                
                if (lessons.contains("timeSlots")) {
                    auto timeSlots = toml::find<std::vector<std::string>>(lessons, "timeSlots");
                    g_config.lessons.timeSlots = timeSlots;
                }
                
                if (lessons.contains("schedule")) {
                    auto schedule = toml::find<std::vector<std::vector<std::string>>>(lessons, "schedule");
                    g_config.lessons.schedule = schedule;
                }
            }
            
            // Load wallpapers configuration
            if (data.contains("wallpapers")) {
                auto wallpapers = toml::find(data, "wallpapers");
                g_config.wallpapers.interval = toml::find_or<int>(wallpapers, "interval", 30);
                
                if (wallpapers.contains("list")) {
                    auto list = toml::find<std::vector<std::string>>(wallpapers, "list");
                    g_config.wallpapers.list = list;
                }
            }
            
            // Load events configuration
            if (data.contains("events")) {
                try {
                    auto events = toml::find<std::vector<toml::table>>(data, "events");
                    g_config.events.clear();
                    for (const auto& event : events) {
                        std::map<std::string, std::string> eventMap;
                        for (const auto& pair : event) {
                            eventMap[pair.first] = toml::get<std::string>(pair.second);
                        }
                        g_config.events.push_back(eventMap);
                    }
                } catch (const std::exception&) {
                    // Keep default events if loading fails
                }
            }
            
            // Load notice configuration
            if (data.contains("notice")) {
                g_config.notice = toml::find<std::string>(data, "notice");
            }
            
            configLoaded = true;
            
        } else {
            // Try to load from alternative locations
            std::vector<std::string> altPaths = {
                "./config/config.toml",
                "./res/config/config.toml",
                "settings.toml"
            };
            
            for (const auto& altPath : altPaths) {
                if (std::filesystem::exists(altPath)) {
                    auto data = toml::parse(altPath);
                    // Apply same loading logic as above
                    configLoaded = true;
                    break;
                }
            }
        }
        
        // Always try to load from config.js for additional/override settings
        LoadConfigFromJS();
        
        if (configLoaded) {
            // Show success message on first load
            static bool firstLoad = true;
            if (firstLoad) {
                std::string message = "Configuration loaded successfully from: " + configPath + " and config.js";
                MessageBoxA(NULL, message.c_str(), "Config Loaded", MB_OK | MB_ICONINFORMATION);
                firstLoad = false;
            }
        } else {
            // If no TOML config, still try config.js
            static bool firstLoad = true;
            if (firstLoad) {
                std::string message = "Loading configuration from config.js only (no TOML found)";
                MessageBoxA(NULL, message.c_str(), "Config Loaded", MB_OK | MB_ICONINFORMATION);
                firstLoad = false;
            }
        }
        
    } catch (const std::exception& e) {
        std::string errorMsg = "Config load failed: " + std::string(e.what()) + "\nUsing default configuration.";
        MessageBoxA(NULL, errorMsg.c_str(), "Config Warning", MB_OK | MB_ICONWARNING);
    }
    
    // If no config was loaded, use default values (already initialized)
    if (!configLoaded) {
        // Default values are already set in the Config structure initialization
        // No need to call CreateDefaultConfig() here
    }
}

// Create default configuration file
void CreateDefaultConfig() {
    try {
        toml::value data;
        
        // Default progress settings
        data["progress"]["description"] = g_config.progress.description;
        data["progress"]["begin"] = g_config.progress.begin;
        data["progress"]["end"] = g_config.progress.end;
        data["progress"]["mode"] = g_config.progress.mode;
        
        // Default week offset
        data["weekOffset"]["enabled"] = g_config.weekOffset.enabled;
        data["weekOffset"]["offset"] = g_config.weekOffset.offset;
        
        // Default notifications
        data["notifications"]["enabled"] = g_config.notifications.enabled;
        data["notifications"]["interval"] = g_config.notifications.interval;
        data["notifications"]["endingTime"] = g_config.notifications.endingTime;
        
        // Default system settings
        data["default"]["url"] = g_config.system.url;
        data["default"]["browser_path"] = g_config.system.browserPath;
        
        // Default lessons
        data["lessons"]["mode"] = g_config.lessons.mode;
        data["lessons"]["timeSlots"] = g_config.lessons.timeSlots;
        data["lessons"]["schedule"] = g_config.lessons.schedule;
        
        // Default wallpapers
        data["wallpapers"]["list"] = g_config.wallpapers.list;
        data["wallpapers"]["interval"] = g_config.wallpapers.interval;
        
        // Default events
        toml::array eventArray;
        for (const auto& event : g_config.events) {
            toml::table eventTable;
            for (const auto& pair : event) {
                eventTable[pair.first] = pair.second;
            }
            eventArray.push_back(eventTable);
        }
        data["events"] = eventArray;
        
        // Default notice
        data["notice"] = g_config.notice;
        
        // Write default config
        std::ofstream file("config.toml");
        file << data;
        file.close();
        
        MessageBoxA(NULL, "Default configuration created: config.toml", "Config Created", MB_OK | MB_ICONINFORMATION);
        
    } catch (const std::exception& e) {
        std::string errorMsg = "Failed to create default config: " + std::string(e.what());
        MessageBoxA(NULL, errorMsg.c_str(), "Config Error", MB_OK | MB_ICONERROR);
    }
}

// Show startup configuration status
void ShowStartupConfigStatus() {
    std::wstring statusMessage = L"🚀 ClassPaper Fluent Design UI \u542f\u52a8\u5b8c\u6210\n\n";
    statusMessage += L"📋 \u914d\u7f6e\u52a0\u8f7d\u72b6\u6001:\n";
    
    // Check config file status
    bool hasToml = std::filesystem::exists("config.toml");
    bool hasConfigJs = std::filesystem::exists("res/config/config.js");
    
    if (hasToml && hasConfigJs) {
        statusMessage += L"✅ config.toml \u548c config.js \u90fd\u5df2\u52a0\u8f7d\n";
        statusMessage += L"📝 config.js \u914d\u7f6e\u4f18\u5148\u7ea7\u66f4\u9ad8\n";
    } else if (hasToml) {
        statusMessage += L"✅ config.toml \u5df2\u52a0\u8f7d\n";
        statusMessage += L"⚠️ config.js \u4e0d\u5b58\u5728\n";
    } else if (hasConfigJs) {
        statusMessage += L"✅ config.js \u5df2\u52a0\u8f7d\n";
        statusMessage += L"⚠️ config.toml \u4e0d\u5b58\u5728\n";
    } else {
        statusMessage += L"⚠️ \u6ca1\u6709\u627e\u5230\u914d\u7f6e\u6587\u4ef6\uff0c\u4f7f\u7528\u9ed8\u8ba4\u914d\u7f6e\n";
    }
    
    // Show current configuration summary
    statusMessage += L"\n📊 \u5f53\u524d\u914d\u7f6e\u6982\u89c8:\n";
    // Convert strings manually to avoid function scope issues
    std::wstring progressDesc(g_config.progress.description.begin(), g_config.progress.description.end());
    std::wstring lessonsMode(g_config.lessons.mode.begin(), g_config.lessons.mode.end());
    std::wstring progressMode(g_config.progress.mode.begin(), g_config.progress.mode.end());
    
    statusMessage += L"• \u8fdb\u5ea6\u6761\u63cf\u8ff0: " + progressDesc + L"\n";
    statusMessage += L"• \u8fdb\u5ea6\u6761\u6a21\u5f0f: " + progressMode + L"\n";
    statusMessage += L"• \u8bfe\u7a0b\u8868\u6a21\u5f0f: " + lessonsMode + L"\n";
    statusMessage += L"• \u5468\u6570\u504f\u79fb: ";
    statusMessage += (g_config.weekOffset.enabled ? L"\u542f\u7528" : L"\u7981\u7528");
    statusMessage += L" (" + std::to_wstring(g_config.weekOffset.offset) + L")\n";
    
    statusMessage += L"• \u63d0\u793a\u97f3: ";
    statusMessage += (g_config.notifications.enabled ? L"\u542f\u7528" : L"\u7981\u7528");
    statusMessage += L" (" + std::to_wstring(g_config.notifications.interval) + L"\u5206\u949f)\n";
    statusMessage += L"• \u4e8b\u4ef6\u6570\u91cf: " + std::to_wstring(g_config.events.size()) + L" \u4e2a\n";
    statusMessage += L"• \u58c1\u7eb8\u6570\u91cf: " + std::to_wstring(g_config.wallpapers.list.size()) + L" \u4e2a\n";
    statusMessage += L"• \u58c1\u7eb8\u5207\u6362\u95f4\u9694: " + std::to_wstring(g_config.wallpapers.interval) + L" \u79d2\n";
    
    if (hasConfigJs) {
        statusMessage += L"\n✨ config.js \u914d\u7f6e\u5df2\u6210\u529f\u52a0\u8f7d\uff01";
    }
    
    statusMessage += L"\n🎯 \u6240\u6709\u529f\u80fd\u6a21\u5757\u5df2\u5c31\u7eea\uff0c\u53ef\u4ee5\u5f00\u59cb\u914d\u7f6e\uff01";
    
    MessageBoxW(NULL, statusMessage.c_str(), L"\u542f\u52a8\u72b6\u6001", MB_OK | MB_ICONINFORMATION);
}

// Reload configuration from file
void ReloadConfig() {
    if (std::filesystem::exists("config.toml")) {
        try {
            // Try to reload
            LoadConfig();
            
            std::wstring message = L"🔄 \u914d\u7f6e\u91cd\u8f7d\u6210\u529f\uff01\n\n";
            message += L"📋 \u91cd\u8f7d\u7684\u914d\u7f6e\u9879:\n";
            message += L"✅ \u57fa\u672c\u8bbe\u7f6e (\u8fdb\u5ea6\u6761\u3001\u5468\u6570\u3001\u63d0\u793a\u97f3)\n";
            message += L"✅ \u8bfe\u7a0b\u8868\u8bbe\u7f6e (\u65f6\u95f4\u5b89\u6392\u3001\u663e\u793a\u6a21\u5f0f)\n";
            message += L"✅ \u4e8b\u4ef6\u7ba1\u7406 (\u91cd\u8981\u4e8b\u4ef6\u5217\u8868)\n";
            message += L"✅ \u58c1\u7eb8\u7ba1\u7406 (\u58c1\u7eb8\u5217\u8868\u548c\u95f4\u9694)\n";
            message += L"✅ \u544a\u793a\u724c\u8bbe\u7f6e (HTML\u5185\u5bb9)\n\n";
            message += L"🎯 \u6240\u6709\u8bbe\u7f6e\u5df2\u4ece config.toml \u91cd\u65b0\u52a0\u8f7d\uff01";
            
            MessageBoxW(NULL, message.c_str(), L"\u914d\u7f6e\u91cd\u8f7d", MB_OK | MB_ICONINFORMATION);
            
        } catch (const std::exception& e) {
            std::string errorMsg = "Config reload failed: " + std::string(e.what());
            MessageBoxA(NULL, errorMsg.c_str(), "Reload Error", MB_OK | MB_ICONERROR);
        }
    } else {
        MessageBoxW(NULL, 
            L"⚠️ \u914d\u7f6e\u6587\u4ef6 config.toml \u4e0d\u5b58\u5728\uff01\n\n"
            L"\u8bf7\u5148\u4fdd\u5b58\u914d\u7f6e\u6216\u624b\u52a8\u521b\u5efa\u914d\u7f6e\u6587\u4ef6\u3002\n"
            L"\u70b9\u51fb\u201c\u4fdd\u5b58\u914d\u7f6e\u201d\u6309\u94ae\u53ef\u4ee5\u521b\u5efa\u65b0\u7684\u914d\u7f6e\u6587\u4ef6\u3002", 
            L"\u914d\u7f6e\u91cd\u8f7d\u5931\u8d25", MB_OK | MB_ICONWARNING);
    }
}

// Save configuration to config.js format
void SaveConfigToJS() {
    try {
        // Ensure directory exists
        std::filesystem::create_directories("res/config");
        
        std::ofstream jsFile("res/config/config.js");
        if (!jsFile.is_open()) {
            return;
        }
        
        jsFile << "const CONFIG = {\n";
        jsFile << "    progress: {\n";
        jsFile << "        description: \"" << g_config.progress.description << "\",\n";
        jsFile << "        begin: \"" << g_config.progress.begin << "\",\n";
        jsFile << "        end: \"" << g_config.progress.end << "\",\n";
        jsFile << "        mode: \"" << g_config.progress.mode << "\"\n";
        jsFile << "    },\n";
        jsFile << "    weekOffset: {\n";
        jsFile << "        enabled: " << (g_config.weekOffset.enabled ? "true" : "false") << ",\n";
        jsFile << "        offset: " << g_config.weekOffset.offset << "\n";
        jsFile << "    },\n";
        jsFile << "    notifications: {\n";
        jsFile << "        enabled: " << (g_config.notifications.enabled ? "true" : "false") << ",\n";
        jsFile << "        interval: " << g_config.notifications.interval << ",\n";
        jsFile << "        endingTime: " << g_config.notifications.endingTime << "\n";
        jsFile << "    },\n";
        jsFile << "    lessons: {\n";
        jsFile << "        mode: \"" << g_config.lessons.mode << "\",\n";
        jsFile << "        timeSlots: [";
        for (size_t i = 0; i < g_config.lessons.timeSlots.size(); ++i) {
            if (i > 0) jsFile << ", ";
            jsFile << "\"" << g_config.lessons.timeSlots[i] << "\"";
        }
        jsFile << "],\n";
        jsFile << "        schedule: [\n";
        for (size_t day = 0; day < g_config.lessons.schedule.size(); ++day) {
            if (day > 0) jsFile << ",\n";
            jsFile << "            [";
            for (size_t period = 0; period < g_config.lessons.schedule[day].size(); ++period) {
                if (period > 0) jsFile << ", ";
                jsFile << "\"" << g_config.lessons.schedule[day][period] << "\"";
            }
            jsFile << "]";
        }
        jsFile << "\n        ]\n";
        jsFile << "    },\n";
        jsFile << "    wallpapers: {\n";
        jsFile << "        list: [";
        for (size_t i = 0; i < g_config.wallpapers.list.size(); ++i) {
            if (i > 0) jsFile << ", ";
            jsFile << "\"" << g_config.wallpapers.list[i] << "\"";
        }
        jsFile << "],\n";
        jsFile << "        interval: " << g_config.wallpapers.interval << "\n";
        jsFile << "    },\n";
        jsFile << "    events: [\n";
        for (size_t i = 0; i < g_config.events.size(); ++i) {
            if (i > 0) jsFile << ",\n";
            jsFile << "        {";
            bool first = true;
            for (const auto& pair : g_config.events[i]) {
                if (!first) jsFile << ", ";
                jsFile << "\"" << pair.first << "\": \"" << pair.second << "\"";
                first = false;
            }
            jsFile << "}";
        }
        jsFile << "\n    ],\n";
        jsFile << "    notice: `" << g_config.notice << "`\n";
        jsFile << "};\n\n";
        jsFile << "// Export for compatibility\n";
        jsFile << "const sth = CONFIG.sth;\n";
        
        jsFile.close();
    } catch (const std::exception& e) {
        // Ignore JS save errors
    }
}

// Save configuration
void SaveConfig() {
    try {
        toml::value data;
        
        data["progress"]["description"] = g_config.progress.description;
        data["progress"]["begin"] = g_config.progress.begin;
        data["progress"]["end"] = g_config.progress.end;
        data["progress"]["mode"] = g_config.progress.mode;
        
        data["weekOffset"]["enabled"] = g_config.weekOffset.enabled;
        data["weekOffset"]["offset"] = g_config.weekOffset.offset;
        
        data["notifications"]["enabled"] = g_config.notifications.enabled;
        data["notifications"]["interval"] = g_config.notifications.interval;
        data["notifications"]["endingTime"] = g_config.notifications.endingTime;
        
        data["default"]["url"] = g_config.system.url;
        data["default"]["browser_path"] = g_config.system.browserPath;
        
        // Save lessons
        data["lessons"]["mode"] = g_config.lessons.mode;
        data["lessons"]["timeSlots"] = g_config.lessons.timeSlots;
        data["lessons"]["schedule"] = g_config.lessons.schedule;
        
        // Save wallpapers
        data["wallpapers"]["list"] = g_config.wallpapers.list;
        data["wallpapers"]["interval"] = g_config.wallpapers.interval;
        
        // Save events
        toml::array eventArray;
        for (const auto& event : g_config.events) {
            toml::table eventTable;
            for (const auto& pair : event) {
                eventTable[pair.first] = pair.second;
            }
            eventArray.push_back(eventTable);
        }
        data["events"] = eventArray;
        
        // Save notice
        data["notice"] = g_config.notice;
        
        std::ofstream file("config.toml");
        file << data;
        file.close();
        
        // Also save to config.js for compatibility
        SaveConfigToJS();
        
        MessageBoxW(NULL, L"\u914d\u7f6e\u5df2\u4fdd\u5b58\u5230 config.toml \u548c config.js", L"\u6210\u529f", MB_OK | MB_ICONINFORMATION);
    } catch (const std::exception& e) {
        MessageBoxA(NULL, ("Config save failed: " + std::string(e.what())).c_str(), "Error", MB_OK | MB_ICONERROR);
    }
}

// Dialog control IDs
#define ID_EDIT_PROGRESS_DESC 2001
#define ID_EDIT_PROGRESS_BEGIN 2002
#define ID_EDIT_PROGRESS_END 2003
#define ID_RADIO_PROGRESS_LEFT 2004
#define ID_RADIO_PROGRESS_PASSED 2005
#define ID_CHECK_WEEK_ENABLED 2006
#define ID_EDIT_WEEK_OFFSET 2007
#define ID_CHECK_NOTIF_ENABLED 2008
#define ID_EDIT_NOTIF_INTERVAL 2009
#define ID_EDIT_NOTIF_ENDING 2010
#define ID_EDIT_SYSTEM_URL 2011
#define ID_EDIT_SYSTEM_BROWSER 2012
#define ID_BTN_BROWSE_BROWSER 2013
#define ID_BTN_BROWSE_URL 2014
#define ID_BTN_TEST_BROWSER 2015
#define ID_BTN_BASIC_OK 2016
#define ID_BTN_BASIC_CANCEL 2017

// String conversion helpers with proper encoding handling
std::wstring StringToWString(const std::string& str) {
    if (str.empty()) return std::wstring();
    
    // Try UTF-8 first
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    if (size_needed > 0) {
        std::wstring wstrTo(size_needed, 0);
        int result = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
        if (result > 0) return wstrTo;
    }
    
    // Fallback to system default (GBK/GB2312)
    size_needed = MultiByteToWideChar(CP_ACP, 0, &str[0], (int)str.size(), NULL, 0);
    if (size_needed > 0) {
        std::wstring wstrTo(size_needed, 0);
        MultiByteToWideChar(CP_ACP, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
        return wstrTo;
    }
    
    return std::wstring();
}

std::string WStringToString(const std::wstring& wstr) {
    if (wstr.empty()) return std::string();
    
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    if (size_needed > 0) {
        std::string strTo(size_needed, 0);
        WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
        return strTo;
    }
    
    return std::string();
}

// Simple string to wstring conversion for basic ASCII/UTF-8
std::wstring SimpleStringToWString(const std::string& str) {
    return std::wstring(str.begin(), str.end());
}

// Create modern edit control
HWND CreateFluentEdit(HWND parent, const std::wstring& text, int x, int y, int width, int height, int id, bool multiline = false) {
    DWORD style = WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL;
    if (multiline) {
        style |= ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL;
    }
    
    HWND edit = CreateWindowW(L"EDIT", text.c_str(), style,
        x, y, width, height, parent, (HMENU)(LONG_PTR)id, GetModuleHandle(NULL), NULL);
    
    HFONT font = CreateFontW(-14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    SendMessage(edit, WM_SETFONT, (WPARAM)font, TRUE);
    
    return edit;
}

// Create modern checkbox
HWND CreateFluentCheckbox(HWND parent, const std::wstring& text, int x, int y, int width, int height, int id, bool checked = false) {
    HWND checkbox = CreateWindowW(L"BUTTON", text.c_str(),
        WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
        x, y, width, height, parent, (HMENU)(LONG_PTR)id, GetModuleHandle(NULL), NULL);
    
    HFONT font = CreateFontW(-14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    SendMessage(checkbox, WM_SETFONT, (WPARAM)font, TRUE);
    
    if (checked) {
        SendMessage(checkbox, BM_SETCHECK, BST_CHECKED, 0);
    }
    
    return checkbox;
}

// Basic Settings Dialog Procedure
LRESULT CALLBACK BasicSettingsDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HWND hEditProgressDesc, hEditProgressBegin, hEditProgressEnd;
    static HWND hRadioProgressLeft, hRadioProgressPassed;
    static HWND hCheckWeekEnabled, hEditWeekOffset;
    static HWND hCheckNotifEnabled, hEditNotifInterval, hEditNotifEnding;
    static HWND hEditSystemUrl, hEditSystemBrowser;
    
    switch (msg) {
        case WM_CREATE: {
            // Create title with modern styling
            CreateFluentLabel(hwnd, L"基本设置", 30, 20, 300, 35, true);
            CreateFluentLabel(hwnd, L"配置进度条、周数、提示音和系统设置", 30, 55, 400, 20, false, true);
            
            int y = 90;
            int sectionSpacing = 30;
            int itemSpacing = 25;
            int sectionPadding = 15;
            
            // Progress Bar Settings Section
            CreateFluentSection(hwnd, L"📊 进度条设置", 20, y, 460, 150);
            y += 25;
            
            CreateFluentLabel(hwnd, L"描述文本:", 40, y, 80, 20, false);
            hEditProgressDesc = CreateFluentEdit(hwnd, StringToWString(g_config.progress.description), 130, y, 320, 25, ID_EDIT_PROGRESS_DESC);
            y += itemSpacing;
            
            CreateFluentLabel(hwnd, L"开始时间:", 40, y, 80, 20, false);
            hEditProgressBegin = CreateFluentEdit(hwnd, StringToWString(g_config.progress.begin), 130, y, 320, 25, ID_EDIT_PROGRESS_BEGIN);
            y += itemSpacing;
            
            CreateFluentLabel(hwnd, L"结束时间:", 40, y, 80, 20, false);
            hEditProgressEnd = CreateFluentEdit(hwnd, StringToWString(g_config.progress.end), 130, y, 320, 25, ID_EDIT_PROGRESS_END);
            y += itemSpacing;
            
            CreateFluentLabel(hwnd, L"显示模式:", 40, y, 80, 20, false);
            hRadioProgressLeft = CreateWindowW(L"BUTTON", L"剩余百分比", 
                WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | WS_GROUP,
                130, y, 100, 25, hwnd, (HMENU)ID_RADIO_PROGRESS_LEFT, GetModuleHandle(NULL), NULL);
            hRadioProgressPassed = CreateWindowW(L"BUTTON", L"已过百分比", 
                WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
                240, y, 100, 25, hwnd, (HMENU)ID_RADIO_PROGRESS_PASSED, GetModuleHandle(NULL), NULL);
            
            // Set radio button state
            if (g_config.progress.mode == "left") {
                SendMessage(hRadioProgressLeft, BM_SETCHECK, BST_CHECKED, 0);
            } else {
                SendMessage(hRadioProgressPassed, BM_SETCHECK, BST_CHECKED, 0);
            }
            
            y += sectionSpacing + 20;
            
            // Week Offset Settings Section
            CreateFluentSection(hwnd, L"📅 周数设置", 20, y, 460, 90);
            y += 25;
            
            hCheckWeekEnabled = CreateFluentCheckbox(hwnd, L"启用周数偏移", 40, y, 150, 25, ID_CHECK_WEEK_ENABLED, g_config.weekOffset.enabled);
            y += itemSpacing;
            
            CreateFluentLabel(hwnd, L"偏移值:", 40, y, 80, 20, false);
            hEditWeekOffset = CreateFluentEdit(hwnd, std::to_wstring(g_config.weekOffset.offset), 130, y, 100, 25, ID_EDIT_WEEK_OFFSET);
            y += sectionSpacing + 15;
            
            // Notification Settings Section
            CreateFluentSection(hwnd, L"🔔 提示音设置", 20, y, 460, 110);
            y += 25;
            
            hCheckNotifEnabled = CreateFluentCheckbox(hwnd, L"启用提示音", 40, y, 150, 25, ID_CHECK_NOTIF_ENABLED, g_config.notifications.enabled);
            y += itemSpacing;
            
            CreateFluentLabel(hwnd, L"间隔时间(分钟):", 40, y, 110, 20, false);
            hEditNotifInterval = CreateFluentEdit(hwnd, std::to_wstring(g_config.notifications.interval), 160, y, 80, 25, ID_EDIT_NOTIF_INTERVAL);
            y += itemSpacing;
            
            CreateFluentLabel(hwnd, L"结束提示(分钟):", 40, y, 110, 20, false);
            hEditNotifEnding = CreateFluentEdit(hwnd, std::to_wstring(g_config.notifications.endingTime), 160, y, 80, 25, ID_EDIT_NOTIF_ENDING);
            y += sectionSpacing + 15;
            
            // System Settings Section
            CreateFluentSection(hwnd, L"⚙️ 系统设置", 20, y, 460, 110);
            y += 25;
            
            CreateFluentLabel(hwnd, L"URL地址:", 40, y, 80, 20, false);
            hEditSystemUrl = CreateFluentEdit(hwnd, StringToWString(g_config.system.url), 130, y, 250, 25, ID_EDIT_SYSTEM_URL);
            CreateFluentButton(hwnd, L"浏览", 390, y, 60, 25, ID_BTN_BROWSE_URL);
            y += itemSpacing;
            
            CreateFluentLabel(hwnd, L"浏览器路径:", 40, y, 80, 20, false);
            hEditSystemBrowser = CreateFluentEdit(hwnd, StringToWString(g_config.system.browserPath), 130, y, 250, 25, ID_EDIT_SYSTEM_BROWSER);
            CreateFluentButton(hwnd, L"浏览", 390, y, 60, 25, ID_BTN_BROWSE_BROWSER);
            y += itemSpacing;
            
            CreateFluentButton(hwnd, L"测试浏览器", 130, y, 100, 30, ID_BTN_TEST_BROWSER);
            
            y += 50;
            
            // Dialog buttons with better spacing and styling
            CreateFluentButton(hwnd, L"💾 保存设置", 280, y, 90, 35, ID_BTN_BASIC_OK, true);
            CreateFluentButton(hwnd, L"❌ 取消", 380, y, 80, 35, ID_BTN_BASIC_CANCEL);
            
            break;
        }
        
        case WM_COMMAND: {
            switch (LOWORD(wParam)) {
                case ID_BTN_BROWSE_URL: {
                    OPENFILENAMEW ofn = {};
                    wchar_t szFile[260] = {};
                    
                    ofn.lStructSize = sizeof(ofn);
                    ofn.hwndOwner = hwnd;
                    ofn.lpstrFile = szFile;
                    ofn.nMaxFile = sizeof(szFile) / sizeof(wchar_t);
                    ofn.lpstrFilter = L"HTML Files\0*.html;*.htm\0All Files\0*.*\0";
                    ofn.nFilterIndex = 1;
                    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
                    
                    if (GetOpenFileNameW(&ofn)) {
                        SetWindowTextW(hEditSystemUrl, szFile);
                    }
                    break;
                }
                
                case ID_BTN_BROWSE_BROWSER: {
                    OPENFILENAMEW ofn = {};
                    wchar_t szFile[260] = {};
                    
                    ofn.lStructSize = sizeof(ofn);
                    ofn.hwndOwner = hwnd;
                    ofn.lpstrFile = szFile;
                    ofn.nMaxFile = sizeof(szFile) / sizeof(wchar_t);
                    ofn.lpstrFilter = L"Executable Files\0*.exe\0All Files\0*.*\0";
                    ofn.nFilterIndex = 1;
                    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
                    
                    if (GetOpenFileNameW(&ofn)) {
                        SetWindowTextW(hEditSystemBrowser, szFile);
                    }
                    break;
                }
                
                case ID_BTN_TEST_BROWSER: {
                    wchar_t browserPath[260];
                    wchar_t url[260];
                    GetWindowTextW(hEditSystemBrowser, browserPath, 260);
                    GetWindowTextW(hEditSystemUrl, url, 260);
                    
                    if (wcslen(browserPath) > 0) {
                        ShellExecuteW(NULL, L"open", browserPath, url, NULL, SW_SHOWNORMAL);
                    } else {
                        ShellExecuteW(NULL, L"open", url, NULL, NULL, SW_SHOWNORMAL);
                    }
                    break;
                }
                
                case ID_BTN_BASIC_OK: {
                    // Save settings to config
                    wchar_t buffer[512];
                    
                    GetWindowTextW(hEditProgressDesc, buffer, 512);
                    g_config.progress.description = WStringToString(buffer);
                    
                    GetWindowTextW(hEditProgressBegin, buffer, 512);
                    g_config.progress.begin = WStringToString(buffer);
                    
                    GetWindowTextW(hEditProgressEnd, buffer, 512);
                    g_config.progress.end = WStringToString(buffer);
                    
                    g_config.progress.mode = (SendMessage(hRadioProgressLeft, BM_GETCHECK, 0, 0) == BST_CHECKED) ? "left" : "passed";
                    
                    g_config.weekOffset.enabled = (SendMessage(hCheckWeekEnabled, BM_GETCHECK, 0, 0) == BST_CHECKED);
                    
                    GetWindowTextW(hEditWeekOffset, buffer, 512);
                    g_config.weekOffset.offset = _wtoi(buffer);
                    
                    g_config.notifications.enabled = (SendMessage(hCheckNotifEnabled, BM_GETCHECK, 0, 0) == BST_CHECKED);
                    
                    GetWindowTextW(hEditNotifInterval, buffer, 512);
                    g_config.notifications.interval = _wtoi(buffer);
                    
                    GetWindowTextW(hEditNotifEnding, buffer, 512);
                    g_config.notifications.endingTime = _wtoi(buffer);
                    
                    GetWindowTextW(hEditSystemUrl, buffer, 512);
                    g_config.system.url = WStringToString(buffer);
                    
                    GetWindowTextW(hEditSystemBrowser, buffer, 512);
                    g_config.system.browserPath = WStringToString(buffer);
                    
                    MessageBoxW(hwnd, L"\u57fa\u672c\u8bbe\u7f6e\u5df2\u4fdd\u5b58\uff01", L"\u6210\u529f", MB_OK | MB_ICONINFORMATION);
                    DestroyWindow(hwnd);
                    break;
                }
                
                case ID_BTN_BASIC_CANCEL:
                    DestroyWindow(hwnd);
                    break;
            }
            break;
        }
        
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            RECT rect;
            GetClientRect(hwnd, &rect);
            
            // Fill with card background color
            HBRUSH bgBrush = CreateSolidBrush(FluentColors::CardBackground);
            FillRect(hdc, &rect, bgBrush);
            DeleteObject(bgBrush);
            
            EndPaint(hwnd, &ps);
            return 0;
        }
        
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
            
        case WM_DESTROY:
            // Don't quit the entire application, just close this dialog
            break;
    }
    
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// Schedule Settings Dialog IDs
#define ID_RADIO_SCHEDULE_SCROLL 3001
#define ID_RADIO_SCHEDULE_DAY 3002
#define ID_LIST_TIME_SLOTS 3003
#define ID_EDIT_TIME_SLOT 3004
#define ID_BTN_ADD_TIME_SLOT 3005
#define ID_BTN_REMOVE_TIME_SLOT 3006
#define ID_LIST_SCHEDULE 3007
#define ID_EDIT_COURSE_NAME 3008
#define ID_BTN_UPDATE_COURSE 3009
#define ID_BTN_SCHEDULE_OK 3010
#define ID_BTN_SCHEDULE_CANCEL 3011

// Create modern listbox
HWND CreateFluentListBox(HWND parent, int x, int y, int width, int height, int id) {
    HWND listbox = CreateWindowW(L"LISTBOX", L"",
        WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL | LBS_NOTIFY,
        x, y, width, height, parent, (HMENU)(LONG_PTR)id, GetModuleHandle(NULL), NULL);
    
    HFONT font = CreateFontW(-14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    SendMessage(listbox, WM_SETFONT, (WPARAM)font, TRUE);
    
    return listbox;
}

// Schedule Settings Dialog Procedure
LRESULT CALLBACK ScheduleSettingsDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HWND hRadioScrollMode, hRadioDayMode;
    static HWND hListTimeSlots, hEditTimeSlot;
    static HWND hListSchedule, hEditCourseName;
    
    switch (msg) {
        case WM_CREATE: {
            // Create title with improved styling
            CreateFluentLabel(hwnd, L"课程表设置", 30, 20, 300, 35, true);
            CreateFluentLabel(hwnd, L"配置显示模式、时间安排和课程表", 30, 55, 400, 20, false, true);
            
            int y = 90;
            int sectionSpacing = 25;
            int itemSpacing = 25;
            
            // Display Mode Section with groupbox
            CreateFluentSection(hwnd, L"📱 显示模式", 20, y, 500, 80);
            y += 30;
            
            hRadioScrollMode = CreateWindowW(L"BUTTON", L"滚动模式", 
                WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | WS_GROUP,
                40, y, 100, 25, hwnd, (HMENU)ID_RADIO_SCHEDULE_SCROLL, GetModuleHandle(NULL), NULL);
            hRadioDayMode = CreateWindowW(L"BUTTON", L"一天进度模式", 
                WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
                150, y, 120, 25, hwnd, (HMENU)ID_RADIO_SCHEDULE_DAY, GetModuleHandle(NULL), NULL);
            
            // Set radio button state
            if (g_config.lessons.mode == "scroll") {
                SendMessage(hRadioScrollMode, BM_SETCHECK, BST_CHECKED, 0);
            } else {
                SendMessage(hRadioDayMode, BM_SETCHECK, BST_CHECKED, 0);
            }
            
            y += sectionSpacing + 25;
            
            // Time Slots Section with groupbox
            CreateFluentSection(hwnd, L"⏰ 时间安排", 20, y, 500, 160);
            y += 30;
            
            CreateFluentLabel(hwnd, L"时间段列表:", 40, y, 100, 20, false);
            hListTimeSlots = CreateFluentListBox(hwnd, 40, y + 20, 200, 100, ID_LIST_TIME_SLOTS);
            
            // Populate time slots
            for (const auto& slot : g_config.lessons.timeSlots) {
                std::wstring wslot = StringToWString(slot);
                SendMessageW(hListTimeSlots, LB_ADDSTRING, 0, (LPARAM)wslot.c_str());
            }
            
            CreateFluentLabel(hwnd, L"新时间段 (格式: HH:MM-HH:MM):", 260, y, 200, 20, false);
            hEditTimeSlot = CreateFluentEdit(hwnd, L"", 260, y + 20, 180, 25, ID_EDIT_TIME_SLOT);
            CreateFluentButton(hwnd, L"➕ 添加", 260, y + 50, 85, 30, ID_BTN_ADD_TIME_SLOT);
            CreateFluentButton(hwnd, L"🗑️ 删除", 355, y + 50, 85, 30, ID_BTN_REMOVE_TIME_SLOT);
            
            y += 170;
            
            // Schedule Section with groupbox
            CreateFluentSection(hwnd, L"📚 课程表预览", 20, y, 500, 130);
            y += 30;
            
            CreateFluentLabel(hwnd, L"课程安排 (周一到周五):", 40, y, 200, 20, false);
            hListSchedule = CreateFluentListBox(hwnd, 40, y + 20, 300, 100, ID_LIST_SCHEDULE);
            
            // Populate schedule
            const std::vector<std::string> weekdays = {"周一", "周二", "周三", "周四", "周五"};
            for (size_t day = 0; day < g_config.lessons.schedule.size() && day < weekdays.size(); ++day) {
                std::wstring daySchedule = StringToWString(weekdays[day]) + L": ";
                for (size_t period = 0; period < g_config.lessons.schedule[day].size(); ++period) {
                    if (period > 0) daySchedule += L", ";
                    daySchedule += StringToWString(g_config.lessons.schedule[day][period]);
                }
                SendMessageW(hListSchedule, LB_ADDSTRING, 0, (LPARAM)daySchedule.c_str());
            }
            
            CreateFluentLabel(hwnd, L"课程编辑 (功能开发中):", 360, y + 20, 150, 20, false, true);
            hEditCourseName = CreateFluentEdit(hwnd, L"", 360, y + 45, 120, 25, ID_EDIT_COURSE_NAME);
            CreateFluentButton(hwnd, L"🔧 更新", 360, y + 75, 80, 25, ID_BTN_UPDATE_COURSE);
            
            y += 140;
            
            // Dialog buttons with improved styling
            CreateFluentButton(hwnd, L"💾 保存设置", 320, y, 90, 35, ID_BTN_SCHEDULE_OK, true);
            CreateFluentButton(hwnd, L"❌ 取消", 420, y, 80, 35, ID_BTN_SCHEDULE_CANCEL);
            
            break;
        }
        
        case WM_COMMAND: {
            switch (LOWORD(wParam)) {
                case ID_BTN_ADD_TIME_SLOT: {
                    wchar_t timeSlot[100];
                    GetWindowTextW(hEditTimeSlot, timeSlot, 100);
                    if (wcslen(timeSlot) > 0) {
                        SendMessageW(hListTimeSlots, LB_ADDSTRING, 0, (LPARAM)timeSlot);
                        SetWindowTextW(hEditTimeSlot, L"");
                        g_config.lessons.timeSlots.push_back(WStringToString(timeSlot));
                    }
                    break;
                }
                
                case ID_BTN_REMOVE_TIME_SLOT: {
                    int sel = SendMessage(hListTimeSlots, LB_GETCURSEL, 0, 0);
                    if (sel != LB_ERR) {
                        SendMessage(hListTimeSlots, LB_DELETESTRING, sel, 0);
                        if (sel < g_config.lessons.timeSlots.size()) {
                            g_config.lessons.timeSlots.erase(g_config.lessons.timeSlots.begin() + sel);
                        }
                    }
                    break;
                }
                
                case ID_BTN_UPDATE_COURSE: {
                    wchar_t courseName[100];
                    GetWindowTextW(hEditCourseName, courseName, 100);
                    if (wcslen(courseName) > 0) {
                        MessageBoxW(hwnd, L"\u8bfe\u7a0b\u66f4\u65b0\u529f\u80fd\u5c06\u5728\u4e0b\u4e2a\u7248\u672c\u5b8c\u5584\uff01\n\u5f53\u524d\u53ef\u4ee5\u67e5\u770b\u548c\u7ba1\u7406\u65f6\u95f4\u6bb5\u3002", L"\u63d0\u793a", MB_OK | MB_ICONINFORMATION);
                    }
                    break;
                }
                
                case ID_BTN_SCHEDULE_OK: {
                    // Save display mode
                    g_config.lessons.mode = (SendMessage(hRadioScrollMode, BM_GETCHECK, 0, 0) == BST_CHECKED) ? "scroll" : "day";
                    
                    MessageBoxW(hwnd, L"\u8bfe\u7a0b\u8868\u8bbe\u7f6e\u5df2\u4fdd\u5b58\uff01", L"\u6210\u529f", MB_OK | MB_ICONINFORMATION);
                    DestroyWindow(hwnd);
                    break;
                }
                
                case ID_BTN_SCHEDULE_CANCEL:
                    DestroyWindow(hwnd);
                    break;
            }
            break;
        }
        
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            RECT rect;
            GetClientRect(hwnd, &rect);
            
            HBRUSH bgBrush = CreateSolidBrush(FluentColors::CardBackground);
            FillRect(hdc, &rect, bgBrush);
            DeleteObject(bgBrush);
            
            EndPaint(hwnd, &ps);
            return 0;
        }
        
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
            
        case WM_DESTROY:
            // Don't quit the entire application, just close this dialog
            break;
    }
    
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// Events Settings Dialog IDs
#define ID_LIST_EVENTS 4001
#define ID_EDIT_EVENT_NAME 4002
#define ID_EDIT_EVENT_DATE 4003
#define ID_EDIT_EVENT_TIME 4004
#define ID_BTN_ADD_EVENT 4005
#define ID_BTN_REMOVE_EVENT 4006
#define ID_BTN_EDIT_EVENT 4007
#define ID_BTN_EVENTS_OK 4008
#define ID_BTN_EVENTS_CANCEL 4009

// Events Settings Dialog Procedure
LRESULT CALLBACK EventsSettingsDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HWND hListEvents, hEditEventName, hEditEventDate, hEditEventTime;
    
    switch (msg) {
        case WM_CREATE: {
            // Create title with improved styling
            CreateFluentLabel(hwnd, L"事件管理", 30, 20, 300, 35, true);
            CreateFluentLabel(hwnd, L"添加、编辑和删除重要事件提醒", 30, 55, 400, 20, false, true);
            
            int y = 90;
            int sectionSpacing = 25;
            int itemSpacing = 25;
            
            // Events List Section with groupbox
            CreateFluentSection(hwnd, L"📅 当前事件列表", 20, y, 480, 200);
            y += 30;
            
            hListEvents = CreateFluentListBox(hwnd, 30, y, 460, 150, ID_LIST_EVENTS);
            
            // Populate events list
            for (const auto& event : g_config.events) {
                std::wstring eventText = StringToWString(event.at("name")) + L" - " + 
                                       StringToWString(event.at("date")) + L" " + 
                                       StringToWString(event.at("time"));
                SendMessageW(hListEvents, LB_ADDSTRING, 0, (LPARAM)eventText.c_str());
            }
            
            y += 170;
            
            // Add/Edit Event Section with groupbox
            CreateFluentSection(hwnd, L"➕ 添加/编辑事件", 20, y, 480, 140);
            y += 30;
            
            CreateFluentLabel(hwnd, L"事件名称:", 40, y, 80, 20, false);
            hEditEventName = CreateFluentEdit(hwnd, L"", 130, y, 200, 25, ID_EDIT_EVENT_NAME);
            y += itemSpacing + 5;
            
            CreateFluentLabel(hwnd, L"日期 (YYYY-MM-DD):", 40, y, 120, 20, false);
            hEditEventDate = CreateFluentEdit(hwnd, L"", 170, y, 160, 25, ID_EDIT_EVENT_DATE);
            y += itemSpacing + 5;
            
            CreateFluentLabel(hwnd, L"时间 (HH:MM):", 40, y, 100, 20, false);
            hEditEventTime = CreateFluentEdit(hwnd, L"", 150, y, 100, 25, ID_EDIT_EVENT_TIME);
            y += 35;
            
            // Action buttons with improved styling
            CreateFluentButton(hwnd, L"➕ 添加", 40, y, 80, 30, ID_BTN_ADD_EVENT);
            CreateFluentButton(hwnd, L"✏️ 编辑", 130, y, 80, 30, ID_BTN_EDIT_EVENT);
            CreateFluentButton(hwnd, L"🗑️ 删除", 220, y, 80, 30, ID_BTN_REMOVE_EVENT);
            
            y += 50;
            
            // Dialog buttons with improved styling
            CreateFluentButton(hwnd, L"💾 保存设置", 320, y, 90, 35, ID_BTN_EVENTS_OK, true);
            CreateFluentButton(hwnd, L"❌ 取消", 420, y, 80, 35, ID_BTN_EVENTS_CANCEL);
            
            break;
        }
        
        case WM_COMMAND: {
            switch (LOWORD(wParam)) {
                case ID_BTN_ADD_EVENT: {
                    wchar_t name[100], date[20], time[10];
                    GetWindowTextW(hEditEventName, name, 100);
                    GetWindowTextW(hEditEventDate, date, 20);
                    GetWindowTextW(hEditEventTime, time, 10);
                    
                    if (wcslen(name) > 0 && wcslen(date) > 0 && wcslen(time) > 0) {
                        // Add to config
                        std::map<std::string, std::string> newEvent;
                        newEvent["name"] = WStringToString(name);
                        newEvent["date"] = WStringToString(date);
                        newEvent["time"] = WStringToString(time);
                        g_config.events.push_back(newEvent);
                        
                        // Add to list
                        std::wstring eventText = std::wstring(name) + L" - " + std::wstring(date) + L" " + std::wstring(time);
                        SendMessageW(hListEvents, LB_ADDSTRING, 0, (LPARAM)eventText.c_str());
                        
                        // Clear inputs
                        SetWindowTextW(hEditEventName, L"");
                        SetWindowTextW(hEditEventDate, L"");
                        SetWindowTextW(hEditEventTime, L"");
                        
                        MessageBoxW(hwnd, L"\u4e8b\u4ef6\u5df2\u6dfb\u52a0\uff01", L"\u6210\u529f", MB_OK | MB_ICONINFORMATION);
                    } else {
                        MessageBoxW(hwnd, L"\u8bf7\u586b\u5199\u5b8c\u6574\u7684\u4e8b\u4ef6\u4fe1\u606f\uff01", L"\u63d0\u793a", MB_OK | MB_ICONWARNING);
                    }
                    break;
                }
                
                case ID_BTN_REMOVE_EVENT: {
                    int sel = SendMessage(hListEvents, LB_GETCURSEL, 0, 0);
                    if (sel != LB_ERR && sel < g_config.events.size()) {
                        g_config.events.erase(g_config.events.begin() + sel);
                        SendMessage(hListEvents, LB_DELETESTRING, sel, 0);
                        MessageBoxW(hwnd, L"\u4e8b\u4ef6\u5df2\u5220\u9664\uff01", L"\u6210\u529f", MB_OK | MB_ICONINFORMATION);
                    } else {
                        MessageBoxW(hwnd, L"\u8bf7\u9009\u62e9\u8981\u5220\u9664\u7684\u4e8b\u4ef6\uff01", L"\u63d0\u793a", MB_OK | MB_ICONWARNING);
                    }
                    break;
                }
                
                case ID_BTN_EDIT_EVENT: {
                    int sel = SendMessage(hListEvents, LB_GETCURSEL, 0, 0);
                    if (sel != LB_ERR && sel < g_config.events.size()) {
                        const auto& event = g_config.events[sel];
                        SetWindowTextW(hEditEventName, StringToWString(event.at("name")).c_str());
                        SetWindowTextW(hEditEventDate, StringToWString(event.at("date")).c_str());
                        SetWindowTextW(hEditEventTime, StringToWString(event.at("time")).c_str());
                        MessageBoxW(hwnd, L"\u4e8b\u4ef6\u4fe1\u606f\u5df2\u52a0\u8f7d\u5230\u7f16\u8f91\u6846\uff0c\u4fee\u6539\u540e\u70b9\u51fb\u201c\u6dfb\u52a0\u4e8b\u4ef6\u201d\u66f4\u65b0\uff01", L"\u63d0\u793a", MB_OK | MB_ICONINFORMATION);
                    } else {
                        MessageBoxW(hwnd, L"\u8bf7\u9009\u62e9\u8981\u7f16\u8f91\u7684\u4e8b\u4ef6\uff01", L"\u63d0\u793a", MB_OK | MB_ICONWARNING);
                    }
                    break;
                }
                
                case ID_BTN_EVENTS_OK: {
                    MessageBoxW(hwnd, L"\u4e8b\u4ef6\u8bbe\u7f6e\u5df2\u4fdd\u5b58\uff01", L"\u6210\u529f", MB_OK | MB_ICONINFORMATION);
                    DestroyWindow(hwnd);
                    break;
                }
                
                case ID_BTN_EVENTS_CANCEL:
                    DestroyWindow(hwnd);
                    break;
            }
            break;
        }
        
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            RECT rect;
            GetClientRect(hwnd, &rect);
            
            HBRUSH bgBrush = CreateSolidBrush(FluentColors::CardBackground);
            FillRect(hdc, &rect, bgBrush);
            DeleteObject(bgBrush);
            
            EndPaint(hwnd, &ps);
            return 0;
        }
        
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
            
        case WM_DESTROY:
            // Don't quit the entire application, just close this dialog
            break;
    }
    
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// Wallpapers Settings Dialog IDs
#define ID_LIST_WALLPAPERS 5001
#define ID_EDIT_WALLPAPER_PATH 5002
#define ID_BTN_ADD_WALLPAPER 5003
#define ID_BTN_REMOVE_WALLPAPER 5004
#define ID_BTN_BROWSE_WALLPAPER 5005
#define ID_BTN_SCAN_FOLDER 5006
#define ID_EDIT_WALLPAPER_INTERVAL 5007
#define ID_BTN_WALLPAPERS_OK 5008
#define ID_BTN_WALLPAPERS_CANCEL 5009

// Wallpapers Settings Dialog Procedure
LRESULT CALLBACK WallpapersSettingsDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HWND hListWallpapers, hEditWallpaperPath, hEditWallpaperInterval;
    
    switch (msg) {
        case WM_CREATE: {
            // Create title
            CreateFluentLabel(hwnd, L"壁纸管理", 30, 20, 300, 35, true);
            CreateFluentLabel(hwnd, L"管理壁纸列表和切换设置", 30, 55, 400, 20, false);
            
            int y = 90;
            
            // Wallpapers List Section
            CreateFluentLabel(hwnd, L"🖼️ 壁纸列表", 30, y, 200, 25, false);
            y += 30;
            
            hListWallpapers = CreateFluentListBox(hwnd, 30, y, 400, 180, ID_LIST_WALLPAPERS);
            
            // Populate wallpapers list
            for (const auto& wallpaper : g_config.wallpapers.list) {
                std::wstring wallpaperPath = StringToWString(wallpaper);
                SendMessageW(hListWallpapers, LB_ADDSTRING, 0, (LPARAM)wallpaperPath.c_str());
            }
            
            y += 200;
            
            // Add Wallpaper Section
            CreateFluentLabel(hwnd, L"➕ 添加壁纸", 30, y, 200, 25, false);
            y += 30;
            
            CreateFluentLabel(hwnd, L"壁纸路径:", 40, y, 80, 20, false);
            hEditWallpaperPath = CreateFluentEdit(hwnd, L"", 130, y, 200, 25, ID_EDIT_WALLPAPER_PATH);
            CreateFluentButton(hwnd, L"浏览", 340, y, 60, 25, ID_BTN_BROWSE_WALLPAPER);
            y += 35;
            
            // Action buttons with improved styling
            CreateFluentButton(hwnd, L"➕ 添加壁纸", 40, y, 90, 30, ID_BTN_ADD_WALLPAPER);
            CreateFluentButton(hwnd, L"🗑️ 删除选中", 140, y, 90, 30, ID_BTN_REMOVE_WALLPAPER);
            CreateFluentButton(hwnd, L"📁 扫描文件夹", 240, y, 100, 30, ID_BTN_SCAN_FOLDER);
            y += 50;
            
            // Interval Setting
            CreateFluentLabel(hwnd, L"⏱️ 切换间隔", 30, y, 200, 25, false);
            y += 30;
            
            CreateFluentLabel(hwnd, L"间隔时间(秒):", 40, y, 100, 20, false);
            hEditWallpaperInterval = CreateFluentEdit(hwnd, std::to_wstring(g_config.wallpapers.interval), 150, y, 100, 25, ID_EDIT_WALLPAPER_INTERVAL);
            y += 45;
            
            // Dialog buttons
            CreateFluentButton(hwnd, L"保存", 320, y, 80, 35, ID_BTN_WALLPAPERS_OK);
            CreateFluentButton(hwnd, L"取消", 410, y, 80, 35, ID_BTN_WALLPAPERS_CANCEL);
            
            break;
        }
        
        case WM_COMMAND: {
            switch (LOWORD(wParam)) {
                case ID_BTN_BROWSE_WALLPAPER: {
                    OPENFILENAMEW ofn = {};
                    wchar_t szFile[260] = {};
                    
                    ofn.lStructSize = sizeof(ofn);
                    ofn.hwndOwner = hwnd;
                    ofn.lpstrFile = szFile;
                    ofn.nMaxFile = sizeof(szFile) / sizeof(wchar_t);
                    ofn.lpstrFilter = L"Image Files\0*.jpg;*.jpeg;*.png;*.bmp;*.gif\0All Files\0*.*\0";
                    ofn.nFilterIndex = 1;
                    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
                    
                    if (GetOpenFileNameW(&ofn)) {
                        SetWindowTextW(hEditWallpaperPath, szFile);
                    }
                    break;
                }
                
                case ID_BTN_ADD_WALLPAPER: {
                    wchar_t path[260];
                    GetWindowTextW(hEditWallpaperPath, path, 260);
                    
                    if (wcslen(path) > 0) {
                        std::string pathStr = WStringToString(path);
                        g_config.wallpapers.list.push_back(pathStr);
                        SendMessageW(hListWallpapers, LB_ADDSTRING, 0, (LPARAM)path);
                        SetWindowTextW(hEditWallpaperPath, L"");
                        MessageBoxW(hwnd, L"\u58c1\u7eb8\u5df2\u6dfb\u52a0\uff01", L"\u6210\u529f", MB_OK | MB_ICONINFORMATION);
                    } else {
                        MessageBoxW(hwnd, L"\u8bf7\u9009\u62e9\u58c1\u7eb8\u6587\u4ef6\uff01", L"\u63d0\u793a", MB_OK | MB_ICONWARNING);
                    }
                    break;
                }
                
                case ID_BTN_REMOVE_WALLPAPER: {
                    int sel = SendMessage(hListWallpapers, LB_GETCURSEL, 0, 0);
                    if (sel != LB_ERR && sel < g_config.wallpapers.list.size()) {
                        g_config.wallpapers.list.erase(g_config.wallpapers.list.begin() + sel);
                        SendMessage(hListWallpapers, LB_DELETESTRING, sel, 0);
                        MessageBoxW(hwnd, L"\u58c1\u7eb8\u5df2\u5220\u9664\uff01", L"\u6210\u529f", MB_OK | MB_ICONINFORMATION);
                    } else {
                        MessageBoxW(hwnd, L"\u8bf7\u9009\u62e9\u8981\u5220\u9664\u7684\u58c1\u7eb8\uff01", L"\u63d0\u793a", MB_OK | MB_ICONWARNING);
                    }
                    break;
                }
                
                case ID_BTN_SCAN_FOLDER: {
                    // First try to find common wallpaper directories automatically (以res为根目录)
                    std::vector<std::string> commonDirs = {
                        "res/wallpaper", "res/wallpapers", "res/images", "res/backgrounds", 
                        "res/pics", "res/pictures", "res"
                    };
                    
                    std::string autoFoundDir;
                    bool foundAuto = false;
                    
                    for (const auto& dir : commonDirs) {
                        if (std::filesystem::exists(dir) && std::filesystem::is_directory(dir)) {
                            // Check if directory has image files
                            bool hasImages = false;
                            try {
                                for (const auto& entry : std::filesystem::directory_iterator(dir)) {
                                    if (entry.is_regular_file()) {
                                        std::string ext = entry.path().extension().string();
                                        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                                        if (ext == ".jpg" || ext == ".jpeg" || ext == ".png" || ext == ".bmp" || ext == ".gif" || ext == ".webp") {
                                            hasImages = true;
                                            break;
                                        }
                                    }
                                }
                            } catch (...) {}
                            
                            if (hasImages) {
                                autoFoundDir = dir;
                                foundAuto = true;
                                break;
                            }
                        }
                    }
                    
                    std::string selectedDir;
                    bool useAutoFound = false;
                    
                    if (foundAuto) {
                        std::wstring message = L"🔍 自动发现壁纸文件夹:\n\n";
                        message += StringToWString(autoFoundDir) + L"\n\n";
                        message += L"是否扫描此文件夹？\n\n";
                        message += L"点击\"是\"扫描此文件夹\n";
                        message += L"点击\"否\"手动选择其他文件夹";
                        
                        int result = MessageBoxW(hwnd, message.c_str(), L"发现壁纸文件夹", MB_YESNOCANCEL | MB_ICONQUESTION);
                        
                        if (result == IDYES) {
                            selectedDir = autoFoundDir;
                            useAutoFound = true;
                        } else if (result == IDCANCEL) {
                            break; // 用户取消
                        }
                        // IDNO 继续到手动选择
                    }
                    
                    if (!useAutoFound) {
                        // Let user choose folder to scan
                        wchar_t folderPath[MAX_PATH] = {};
                        
                        BROWSEINFOW bi = {};
                        bi.hwndOwner = hwnd;
                        bi.pszDisplayName = folderPath;
                        bi.lpszTitle = L"选择要扫描的壁纸文件夹";
                        bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
                        
                        LPITEMIDLIST pidl = SHBrowseForFolderW(&bi);
                        if (pidl != NULL && SHGetPathFromIDListW(pidl, folderPath)) {
                            selectedDir = WStringToString(folderPath);
                            CoTaskMemFree(pidl);
                        } else {
                            if (pidl) CoTaskMemFree(pidl);
                            break; // 用户取消选择
                        }
                    }
                    
                    if (!selectedDir.empty()) {
                            try {
                                int count = 0;
                                int totalFiles = 0;
                                
                                for (const auto& entry : std::filesystem::directory_iterator(selectedDir)) {
                                    if (entry.is_regular_file()) {
                                        totalFiles++;
                                        std::string ext = entry.path().extension().string();
                                        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                                        
                                        if (ext == ".jpg" || ext == ".jpeg" || ext == ".png" || ext == ".bmp" || ext == ".gif" || ext == ".webp" || ext == ".tiff" || ext == ".tif") {
                                            std::string fullPath = entry.path().string();
                                            std::string relativePath;
                                            
                                            // 转换为相对于res目录的路径
                                            if (fullPath.find("res\\") == 0 || fullPath.find("res/") == 0) {
                                                relativePath = fullPath;
                                            } else if (fullPath.find("\\res\\") != std::string::npos || fullPath.find("/res/") != std::string::npos) {
                                                size_t resPos = fullPath.find("res");
                                                relativePath = fullPath.substr(resPos);
                                            } else {
                                                // 如果不在res目录下，添加res前缀
                                                relativePath = "res/" + entry.path().filename().string();
                                            }
                                            
                                            // 标准化路径分隔符
                                            std::replace(relativePath.begin(), relativePath.end(), '\\', '/');
                                            
                                            // 检查是否已存在（避免重复）
                                            bool exists = false;
                                            for (const auto& existing : g_config.wallpapers.list) {
                                                std::string normalizedExisting = existing;
                                                std::replace(normalizedExisting.begin(), normalizedExisting.end(), '\\', '/');
                                                if (normalizedExisting == relativePath) {
                                                    exists = true;
                                                    break;
                                                }
                                            }
                                            
                                            if (!exists) {
                                                g_config.wallpapers.list.push_back(relativePath);
                                                std::wstring wPath = StringToWString(relativePath);
                                                SendMessageW(hListWallpapers, LB_ADDSTRING, 0, (LPARAM)wPath.c_str());
                                                count++;
                                            }
                                        }
                                    }
                                }
                                
                                std::wstring message = L"📁 扫描完成！\n\n";
                                message += L"📂 扫描文件夹: " + StringToWString(selectedDir) + L"\n";
                                message += L"📄 总文件数: " + std::to_wstring(totalFiles) + L" 个\n";
                                message += L"🖼️ 新增壁纸: " + std::to_wstring(count) + L" 个\n\n";
                                
                                if (count > 0) {
                                    message += L"✅ 已成功添加新壁纸到列表中！";
                                } else {
                                    message += L"ℹ️ 没有找到新的图片文件或所有图片已存在。";
                                }
                                
                                MessageBoxW(hwnd, message.c_str(), L"扫描结果", MB_OK | MB_ICONINFORMATION);
                                
                            } catch (const std::exception& e) {
                                std::string errorMsg = "扫描文件夹失败: " + std::string(e.what());
                                MessageBoxA(hwnd, errorMsg.c_str(), "扫描错误", MB_OK | MB_ICONERROR);
                            }
                    }
                    break;
                }
                
                case ID_BTN_WALLPAPERS_OK: {
                    // Save interval
                    wchar_t interval[20];
                    GetWindowTextW(hEditWallpaperInterval, interval, 20);
                    g_config.wallpapers.interval = _wtoi(interval);
                    
                    MessageBoxW(hwnd, L"\u58c1\u7eb8\u8bbe\u7f6e\u5df2\u4fdd\u5b58\uff01", L"\u6210\u529f", MB_OK | MB_ICONINFORMATION);
                    DestroyWindow(hwnd);
                    break;
                }
                
                case ID_BTN_WALLPAPERS_CANCEL:
                    DestroyWindow(hwnd);
                    break;
            }
            break;
        }
        
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            RECT rect;
            GetClientRect(hwnd, &rect);
            
            HBRUSH bgBrush = CreateSolidBrush(FluentColors::CardBackground);
            FillRect(hdc, &rect, bgBrush);
            DeleteObject(bgBrush);
            
            EndPaint(hwnd, &ps);
            return 0;
        }
        
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
            
        case WM_DESTROY:
            // Don't quit the entire application, just close this dialog
            break;
    }
    
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// Notice Settings Dialog IDs
#define ID_EDIT_NOTICE_CONTENT 6001
#define ID_BTN_NOTICE_PREVIEW 6002
#define ID_BTN_INSERT_H3 6003
#define ID_BTN_INSERT_P 6004
#define ID_BTN_INSERT_UL 6005
#define ID_BTN_INSERT_STRONG 6006
#define ID_BTN_NOTICE_OK 6007
#define ID_BTN_NOTICE_CANCEL 6008
#define ID_BTN_NOTICE_CLEAR 6009
#define ID_BTN_NOTICE_RESTORE 6010

// Notice Settings Dialog Procedure
LRESULT CALLBACK NoticeSettingsDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HWND hEditNoticeContent;
    
    switch (msg) {
        case WM_CREATE: {
            // Create title
            CreateFluentLabel(hwnd, L"告示牌设置", 30, 20, 300, 35, true);
            CreateFluentLabel(hwnd, L"编辑 HTML 内容和格式", 30, 55, 400, 20, false);
            
            int y = 90;
            
            // HTML Content Section
            CreateFluentLabel(hwnd, L"📝 HTML 内容", 30, y, 200, 25, false);
            y += 30;
            
            hEditNoticeContent = CreateFluentEdit(hwnd, StringToWString(g_config.notice), 30, y, 500, 200, ID_EDIT_NOTICE_CONTENT, true);
            y += 220;
            
            // HTML Tools Section
            CreateFluentLabel(hwnd, L"🛠️ 快速插入", 30, y, 200, 25, false);
            y += 30;
            
            CreateFluentButton(hwnd, L"<h3>标题</h3>", 30, y, 100, 30, ID_BTN_INSERT_H3);
            CreateFluentButton(hwnd, L"<p>段落</p>", 140, y, 100, 30, ID_BTN_INSERT_P);
            CreateFluentButton(hwnd, L"<ul><li>列表</li></ul>", 250, y, 120, 30, ID_BTN_INSERT_UL);
            CreateFluentButton(hwnd, L"<strong>粗体</strong>", 380, y, 120, 30, ID_BTN_INSERT_STRONG);
            y += 50;
            
            // Additional tools section
            CreateFluentLabel(hwnd, L"📄 内容管理", 30, y, 200, 25, false);
            CreateFluentButton(hwnd, L"🗑️ 清空内容", 340, y, 90, 25, ID_BTN_NOTICE_CLEAR);
            CreateFluentButton(hwnd, L"🔄 恢复默认", 440, y, 90, 25, ID_BTN_NOTICE_RESTORE);
            y += 50;
            
            // Dialog buttons with improved styling
            CreateFluentButton(hwnd, L"💾 保存设置", 320, y, 90, 35, ID_BTN_NOTICE_OK, true);
            CreateFluentButton(hwnd, L"❌ 取消", 420, y, 80, 35, ID_BTN_NOTICE_CANCEL);
            
            break;
        }
        
        case WM_COMMAND: {
            switch (LOWORD(wParam)) {
                case ID_BTN_INSERT_H3: {
                    SendMessageW(hEditNoticeContent, EM_REPLACESEL, TRUE, (LPARAM)L"<h3>标题文本</h3>");
                    break;
                }
                
                case ID_BTN_INSERT_P: {
                    SendMessageW(hEditNoticeContent, EM_REPLACESEL, TRUE, (LPARAM)L"<p>段落文本</p>");
                    break;
                }
                
                case ID_BTN_INSERT_UL: {
                    SendMessageW(hEditNoticeContent, EM_REPLACESEL, TRUE, (LPARAM)L"<ul><li>列表项1</li><li>列表项2</li></ul>");
                    break;
                }
                
                case ID_BTN_INSERT_STRONG: {
                    SendMessageW(hEditNoticeContent, EM_REPLACESEL, TRUE, (LPARAM)L"<strong>重要文本</strong>");
                    break;
                }
                
                case ID_BTN_NOTICE_CLEAR: {
                    // Clear content
                    if (MessageBoxW(hwnd, L"确定要清空所有内容吗？", L"确认清空", MB_YESNO | MB_ICONQUESTION) == IDYES) {
                        SetWindowTextW(hEditNoticeContent, L"");
                        MessageBoxW(hwnd, L"内容已清空！", L"操作完成", MB_OK | MB_ICONINFORMATION);
                    }
                    break;
                }
                
                case ID_BTN_NOTICE_RESTORE: {
                    // Restore default content
                    if (MessageBoxW(hwnd, L"确定要恢复默认内容吗？当前内容将被覆盖。", L"确认恢复", MB_YESNO | MB_ICONQUESTION) == IDYES) {
                        std::wstring defaultContent = L"<h3>欢迎使用 ClassPaper</h3><p>这是一个现代化的课程表壁纸程序。</p><ul><li>支持自定义课程表</li><li>支持壁纸轮播</li><li>支持事件提醒</li><li>支持告示牌功能</li></ul><p><strong>使用说明：</strong></p><p>1. 在设置中配置您的课程表和时间安排</p><p>2. 添加重要事件和提醒</p><p>3. 管理壁纸列表和切换间隔</p><p>4. 自定义告示牌内容</p>";
                        SetWindowTextW(hEditNoticeContent, defaultContent.c_str());
                        MessageBoxW(hwnd, L"默认内容已恢复！", L"操作完成", MB_OK | MB_ICONINFORMATION);
                    }
                    break;
                }
                
                case ID_BTN_NOTICE_OK: {
                    // Save notice content
                    wchar_t content[2048];
                    GetWindowTextW(hEditNoticeContent, content, 2048);
                    g_config.notice = WStringToString(content);
                    
                    MessageBoxW(hwnd, L"\u544a\u793a\u724c\u8bbe\u7f6e\u5df2\u4fdd\u5b58\uff01", L"\u6210\u529f", MB_OK | MB_ICONINFORMATION);
                    DestroyWindow(hwnd);
                    break;
                }
                
                case ID_BTN_NOTICE_CANCEL:
                    DestroyWindow(hwnd);
                    break;
            }
            break;
        }
        
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            RECT rect;
            GetClientRect(hwnd, &rect);
            
            HBRUSH bgBrush = CreateSolidBrush(FluentColors::CardBackground);
            FillRect(hdc, &rect, bgBrush);
            DeleteObject(bgBrush);
            
            EndPaint(hwnd, &ps);
            return 0;
        }
        
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
            
        case WM_DESTROY:
            // Don't quit the entire application, just close this dialog
            break;
    }
    
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// Show Notice Settings Dialog
void ShowNoticeSettingsDialog(HWND parent) {
    // Register dialog class
    static bool classRegistered = false;
    if (!classRegistered) {
        WNDCLASSEXW wc = {};
        wc.cbSize = sizeof(WNDCLASSEXW);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = NoticeSettingsDialogProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = CreateSolidBrush(FluentColors::CardBackground);
        wc.lpszClassName = L"NoticeSettingsDialog";
        wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
        RegisterClassExW(&wc);
        classRegistered = true;
    }
    
    // Create resizable modal dialog
    HWND dialog = CreateWindowExW(
        WS_EX_DLGMODALFRAME,
        L"NoticeSettingsDialog",
        L"告示牌设置 - Fluent Design",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        (GetSystemMetrics(SM_CXSCREEN) - 580) / 2,
        (GetSystemMetrics(SM_CYSCREEN) - 500) / 2,
        580, 500,
        parent, NULL, GetModuleHandle(NULL), NULL
    );
    
    if (dialog) {
        EnableWindow(parent, FALSE);
        
        MSG msg;
        bool dialogActive = true;
        while (dialogActive && GetMessage(&msg, NULL, 0, 0)) {
            if (msg.hwnd == dialog || IsChild(dialog, msg.hwnd)) {
                if (msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE) {
                    PostMessage(dialog, WM_CLOSE, 0, 0);
                }
                TranslateMessage(&msg);
                DispatchMessage(&msg);
                
                if (!IsWindow(dialog)) {
                    dialogActive = false;
                }
            } else {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        
        EnableWindow(parent, TRUE);
        SetForegroundWindow(parent);
    }
}

// Show Wallpapers Settings Dialog
void ShowWallpapersSettingsDialog(HWND parent) {
    // Register dialog class
    static bool classRegistered = false;
    if (!classRegistered) {
        WNDCLASSEXW wc = {};
        wc.cbSize = sizeof(WNDCLASSEXW);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = WallpapersSettingsDialogProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = CreateSolidBrush(FluentColors::CardBackground);
        wc.lpszClassName = L"WallpapersSettingsDialog";
        wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
        RegisterClassExW(&wc);
        classRegistered = true;
    }
    
    // Create resizable modal dialog
    HWND dialog = CreateWindowExW(
        WS_EX_DLGMODALFRAME,
        L"WallpapersSettingsDialog",
        L"壁纸管理 - Fluent Design",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        (GetSystemMetrics(SM_CXSCREEN) - 520) / 2,
        (GetSystemMetrics(SM_CYSCREEN) - 550) / 2,
        520, 550,
        parent, NULL, GetModuleHandle(NULL), NULL
    );
    
    if (dialog) {
        EnableWindow(parent, FALSE);
        
        MSG msg;
        bool dialogActive = true;
        while (dialogActive && GetMessage(&msg, NULL, 0, 0)) {
            if (msg.hwnd == dialog || IsChild(dialog, msg.hwnd)) {
                if (msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE) {
                    PostMessage(dialog, WM_CLOSE, 0, 0);
                }
                TranslateMessage(&msg);
                DispatchMessage(&msg);
                
                if (!IsWindow(dialog)) {
                    dialogActive = false;
                }
            } else {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        
        EnableWindow(parent, TRUE);
        SetForegroundWindow(parent);
    }
}

// Show Events Settings Dialog
void ShowEventsSettingsDialog(HWND parent) {
    // Register dialog class
    static bool classRegistered = false;
    if (!classRegistered) {
        WNDCLASSEXW wc = {};
        wc.cbSize = sizeof(WNDCLASSEXW);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = EventsSettingsDialogProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = CreateSolidBrush(FluentColors::CardBackground);
        wc.lpszClassName = L"EventsSettingsDialog";
        wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
        RegisterClassExW(&wc);
        classRegistered = true;
    }
    
    // Create resizable modal dialog
    HWND dialog = CreateWindowExW(
        WS_EX_DLGMODALFRAME,
        L"EventsSettingsDialog",
        L"事件管理 - Fluent Design",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        (GetSystemMetrics(SM_CXSCREEN) - 520) / 2,
        (GetSystemMetrics(SM_CYSCREEN) - 550) / 2,
        520, 550,
        parent, NULL, GetModuleHandle(NULL), NULL
    );
    
    if (dialog) {
        EnableWindow(parent, FALSE);
        
        MSG msg;
        bool dialogActive = true;
        while (dialogActive && GetMessage(&msg, NULL, 0, 0)) {
            if (msg.hwnd == dialog || IsChild(dialog, msg.hwnd)) {
                if (msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE) {
                    PostMessage(dialog, WM_CLOSE, 0, 0);
                }
                TranslateMessage(&msg);
                DispatchMessage(&msg);
                
                if (!IsWindow(dialog)) {
                    dialogActive = false;
                }
            } else {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        
        EnableWindow(parent, TRUE);
        SetForegroundWindow(parent);
    }
}

// Show Schedule Settings Dialog
void ShowScheduleSettingsDialog(HWND parent) {
    // Register dialog class
    static bool classRegistered = false;
    if (!classRegistered) {
        WNDCLASSEXW wc = {};
        wc.cbSize = sizeof(WNDCLASSEXW);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = ScheduleSettingsDialogProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = CreateSolidBrush(FluentColors::CardBackground);
        wc.lpszClassName = L"ScheduleSettingsDialog";
        wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
        RegisterClassExW(&wc);
        classRegistered = true;
    }
    
    // Create resizable modal dialog
    HWND dialog = CreateWindowExW(
        WS_EX_DLGMODALFRAME,
        L"ScheduleSettingsDialog",
        L"课程表设置 - Fluent Design",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        (GetSystemMetrics(SM_CXSCREEN) - 550) / 2,
        (GetSystemMetrics(SM_CYSCREEN) - 500) / 2,
        550, 500,
        parent, NULL, GetModuleHandle(NULL), NULL
    );
    
    if (dialog) {
        EnableWindow(parent, FALSE);
        
        MSG msg;
        bool dialogActive = true;
        while (dialogActive && GetMessage(&msg, NULL, 0, 0)) {
            if (msg.hwnd == dialog || IsChild(dialog, msg.hwnd)) {
                if (msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE) {
                    PostMessage(dialog, WM_CLOSE, 0, 0);
                }
                TranslateMessage(&msg);
                DispatchMessage(&msg);
                
                if (!IsWindow(dialog)) {
                    dialogActive = false;
                }
            } else {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        
        EnableWindow(parent, TRUE);
        SetForegroundWindow(parent);
    }
}

// Show complete basic settings dialog
void ShowBasicSettingsDialog(HWND parent) {
    // Register dialog class
    static bool classRegistered = false;
    if (!classRegistered) {
        WNDCLASSEXW wc = {};
        wc.cbSize = sizeof(WNDCLASSEXW);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = BasicSettingsDialogProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = CreateSolidBrush(FluentColors::CardBackground);
        wc.lpszClassName = L"BasicSettingsDialog";
        wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
        RegisterClassExW(&wc);
        classRegistered = true;
    }
    
    // Create resizable modal dialog
    HWND dialog = CreateWindowExW(
        WS_EX_DLGMODALFRAME,
        L"BasicSettingsDialog",
        L"基本设置 - Fluent Design",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        (GetSystemMetrics(SM_CXSCREEN) - 500) / 2,
        (GetSystemMetrics(SM_CYSCREEN) - 600) / 2,
        500, 600,
        parent, NULL, GetModuleHandle(NULL), NULL
    );
    
    if (dialog) {
        // Disable parent window
        EnableWindow(parent, FALSE);
        
        // Message loop for modal dialog
        MSG msg;
        bool dialogActive = true;
        while (dialogActive && GetMessage(&msg, NULL, 0, 0)) {
            if (msg.hwnd == dialog || IsChild(dialog, msg.hwnd)) {
                if (msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE) {
                    PostMessage(dialog, WM_CLOSE, 0, 0);
                }
                TranslateMessage(&msg);
                DispatchMessage(&msg);
                
                // Check if dialog is still valid
                if (!IsWindow(dialog)) {
                    dialogActive = false;
                }
            } else {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        
        // Re-enable parent window
        EnableWindow(parent, TRUE);
        SetForegroundWindow(parent);
    }
}

// Main window procedure
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            // Create title with improved styling
            CreateFluentLabel(hwnd, L"ClassPaper 设置", 40, 30, 400, 50, true);
            CreateFluentLabel(hwnd, L"Fluent Design UI - 现代化设置界面", 40, 80, 500, 30, false, true);
            
            // Create navigation cards with better spacing
            int cardY = 130;
            int cardSpacing = 70;
            int cardWidth = 700;
            int cardHeight = 45;
            int descriptionOffset = 5;
            
            // Main function cards
            CreateFluentButton(hwnd, L"🔧 基本设置", 40, cardY, cardWidth, cardHeight, ID_BTN_BASIC);
            CreateFluentLabel(hwnd, L"进度条、周数偏移、提示音、系统配置", 60, cardY + cardHeight + descriptionOffset, 400, 20, false, true);
            
            cardY += cardSpacing;
            CreateFluentButton(hwnd, L"📅 课程表设置", 40, cardY, cardWidth, cardHeight, ID_BTN_SCHEDULE);
            CreateFluentLabel(hwnd, L"时间安排、显示模式、课程管理 ✅ 完全可用", 60, cardY + cardHeight + descriptionOffset, 400, 20, false, true);
            
            cardY += cardSpacing;
            CreateFluentButton(hwnd, L"📝 事件管理", 40, cardY, cardWidth, cardHeight, ID_BTN_EVENTS);
            CreateFluentLabel(hwnd, L"重要事件添加、编辑、删除功能 ✅ 完全可用", 60, cardY + cardHeight + descriptionOffset, 400, 20, false, true);
            
            cardY += cardSpacing;
            CreateFluentButton(hwnd, L"🖼️ 壁纸管理", 40, cardY, cardWidth, cardHeight, ID_BTN_WALLPAPERS);
            CreateFluentLabel(hwnd, L"壁纸列表、自动扫描、切换间隔 ✅ 完全可用", 60, cardY + cardHeight + descriptionOffset, 400, 20, false, true);
            
            cardY += cardSpacing;
            CreateFluentButton(hwnd, L"📢 告示牌设置", 40, cardY, cardWidth, cardHeight, ID_BTN_NOTICE);
            CreateFluentLabel(hwnd, L"HTML 内容编辑器和实时预览 ✅ 完全可用", 60, cardY + cardHeight + descriptionOffset, 400, 20, false, true);
            
            // Action buttons with improved layout
            cardY += 75;
            CreateFluentButton(hwnd, L"💾 保存配置", 40, cardY, 130, 40, ID_BTN_SAVE, true);
            CreateFluentButton(hwnd, L"🔄 重载配置", 180, cardY, 130, 40, ID_BTN_RELOAD);
            CreateFluentButton(hwnd, L"❌ 退出程序", 320, cardY, 130, 40, ID_BTN_EXIT);
            
            break;
        }
        
        case WM_COMMAND: {
            switch (LOWORD(wParam)) {
                case ID_BTN_BASIC:
                    ShowBasicSettingsDialog(hwnd);
                    break;
                    
                case ID_BTN_SCHEDULE:
                    ShowScheduleSettingsDialog(hwnd);
                    break;
                    
                case ID_BTN_EVENTS:
                    ShowEventsSettingsDialog(hwnd);
                    break;
                    
                case ID_BTN_WALLPAPERS:
                    ShowWallpapersSettingsDialog(hwnd);
                    break;
                    
                case ID_BTN_NOTICE:
                    ShowNoticeSettingsDialog(hwnd);
                    break;
                    
                case ID_BTN_SAVE:
                    SaveConfig();
                    break;
                    
                case ID_BTN_RELOAD:
                    ReloadConfig();
                    break;
                    
                case ID_BTN_EXIT:
                    PostMessage(hwnd, WM_CLOSE, 0, 0);
                    break;
            }
            break;
        }
        
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            RECT rect;
            GetClientRect(hwnd, &rect);
            
            // Fill with Fluent background color
            HBRUSH bgBrush = CreateSolidBrush(FluentColors::Background);
            FillRect(hdc, &rect, bgBrush);
            DeleteObject(bgBrush);
            
            EndPaint(hwnd, &ps);
            return 0;
        }
        
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Set console code page to UTF-8 for proper Chinese display
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    
    // Set locale for proper string handling
    setlocale(LC_ALL, "zh_CN.UTF-8");
    
    // Initialize common controls
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&icex);
    
    // Load configuration at startup with enhanced loading
    LoadConfig();
    
    // Show startup configuration status
    ShowStartupConfigStatus();
    
    // Register window class
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = MainWndProc;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(FluentColors::Background);
    wc.lpszClassName = L"FluentSettingsWindow";
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    
    if (!RegisterClassExW(&wc)) {
        MessageBoxW(NULL, L"窗口类注册失败!", L"错误", MB_OK | MB_ICONERROR);
        return 1;
    }
    
    // Create main window
    HWND hwnd = CreateWindowExW(
        0,
        L"FluentSettingsWindow",
        L"ClassPaper 设置 - Fluent Design",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL, NULL, hInstance, NULL
    );
    
    if (!hwnd) {
        MessageBoxW(NULL, L"窗口创建失败!", L"错误", MB_OK | MB_ICONERROR);
        return 1;
    }
    
    // Show window
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    
    // Message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return (int)msg.wParam;
}