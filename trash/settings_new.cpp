// ClassPaper Settings Program - Modern Native Implementation
// Enhanced with modern UI and proper Chinese character support

#include "toml.hpp"
#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <shellapi.h>
#include <uxtheme.h>
#include <dwmapi.h>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <algorithm>
#include <vector>
#include <map>
#include <locale>
#include <codecvt>
#include <windowsx.h> // For GET_X_LPARAM, GET_Y_LPARAM

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "uxtheme.lib")
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")

// Modern UI constants
#define MODERN_BUTTON_HEIGHT 32
#define MODERN_EDIT_HEIGHT 28
#define MODERN_SPACING 12
#define MODERN_MARGIN 20
#define MODERN_GROUP_SPACING 24

// Modern Fluent Design color scheme
#define FLUENT_BG_COLOR RGB(243, 243, 243)
#define FLUENT_CARD_COLOR RGB(255, 255, 255)
#define FLUENT_ACCENT_COLOR RGB(0, 120, 215)
#define FLUENT_ACCENT_HOVER RGB(16, 110, 190)
#define FLUENT_ACCENT_PRESSED RGB(8, 103, 171)
#define FLUENT_BORDER_COLOR RGB(225, 225, 225)
#define FLUENT_TEXT_PRIMARY RGB(32, 31, 30)
#define FLUENT_TEXT_SECONDARY RGB(96, 94, 92)
#define FLUENT_SHADOW_COLOR RGB(0, 0, 0)
#define FLUENT_BUTTON_BG RGB(251, 251, 251)
#define FLUENT_BUTTON_HOVER RGB(246, 246, 246)
#define FLUENT_BUTTON_PRESSED RGB(240, 240, 240)

// UTF-8 to UTF-16 conversion helper
std::wstring utf8_to_utf16(const std::string& utf8) {
    if (utf8.empty()) return std::wstring();
    
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &utf8[0], (int)utf8.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &utf8[0], (int)utf8.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

// UTF-16 to UTF-8 conversion helper
std::string utf16_to_utf8(const std::wstring& utf16) {
    if (utf16.empty()) return std::string();
    
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &utf16[0], (int)utf16.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &utf16[0], (int)utf16.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

// Modern Fluent Design UI helper functions
HFONT CreateFluentFont(int size = 9, bool bold = false) {
    return CreateFontW(
        -MulDiv(size, GetDeviceCaps(GetDC(NULL), LOGPIXELSY), 72), 0, 0, 0,
        bold ? FW_SEMIBOLD : FW_NORMAL,
        FALSE, FALSE, FALSE,
        DEFAULT_CHARSET,
        OUT_TT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE,
        L"Segoe UI Variable Display"
    );
}

// Create rounded rectangle region
HRGN CreateRoundRectRgn(int left, int top, int right, int bottom, int radius) {
    return CreateRoundRectRgn(left, top, right, bottom, radius, radius);
}

// Draw modern card background with shadow effect
void DrawFluentCard(HDC hdc, RECT rect, int cornerRadius = 8) {
    // Create rounded rectangle region
    HRGN hRgn = CreateRoundRectRgn(rect.left, rect.top, rect.right, rect.bottom, cornerRadius);
    
    // Fill with card color
    HBRUSH hCardBrush = CreateSolidBrush(FLUENT_CARD_COLOR);
    FillRgn(hdc, hRgn, hCardBrush);
    
    // Draw border
    HPEN hBorderPen = CreatePen(PS_SOLID, 1, FLUENT_BORDER_COLOR);
    HPEN hOldPen = (HPEN)SelectObject(hdc, hBorderPen);
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
    
    RoundRect(hdc, rect.left, rect.top, rect.right, rect.bottom, cornerRadius, cornerRadius);
    
    SelectObject(hdc, hOldPen);
    SelectObject(hdc, hOldBrush);
    DeleteObject(hBorderPen);
    DeleteObject(hCardBrush);
    DeleteObject(hRgn);
}

// Custom button drawing for Fluent Design
void DrawFluentButton(HDC hdc, RECT rect, const wchar_t* text, bool isHovered, bool isPressed, bool isAccent = false) {
    int cornerRadius = 4;
    COLORREF bgColor;
    COLORREF textColor = FLUENT_TEXT_PRIMARY;
    
    if (isAccent) {
        if (isPressed) bgColor = FLUENT_ACCENT_PRESSED;
        else if (isHovered) bgColor = FLUENT_ACCENT_HOVER;
        else bgColor = FLUENT_ACCENT_COLOR;
        textColor = RGB(255, 255, 255);
    } else {
        if (isPressed) bgColor = FLUENT_BUTTON_PRESSED;
        else if (isHovered) bgColor = FLUENT_BUTTON_HOVER;
        else bgColor = FLUENT_BUTTON_BG;
    }
    
    // Draw button background
    HBRUSH hBrush = CreateSolidBrush(bgColor);
    HPEN hPen = CreatePen(PS_SOLID, 1, isAccent ? bgColor : FLUENT_BORDER_COLOR);
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
    
    RoundRect(hdc, rect.left, rect.top, rect.right, rect.bottom, cornerRadius, cornerRadius);
    
    // Draw text
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, textColor);
    HFONT hFont = CreateFluentFont(10, false);
    HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
    
    DrawTextW(hdc, text, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    
    SelectObject(hdc, hOldFont);
    SelectObject(hdc, hOldPen);
    SelectObject(hdc, hOldBrush);
    DeleteObject(hFont);
    DeleteObject(hBrush);
    DeleteObject(hPen);
}

void ApplyFluentStyle(HWND hwnd) {
    // Enable visual styles and modern theming
    SetWindowTheme(hwnd, L"DarkMode_Explorer", NULL);
    
    // Apply custom drawing for buttons
    WCHAR className[256];
    GetClassNameW(hwnd, className, 256);
    
    if (wcscmp(className, L"Button") == 0) {
        // Set owner draw style for custom button rendering
        LONG style = GetWindowLong(hwnd, GWL_STYLE);
        SetWindowLong(hwnd, GWL_STYLE, style | BS_OWNERDRAW);
    }
}

// Configuration structure based on config.js
struct Config {
    // Basic settings
    std::string progressDescription = "高三剩余";
    std::string progressPercentMode = "left"; // "left" or "passed"
    std::string semesterBegin = "2023-07-31";
    std::string semesterEnd = "2026-06-07";
    
    // Week offset settings
    bool weekOffsetEnabled = true;
    int weekOffset = 7;
    
    // Notification settings
    bool notificationsEnabled = true;
    int regularInterval = 5;
    int endingTime = 5;
    
    // System settings
    std::string url = "./res/index.html";
    std::string browserPath = "";
    
    // Schedule/lessons settings
    struct {
        std::vector<std::string> headers = {"星期", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11"};
        std::string displayMode = "scroll"; // "scroll" or "day"
        struct LessonDay {
            std::string day;
            std::vector<std::string> classes;
        };
        std::vector<LessonDay> schedule;
        struct {
            struct {
                std::string begin;
                std::string end;
            } semester;
            struct TimeSlot {
                int period;
                std::string begin;
                std::string end;
                std::string rest;
            };
            std::vector<TimeSlot> schedule;
        } times;
    } lessons;
    
    // Events settings
    struct Event {
        std::string name;
        std::string date;
    };
    std::vector<Event> events;
    
    // Wallpapers settings
    std::vector<std::string> wallpapers;
    int wallpaperInterval = 30; // seconds
    
    // Notice settings
    std::string sth = ""; // notice content
    
    // All modules completed
};

Config currentConfig;

// Function declarations
bool LoadConfig();
bool SaveConfig();
void ShowBasicSettingsDialog();
void ShowScheduleSettingsDialog();
void ShowEventsSettingsDialog();
void ShowWallpapersSettingsDialog();
void ShowNoticeSettingsDialog();
std::string SelectFile(HWND hwnd, const char* filter = "HTML Files\0*.html;*.htm\0All Files\0*.*\0");
std::string SelectBrowser(HWND hwnd);
void OpenInBrowser(const std::string& url);

// Load configuration from both config.toml and config.js
bool LoadConfig() {
    try {
        // Load basic settings from config.toml
        if (std::filesystem::exists("config.toml")) {
            const auto config = toml::parse("config.toml");
            currentConfig.url = toml::find_or(config, "default", "url", currentConfig.url);
            currentConfig.browserPath = toml::find_or(config, "default", "browser_path", currentConfig.browserPath);
        }
        
        // Load extended settings from config.js (parse as text for now)
        if (std::filesystem::exists("res/config/config.js")) {
            std::ifstream file("res/config/config.js");
            std::string content((std::istreambuf_iterator<char>(file)),
                               std::istreambuf_iterator<char>());
            
            // Simple parsing for basic settings (we'll improve this in later steps)
            // For now, just extract some basic values using string search
            size_t pos;
            
            // Parse progressDescription
            pos = content.find("\"progressDescription\":");
            if (pos != std::string::npos) {
                size_t start = content.find("\"", pos + 20);
                size_t end = content.find("\"", start + 1);
                if (start != std::string::npos && end != std::string::npos) {
                    currentConfig.progressDescription = content.substr(start + 1, end - start - 1);
                }
            }
            
            // Parse progressPercentMode
            pos = content.find("\"progressPercentMode\":");
            if (pos != std::string::npos) {
                size_t start = content.find("\"", pos + 20);
                size_t end = content.find("\"", start + 1);
                if (start != std::string::npos && end != std::string::npos) {
                    currentConfig.progressPercentMode = content.substr(start + 1, end - start - 1);
                }
            }
            
            // Parse semester dates
            pos = content.find("\"begin\":");
            if (pos != std::string::npos) {
                size_t start = content.find("\"", pos + 8);
                size_t end = content.find("\"", start + 1);
                if (start != std::string::npos && end != std::string::npos) {
                    currentConfig.semesterBegin = content.substr(start + 1, end - start - 1);
                }
            }
            
            pos = content.find("\"end\":");
            if (pos != std::string::npos) {
                size_t start = content.find("\"", pos + 6);
                size_t end = content.find("\"", start + 1);
                if (start != std::string::npos && end != std::string::npos) {
                    currentConfig.semesterEnd = content.substr(start + 1, end - start - 1);
                }
            }
        }
        
        std::cout << "Configuration loaded successfully" << std::endl;
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading config: " << e.what() << std::endl;
        return false;
    }
}

// Save configuration to both config.toml and config.js
bool SaveConfig() {
    try {
        // Save basic settings to config.toml
        std::ofstream tomlFile("config.toml");
        if (!tomlFile.is_open()) {
            std::cerr << "Failed to open config.toml for writing" << std::endl;
            return false;
        }

        tomlFile << "\n[default]\n";
        tomlFile << "  browser_path = \"" << currentConfig.browserPath << "\"\n";
        tomlFile << "  url = \"" << currentConfig.url << "\"\n";
        tomlFile.close();
        
        // Save complete settings to config.js
        std::ofstream jsFile("res/config/config.js");
        if (!jsFile.is_open()) {
            std::cerr << "Failed to open res/config/config.js for writing" << std::endl;
            return false;
        }
        
        jsFile << "const CONFIG = {\n";
        jsFile << "  \"lessons\": {\n";
        jsFile << "    \"headers\": [\n";
        for (size_t i = 0; i < currentConfig.lessons.headers.size(); ++i) {
            jsFile << "      \"" << currentConfig.lessons.headers[i] << "\"";
            if (i < currentConfig.lessons.headers.size() - 1) jsFile << ",";
            jsFile << "\n";
        }
        jsFile << "    ],\n";
        jsFile << "    \"displayMode\": \"" << currentConfig.lessons.displayMode << "\",\n";
        jsFile << "    \"schedule\": [\n";
        for (size_t i = 0; i < currentConfig.lessons.schedule.size(); ++i) {
            const auto& day = currentConfig.lessons.schedule[i];
            jsFile << "      {\n";
            jsFile << "        \"day\": \"" << day.day << "\",\n";
            jsFile << "        \"classes\": [\n";
            for (size_t j = 0; j < day.classes.size(); ++j) {
                jsFile << "          \"" << day.classes[j] << "\"";
                if (j < day.classes.size() - 1) jsFile << ",";
                jsFile << "\n";
            }
            jsFile << "        ]\n";
            jsFile << "      }";
            if (i < currentConfig.lessons.schedule.size() - 1) jsFile << ",";
            jsFile << "\n";
        }
        jsFile << "    ],\n";
        jsFile << "    \"times\": {\n";
        jsFile << "      \"semester\": {\n";
        jsFile << "        \"begin\": \"" << currentConfig.semesterBegin << "\",\n";
        jsFile << "        \"end\": \"" << currentConfig.semesterEnd << "\"\n";
        jsFile << "      },\n";
        jsFile << "      \"schedule\": [\n";
        for (size_t i = 0; i < currentConfig.lessons.times.schedule.size(); ++i) {
            const auto& timeSlot = currentConfig.lessons.times.schedule[i];
            jsFile << "        {\n";
            jsFile << "          \"period\": " << timeSlot.period << ",\n";
            jsFile << "          \"begin\": \"" << timeSlot.begin << "\",\n";
            jsFile << "          \"end\": \"" << timeSlot.end << "\",\n";
            if (!timeSlot.rest.empty()) {
                jsFile << "          \"rest\": \"" << timeSlot.rest << "\"\n";
            } else {
                jsFile << "          \"rest\": null\n";
            }
            jsFile << "        }";
            if (i < currentConfig.lessons.times.schedule.size() - 1) jsFile << ",";
            jsFile << "\n";
        }
        jsFile << "      ]\n";
        jsFile << "    }\n";
        jsFile << "  },\n";
        jsFile << "  \"weekOffset\": {\n";
        jsFile << "    \"enabled\": " << (currentConfig.weekOffsetEnabled ? "true" : "false") << ",\n";
        jsFile << "    \"offset\": " << currentConfig.weekOffset << "\n";
        jsFile << "  },\n";
        jsFile << "  \"notifications\": {\n";
        jsFile << "    \"enabled\": " << (currentConfig.notificationsEnabled ? "true" : "false") << ",\n";
        jsFile << "    \"regularInterval\": " << currentConfig.regularInterval << ",\n";
        jsFile << "    \"endingTime\": " << currentConfig.endingTime << "\n";
        jsFile << "  },\n";
        jsFile << "  \"events\": [\n";
        for (size_t i = 0; i < currentConfig.events.size(); ++i) {
            const auto& event = currentConfig.events[i];
            jsFile << "    {\n";
            jsFile << "      \"name\": \"" << event.name << "\",\n";
            jsFile << "      \"date\": \"" << event.date << "\"\n";
            jsFile << "    }";
            if (i < currentConfig.events.size() - 1) jsFile << ",";
            jsFile << "\n";
        }
        jsFile << "  ],\n";
        jsFile << "  \"wallpapers\": [\n";
        for (size_t i = 0; i < currentConfig.wallpapers.size(); ++i) {
            jsFile << "    \"" << currentConfig.wallpapers[i] << "\"";
            if (i < currentConfig.wallpapers.size() - 1) jsFile << ",";
            jsFile << "\n";
        }
        jsFile << "  ],\n";
        jsFile << "  \"wallpaperInterval\": " << currentConfig.wallpaperInterval << ",\n";
        jsFile << "  \"progressDescription\": \"" << currentConfig.progressDescription << "\",\n";
        jsFile << "  \"progressPercentMode\": \"" << currentConfig.progressPercentMode << "\",\n";
        jsFile << "  \"sth\": \"";
        
        // Escape quotes and newlines in notice content
        std::string escapedSth = currentConfig.sth;
        size_t pos = 0;
        while ((pos = escapedSth.find("\"", pos)) != std::string::npos) {
            escapedSth.replace(pos, 1, "\\\"");
            pos += 2;
        }
        pos = 0;
        while ((pos = escapedSth.find("\n", pos)) != std::string::npos) {
            escapedSth.replace(pos, 1, "\\n");
            pos += 2;
        }
        pos = 0;
        while ((pos = escapedSth.find("\r", pos)) != std::string::npos) {
            escapedSth.replace(pos, 1, "\\r");
            pos += 2;
        }
        
        jsFile << escapedSth << "\"\n";
        jsFile << "};\n\n";
        
        // Add backward compatibility exports
        jsFile << "// 为了保持向后兼容，导出原有的变量名\n";
        jsFile << "const lessons = CONFIG.lessons.headers.join(\",\") + \"\\n\" + \n";
        jsFile << "  CONFIG.lessons.schedule.map(day => \" ,\" + day.day + \",\" + day.classes.join(\",\")).join(\"\\n\") + \"\\n\";\n\n";
        jsFile << "const events = \"事件,日期,\\n\" + \n";
        jsFile << "  CONFIG.events.map(event => `${event.name},${event.date},`).join(\"\\n\");\n\n";
        jsFile << "const wallpaperlist = CONFIG.wallpapers;\n\n";
        jsFile << "const sth = CONFIG.sth;\n";
        
        jsFile.close();
        
        std::cout << "Configuration saved successfully to both config.toml and config.js" << std::endl;
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error saving config: " << e.what() << std::endl;
        return false;
    }
}

// File selection dialog
std::string SelectFile(HWND hwnd, const char* filter) {
    OPENFILENAMEA ofn;
    char szFile[260] = {0};
    
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = filter;
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = nullptr;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = nullptr;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    
    if (GetOpenFileNameA(&ofn)) {
        std::string selected_file = szFile;
        // Convert to relative path if possible
        std::filesystem::path current_dir = std::filesystem::current_path();
        std::filesystem::path file_path = selected_file;
        
        try {
            std::filesystem::path relative = std::filesystem::relative(file_path, current_dir);
            selected_file = relative.string();
            std::replace(selected_file.begin(), selected_file.end(), '\\', '/');
        } catch (...) {
            // If relative path fails, use absolute path
        }
        
        return selected_file;
    }
    return "";
}

// Browser selection dialog
std::string SelectBrowser(HWND hwnd) {
    OPENFILENAMEA ofn;
    char szFile[260] = {0};
    
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "Executable Files\0*.exe\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = nullptr;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = nullptr;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    
    if (GetOpenFileNameA(&ofn)) {
        return std::string(szFile);
    }
    return "";
}

// Open URL in browser
void OpenInBrowser(const std::string& url) {
    std::string finalUrl = url;
    
    // Check if it's a local file
    if (finalUrl.find("http://") != 0 && finalUrl.find("https://") != 0) {
        // Convert to absolute path for local files
        std::filesystem::path abs_path = std::filesystem::absolute(finalUrl);
        finalUrl = "file:///" + abs_path.string();
        // Replace backslashes with forward slashes for file URLs
        std::replace(finalUrl.begin(), finalUrl.end(), '\\', '/');
    }
    
    HINSTANCE result;
    if (!currentConfig.browserPath.empty()) {
        // Use specified browser
        result = ShellExecuteA(nullptr, "open", currentConfig.browserPath.c_str(), 
                             finalUrl.c_str(), nullptr, SW_SHOWDEFAULT);
    } else {
        // Use default browser
        result = ShellExecuteA(nullptr, "open", finalUrl.c_str(), 
                             nullptr, nullptr, SW_SHOWDEFAULT);
    }
    
    if (reinterpret_cast<intptr_t>(result) <= 32) {
        MessageBoxA(nullptr, "Failed to open browser", "Error", MB_OK | MB_ICONERROR);
    }
}

// Basic Settings Dialog Procedure
INT_PTR WINAPI BasicSettingsDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_INITDIALOG:
        {
            // Set window title
            SetWindowTextA(hDlg, "ClassPaper 基本设置");
            
            // Initialize controls with current values
            SetDlgItemTextA(hDlg, 1001, currentConfig.progressDescription.c_str());
            SetDlgItemTextA(hDlg, 1002, currentConfig.semesterBegin.c_str());
            SetDlgItemTextA(hDlg, 1003, currentConfig.semesterEnd.c_str());
            
            // Set radio buttons for progress percent mode
            CheckDlgButton(hDlg, 1004, currentConfig.progressPercentMode == "left" ? BST_CHECKED : BST_UNCHECKED);
            CheckDlgButton(hDlg, 1005, currentConfig.progressPercentMode == "passed" ? BST_CHECKED : BST_UNCHECKED);
            
            // Set week offset settings
            CheckDlgButton(hDlg, 1006, currentConfig.weekOffsetEnabled ? BST_CHECKED : BST_UNCHECKED);
            SetDlgItemInt(hDlg, 1007, currentConfig.weekOffset, FALSE);
            
            // Set notification settings
            CheckDlgButton(hDlg, 1008, currentConfig.notificationsEnabled ? BST_CHECKED : BST_UNCHECKED);
            SetDlgItemInt(hDlg, 1009, currentConfig.regularInterval, FALSE);
            SetDlgItemInt(hDlg, 1010, currentConfig.endingTime, FALSE);
            
            // Set system settings
            SetDlgItemTextA(hDlg, 1011, currentConfig.url.c_str());
            SetDlgItemTextA(hDlg, 1012, currentConfig.browserPath.c_str());
            
            return TRUE;
        }
        
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case 1013: // Browse File button
            {
                std::string file = SelectFile(hDlg);
                if (!file.empty()) {
                    SetDlgItemTextA(hDlg, 1011, file.c_str());
                }
                break;
            }
        case 1014: // Browse Browser button
            {
                std::string browser = SelectBrowser(hDlg);
                if (!browser.empty()) {
                    SetDlgItemTextA(hDlg, 1012, browser.c_str());
                }
                break;
            }
        case 1015: // Test in Browser button
            {
                char url_buffer[512];
                GetDlgItemTextA(hDlg, 1011, url_buffer, sizeof(url_buffer));
                if (strlen(url_buffer) > 0) {
                    OpenInBrowser(url_buffer);
                } else {
                    MessageBoxA(hDlg, "请先输入URL", "错误", MB_OK | MB_ICONERROR);
                }
                break;
            }
        case IDOK: // Save button
            {
                // Get values from controls
                char buffer[512];
                
                GetDlgItemTextA(hDlg, 1001, buffer, sizeof(buffer));
                currentConfig.progressDescription = buffer;
                
                GetDlgItemTextA(hDlg, 1002, buffer, sizeof(buffer));
                currentConfig.semesterBegin = buffer;
                
                GetDlgItemTextA(hDlg, 1003, buffer, sizeof(buffer));
                currentConfig.semesterEnd = buffer;
                
                currentConfig.progressPercentMode = IsDlgButtonChecked(hDlg, 1004) ? "left" : "passed";
                currentConfig.weekOffsetEnabled = IsDlgButtonChecked(hDlg, 1006) == BST_CHECKED;
                currentConfig.weekOffset = GetDlgItemInt(hDlg, 1007, nullptr, FALSE);
                currentConfig.notificationsEnabled = IsDlgButtonChecked(hDlg, 1008) == BST_CHECKED;
                currentConfig.regularInterval = GetDlgItemInt(hDlg, 1009, nullptr, FALSE);
                currentConfig.endingTime = GetDlgItemInt(hDlg, 1010, nullptr, FALSE);
                
                GetDlgItemTextA(hDlg, 1011, buffer, sizeof(buffer));
                currentConfig.url = buffer;
                
                GetDlgItemTextA(hDlg, 1012, buffer, sizeof(buffer));
                currentConfig.browserPath = buffer;
                
                // Save configuration
                if (SaveConfig()) {
                    MessageBoxA(hDlg, "设置保存成功", "成功", MB_OK | MB_ICONINFORMATION);
                } else {
                    MessageBoxA(hDlg, "设置保存失败", "错误", MB_OK | MB_ICONERROR);
                }
                
                EndDialog(hDlg, IDOK);
                break;
            }
        case IDCANCEL:
            EndDialog(hDlg, IDCANCEL);
            break;
        }
        break;
        
    default:
        return FALSE;
    }
    return TRUE;
}

// Create dialog template in memory
std::vector<BYTE> CreateBasicSettingsDialogTemplate() {
    // This is a simplified approach - in a real application, you'd use a resource file
    // For now, we'll create the dialog programmatically
    std::vector<BYTE> template_data;
    // TODO: Implement proper dialog template creation
    return template_data;
}

// Modern Basic Settings Dialog Procedure
LRESULT CALLBACK ModernBasicSettingsProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HFONT hFont = nullptr;
    static HFONT hTitleFont = nullptr;
    static HBRUSH hBackgroundBrush = nullptr;
    
    switch (msg) {
    case WM_CREATE:
        {
            // Initialize modern UI
            hFont = CreateFluentFont(9, false);
            hTitleFont = CreateFluentFont(11, true);
            hBackgroundBrush = CreateSolidBrush(FLUENT_BG_COLOR);
            
            // Create modern controls with proper Unicode support
            int y = MODERN_MARGIN;
            const int labelWidth = 150;
            const int controlWidth = 220;
            const int controlHeight = MODERN_EDIT_HEIGHT;
            
            // Progress settings section
            HWND hProgressTitle = CreateWindowW(L"STATIC", L"📊 进度条设�?,
                WS_VISIBLE | WS_CHILD | SS_LEFT,
                MODERN_MARGIN, y, 300, 25, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);
            SendMessage(hProgressTitle, WM_SETFONT, (WPARAM)hTitleFont, TRUE);
            y += MODERN_GROUP_SPACING + 10;
            
            CreateWindowW(L"STATIC", L"进度条描�?",
                WS_VISIBLE | WS_CHILD | SS_LEFT,
                MODERN_MARGIN + 10, y, labelWidth, 20, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);
            HWND hProgressDesc = CreateWindowW(L"EDIT", utf8_to_utf16(currentConfig.progressDescription).c_str(),
                WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
                MODERN_MARGIN + 10 + labelWidth, y, controlWidth, controlHeight,
                hwnd, (HMENU)1001, GetModuleHandle(nullptr), nullptr);
            SendMessage(hProgressDesc, WM_SETFONT, (WPARAM)hFont, TRUE);
            ApplyFluentStyle(hProgressDesc);
            y += controlHeight + MODERN_SPACING;
            
            CreateWindowW(L"STATIC", L"开始时�?",
                WS_VISIBLE | WS_CHILD | SS_LEFT,
                MODERN_MARGIN + 10, y, labelWidth, 20, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);
            HWND hSemesterBegin = CreateWindowW(L"EDIT", utf8_to_utf16(currentConfig.semesterBegin).c_str(),
                WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
                MODERN_MARGIN + 10 + labelWidth, y, controlWidth, controlHeight,
                hwnd, (HMENU)1002, GetModuleHandle(nullptr), nullptr);
            SendMessage(hSemesterBegin, WM_SETFONT, (WPARAM)hFont, TRUE);
            ApplyFluentStyle(hSemesterBegin);
            y += controlHeight + MODERN_SPACING;
            
            CreateWindowW(L"STATIC", L"结束时间:",
                WS_VISIBLE | WS_CHILD | SS_LEFT,
                MODERN_MARGIN + 10, y, labelWidth, 20, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);
            HWND hSemesterEnd = CreateWindowW(L"EDIT", utf8_to_utf16(currentConfig.semesterEnd).c_str(),
                WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
                MODERN_MARGIN + 10 + labelWidth, y, controlWidth, controlHeight,
                hwnd, (HMENU)1003, GetModuleHandle(nullptr), nullptr);
            SendMessage(hSemesterEnd, WM_SETFONT, (WPARAM)hFont, TRUE);
            ApplyFluentStyle(hSemesterEnd);
            y += controlHeight + MODERN_SPACING;
            
            // Progress percent mode
            CreateWindowW(L"STATIC", L"百分比显�?",
                WS_VISIBLE | WS_CHILD | SS_LEFT,
                MODERN_MARGIN + 10, y, labelWidth, 20, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);
            HWND hRadioLeft = CreateWindowW(L"BUTTON", L"显示剩余",
                WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | WS_GROUP,
                MODERN_MARGIN + 10 + labelWidth, y, 100, 20, hwnd, (HMENU)1004, GetModuleHandle(nullptr), nullptr);
            SendMessage(hRadioLeft, WM_SETFONT, (WPARAM)hFont, TRUE);
            ApplyFluentStyle(hRadioLeft);
            
            HWND hRadioPassed = CreateWindowW(L"BUTTON", L"显示已过",
                WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
                MODERN_MARGIN + 10 + labelWidth + 110, y, 100, 20, hwnd, (HMENU)1005, GetModuleHandle(nullptr), nullptr);
            SendMessage(hRadioPassed, WM_SETFONT, (WPARAM)hFont, TRUE);
            ApplyFluentStyle(hRadioPassed);
            
            // Set radio button state
            SendMessage(hRadioLeft, BM_SETCHECK, currentConfig.progressPercentMode == "left" ? BST_CHECKED : BST_UNCHECKED, 0);
            SendMessage(hRadioPassed, BM_SETCHECK, currentConfig.progressPercentMode == "passed" ? BST_CHECKED : BST_UNCHECKED, 0);
            y += 25 + MODERN_GROUP_SPACING;
            
            // Week offset section
            HWND hWeekTitle = CreateWindowW(L"STATIC", L"📅 周数设置",
                WS_VISIBLE | WS_CHILD | SS_LEFT,
                MODERN_MARGIN, y, 300, 25, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);
            SendMessage(hWeekTitle, WM_SETFONT, (WPARAM)hTitleFont, TRUE);
            y += MODERN_GROUP_SPACING + 10;
            
            HWND hWeekOffsetEnabled = CreateWindowW(L"BUTTON", L"启用自定义周�?,
                WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
                MODERN_MARGIN + 10, y, 200, 20, hwnd, (HMENU)1006, GetModuleHandle(nullptr), nullptr);
            SendMessage(hWeekOffsetEnabled, WM_SETFONT, (WPARAM)hFont, TRUE);
            ApplyFluentStyle(hWeekOffsetEnabled);
            SendMessage(hWeekOffsetEnabled, BM_SETCHECK, currentConfig.weekOffsetEnabled ? BST_CHECKED : BST_UNCHECKED, 0);
            y += 25 + MODERN_SPACING;
            
            CreateWindowW(L"STATIC", L"周数偏移�?",
                WS_VISIBLE | WS_CHILD | SS_LEFT,
                MODERN_MARGIN + 10, y, labelWidth, 20, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);
            HWND hWeekOffset = CreateWindowW(L"EDIT", std::to_wstring(currentConfig.weekOffset).c_str(),
                WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_NUMBER,
                MODERN_MARGIN + 10 + labelWidth, y, 80, controlHeight,
                hwnd, (HMENU)1007, GetModuleHandle(nullptr), nullptr);
            SendMessage(hWeekOffset, WM_SETFONT, (WPARAM)hFont, TRUE);
            ApplyFluentStyle(hWeekOffset);
            y += controlHeight + MODERN_GROUP_SPACING;
            
            // Notification section
            HWND hNotificationTitle = CreateWindowW(L"STATIC", L"🔔 提示音设�?,
                WS_VISIBLE | WS_CHILD | SS_LEFT,
                MODERN_MARGIN, y, 300, 25, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);
            SendMessage(hNotificationTitle, WM_SETFONT, (WPARAM)hTitleFont, TRUE);
            y += MODERN_GROUP_SPACING + 10;
            
            HWND hNotificationsEnabled = CreateWindowW(L"BUTTON", L"启用课堂提示�?,
                WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
                MODERN_MARGIN + 10, y, 200, 20, hwnd, (HMENU)1008, GetModuleHandle(nullptr), nullptr);
            SendMessage(hNotificationsEnabled, WM_SETFONT, (WPARAM)hFont, TRUE);
            ApplyFluentStyle(hNotificationsEnabled);
            SendMessage(hNotificationsEnabled, BM_SETCHECK, currentConfig.notificationsEnabled ? BST_CHECKED : BST_UNCHECKED, 0);
            y += 25 + MODERN_SPACING;
            
            CreateWindowW(L"STATIC", L"提示间隔(分钟):",
                WS_VISIBLE | WS_CHILD | SS_LEFT,
                MODERN_MARGIN + 10, y, labelWidth, 20, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);
            HWND hRegularInterval = CreateWindowW(L"EDIT", std::to_wstring(currentConfig.regularInterval).c_str(),
                WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_NUMBER,
                MODERN_MARGIN + 10 + labelWidth, y, 80, controlHeight,
                hwnd, (HMENU)1009, GetModuleHandle(nullptr), nullptr);
            SendMessage(hRegularInterval, WM_SETFONT, (WPARAM)hFont, TRUE);
            ApplyFluentStyle(hRegularInterval);
            y += controlHeight + MODERN_SPACING;
            
            CreateWindowW(L"STATIC", L"结束提示(分钟):",
                WS_VISIBLE | WS_CHILD | SS_LEFT,
                MODERN_MARGIN + 10, y, labelWidth, 20, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);
            HWND hEndingTime = CreateWindowW(L"EDIT", std::to_wstring(currentConfig.endingTime).c_str(),
                WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_NUMBER,
                MODERN_MARGIN + 10 + labelWidth, y, 80, controlHeight,
                hwnd, (HMENU)1010, GetModuleHandle(nullptr), nullptr);
            SendMessage(hEndingTime, WM_SETFONT, (WPARAM)hFont, TRUE);
            ApplyFluentStyle(hEndingTime);
            y += controlHeight + MODERN_GROUP_SPACING;
            
            // System settings section
            HWND hSystemTitle = CreateWindowW(L"STATIC", L"⚙️ 系统设置",
                WS_VISIBLE | WS_CHILD | SS_LEFT,
                MODERN_MARGIN, y, 300, 25, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);
            SendMessage(hSystemTitle, WM_SETFONT, (WPARAM)hTitleFont, TRUE);
            y += MODERN_GROUP_SPACING + 10;
            
            CreateWindowW(L"STATIC", L"壁纸页面URL:",
                WS_VISIBLE | WS_CHILD | SS_LEFT,
                MODERN_MARGIN + 10, y, labelWidth, 20, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);
            HWND hUrl = CreateWindowW(L"EDIT", utf8_to_utf16(currentConfig.url).c_str(),
                WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
                MODERN_MARGIN + 10 + labelWidth, y, controlWidth, controlHeight,
                hwnd, (HMENU)1011, GetModuleHandle(nullptr), nullptr);
            SendMessage(hUrl, WM_SETFONT, (WPARAM)hFont, TRUE);
            ApplyFluentStyle(hUrl);
            
            HWND hBrowseFile = CreateWindowW(L"BUTTON", L"📁 选择文件",
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                MODERN_MARGIN + 10 + labelWidth + controlWidth + 10, y, 100, controlHeight,
                hwnd, (HMENU)1013, GetModuleHandle(nullptr), nullptr);
            SendMessage(hBrowseFile, WM_SETFONT, (WPARAM)hFont, TRUE);
            ApplyFluentStyle(hBrowseFile);
            y += controlHeight + MODERN_SPACING;
            
            CreateWindowW(L"STATIC", L"浏览器路�?",
                WS_VISIBLE | WS_CHILD | SS_LEFT,
                MODERN_MARGIN + 10, y, labelWidth, 20, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);
            HWND hBrowserPath = CreateWindowW(L"EDIT", utf8_to_utf16(currentConfig.browserPath).c_str(),
                WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
                MODERN_MARGIN + 10 + labelWidth, y, controlWidth, controlHeight,
                hwnd, (HMENU)1012, GetModuleHandle(nullptr), nullptr);
            SendMessage(hBrowserPath, WM_SETFONT, (WPARAM)hFont, TRUE);
            ApplyFluentStyle(hBrowserPath);
            
            HWND hBrowseBrowser = CreateWindowW(L"BUTTON", L"🌐 选择浏览�?,
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                MODERN_MARGIN + 10 + labelWidth + controlWidth + 10, y, 100, controlHeight,
                hwnd, (HMENU)1014, GetModuleHandle(nullptr), nullptr);
            SendMessage(hBrowseBrowser, WM_SETFONT, (WPARAM)hFont, TRUE);
            ApplyFluentStyle(hBrowseBrowser);
            y += controlHeight + MODERN_SPACING;
            
            // Test button
            HWND hTestBrowser = CreateWindowW(L"BUTTON", L"🚀 在浏览器中测�?,
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                MODERN_MARGIN + 10, y, 150, MODERN_BUTTON_HEIGHT,
                hwnd, (HMENU)1015, GetModuleHandle(nullptr), nullptr);
            SendMessage(hTestBrowser, WM_SETFONT, (WPARAM)hFont, TRUE);
            ApplyFluentStyle(hTestBrowser);
            y += MODERN_BUTTON_HEIGHT + MODERN_GROUP_SPACING;
            
            // Bottom buttons
            HWND hSaveBtn = CreateWindowW(L"BUTTON", L"💾 保存设置",
                WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                450, y, 100, MODERN_BUTTON_HEIGHT,
                hwnd, (HMENU)IDOK, GetModuleHandle(nullptr), nullptr);
            SendMessage(hSaveBtn, WM_SETFONT, (WPARAM)hFont, TRUE);
            ApplyFluentStyle(hSaveBtn);
            
            HWND hCancelBtn = CreateWindowW(L"BUTTON", L"�?取消",
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                560, y, 80, MODERN_BUTTON_HEIGHT,
                hwnd, (HMENU)IDCANCEL, GetModuleHandle(nullptr), nullptr);
            SendMessage(hCancelBtn, WM_SETFONT, (WPARAM)hFont, TRUE);
            ApplyFluentStyle(hCancelBtn);
            
            return 0;
        }
        
    case WM_CTLCOLORSTATIC:
        {
            HDC hdc = (HDC)wParam;
            SetBkColor(hdc, MODERN_BG_COLOR);
            SetTextColor(hdc, MODERN_COLOR_TEXT);
            return (LRESULT)hBackgroundBrush;
        }
        
    case WM_CTLCOLOREDIT:
        {
            HDC hdc = (HDC)wParam;
            SetBkColor(hdc, RGB(255, 255, 255));
            SetTextColor(hdc, MODERN_COLOR_TEXT);
            return (LRESULT)GetStockObject(WHITE_BRUSH);
        }
        
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case 1013: // Browse File
            {
                std::string file = SelectFile(hwnd);
                if (!file.empty()) {
                    SetWindowTextW(GetDlgItem(hwnd, 1011), utf8_to_utf16(file).c_str());
                }
                break;
            }
        case 1014: // Browse Browser
            {
                std::string browser = SelectBrowser(hwnd);
                if (!browser.empty()) {
                    SetWindowTextW(GetDlgItem(hwnd, 1012), utf8_to_utf16(browser).c_str());
                }
                break;
            }
        case 1015: // Test in Browser
            {
                wchar_t url_buffer[512];
                GetWindowTextW(GetDlgItem(hwnd, 1011), url_buffer, 512);
                if (wcslen(url_buffer) > 0) {
                    OpenInBrowser(utf16_to_utf8(url_buffer));
                } else {
                    MessageBoxW(hwnd, L"请先输入URL", L"错误", MB_OK | MB_ICONERROR);
                }
                break;
            }
        case IDOK: // Save
            {
                // Get values from controls using Unicode functions
                wchar_t buffer[512];
                
                GetWindowTextW(GetDlgItem(hwnd, 1001), buffer, 512);
                currentConfig.progressDescription = utf16_to_utf8(buffer);
                
                GetWindowTextW(GetDlgItem(hwnd, 1002), buffer, 512);
                currentConfig.semesterBegin = utf16_to_utf8(buffer);
                
                GetWindowTextW(GetDlgItem(hwnd, 1003), buffer, 512);
                currentConfig.semesterEnd = utf16_to_utf8(buffer);
                
                currentConfig.progressPercentMode = SendMessage(GetDlgItem(hwnd, 1004), BM_GETCHECK, 0, 0) ? "left" : "passed";
                currentConfig.weekOffsetEnabled = SendMessage(GetDlgItem(hwnd, 1006), BM_GETCHECK, 0, 0) == BST_CHECKED;
                currentConfig.weekOffset = GetDlgItemInt(hwnd, 1007, nullptr, FALSE);
                currentConfig.notificationsEnabled = SendMessage(GetDlgItem(hwnd, 1008), BM_GETCHECK, 0, 0) == BST_CHECKED;
                currentConfig.regularInterval = GetDlgItemInt(hwnd, 1009, nullptr, FALSE);
                currentConfig.endingTime = GetDlgItemInt(hwnd, 1010, nullptr, FALSE);
                
                GetWindowTextW(GetDlgItem(hwnd, 1011), buffer, 512);
                currentConfig.url = utf16_to_utf8(buffer);
                
                GetWindowTextW(GetDlgItem(hwnd, 1012), buffer, 512);
                currentConfig.browserPath = utf16_to_utf8(buffer);
                
                // Save configuration
                if (SaveConfig()) {
                    MessageBoxW(hwnd, L"设置保存成功�?, L"成功", MB_OK | MB_ICONINFORMATION);
                } else {
                    MessageBoxW(hwnd, L"设置保存失败�?, L"错误", MB_OK | MB_ICONERROR);
                }
                
                DestroyWindow(hwnd);
                break;
            }
        case IDCANCEL:
            DestroyWindow(hwnd);
            break;
        }
        break;
        
    case WM_CLOSE:
        if (hFont) DeleteObject(hFont);
        if (hTitleFont) DeleteObject(hTitleFont);
        if (hBackgroundBrush) DeleteObject(hBackgroundBrush);
        DestroyWindow(hwnd);
        break;
        
    default:
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
    return 0;
}

// Show Modern Basic Settings Dialog
void ShowBasicSettingsDialog() {
    // Register window class for basic settings dialog
    static bool classRegistered = false;
    if (!classRegistered) {
        WNDCLASSEXW wc = {};
        wc.cbSize = sizeof(WNDCLASSEXW);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = ModernBasicSettingsProc;
        wc.hInstance = GetModuleHandle(nullptr);
        wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = CreateSolidBrush(MODERN_BG_COLOR);
        wc.lpszClassName = L"ModernBasicSettingsDialog";
        wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
        
        if (RegisterClassExW(&wc)) {
            classRegistered = true;
        }
    }
    
    // Create modern dialog window
    HWND hwnd = CreateWindowExW(
        WS_EX_DLGMODALFRAME | WS_EX_TOPMOST,
        L"ModernBasicSettingsDialog",
        L"ClassPaper 基本设置",
        WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 680, 720,
        nullptr, nullptr, GetModuleHandle(nullptr), nullptr
    );
    
    if (!hwnd) {
        MessageBoxW(nullptr, L"无法创建设置窗口", L"错误", MB_OK | MB_ICONERROR);
        return;
    }
    
    // Center the dialog on screen
    RECT rect;
    GetWindowRect(hwnd, &rect);
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    SetWindowPos(hwnd, nullptr, 
        (screenWidth - width) / 2, (screenHeight - height) / 2,
        0, 0, SWP_NOSIZE | SWP_NOZORDER);
    
    // Message loop for this dialog
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (msg.hwnd == hwnd || IsChild(hwnd, msg.hwnd)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            
            // Check if our window is destroyed
            if (!IsWindow(hwnd)) {
                break;
            }
        } else {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}

// Show Schedule Settings Dialog
void ShowScheduleSettingsDialog() {
    // Create a modal dialog window for schedule settings
    HWND hwnd = CreateWindowExA(
        WS_EX_DLGMODALFRAME | WS_EX_TOPMOST,
        "STATIC",
        "ClassPaper 课程表设�?,
        WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        nullptr, nullptr, GetModuleHandle(nullptr), nullptr
    );
    
    if (!hwnd) {
        MessageBoxA(nullptr, "无法创建课程表设置窗�?, "错误", MB_OK | MB_ICONERROR);
        return;
    }
    
    // Create controls
    int y = 20;
    const int spacing = 30;
    
    // Display mode section
    CreateWindowA("STATIC", "课表显示模式", WS_VISIBLE | WS_CHILD | SS_LEFT,
                  20, y, 200, 20, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);
    y += 25;
    
    HWND hRadioScroll = CreateWindowA("BUTTON", "滚动模式（当前课程居中）", 
                                      WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | WS_GROUP,
                                      30, y, 250, 20, hwnd, (HMENU)2001, GetModuleHandle(nullptr), nullptr);
    y += 25;
    
    HWND hRadioDay = CreateWindowA("BUTTON", "一天进度模式（只显示当天）", 
                                   WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
                                   30, y, 250, 20, hwnd, (HMENU)2002, GetModuleHandle(nullptr), nullptr);
    
    // Set radio button state
    SendMessage(hRadioScroll, BM_SETCHECK, currentConfig.lessons.displayMode == "scroll" ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessage(hRadioDay, BM_SETCHECK, currentConfig.lessons.displayMode == "day" ? BST_CHECKED : BST_UNCHECKED, 0);
    y += spacing + 10;
    
    // Time table section
    CreateWindowA("STATIC", "课程时间安排", WS_VISIBLE | WS_CHILD | SS_LEFT,
                  20, y, 200, 20, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);
    y += 25;
    
    // Add time slot button
    HWND hAddTimeSlot = CreateWindowA("BUTTON", "添加时间�?, WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                                      30, y, 100, 25, hwnd, (HMENU)2003, GetModuleHandle(nullptr), nullptr);
    
    // Smart guess button
    HWND hSmartGuess = CreateWindowA("BUTTON", "智能推算", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                                     140, y, 100, 25, hwnd, (HMENU)2004, GetModuleHandle(nullptr), nullptr);
    y += 35;
    
    // Time table list (simplified - in a real implementation, you'd use a ListView)
    CreateWindowA("STATIC", "时间表列�?", WS_VISIBLE | WS_CHILD | SS_LEFT,
                  30, y, 100, 20, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);
    y += 25;
    
    HWND hTimeList = CreateWindowA("LISTBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL,
                                   30, y, 400, 150, hwnd, (HMENU)2005, GetModuleHandle(nullptr), nullptr);
    
    // Populate time list with current schedule
    for (const auto& timeSlot : currentConfig.lessons.times.schedule) {
        std::string timeStr = "�? + std::to_string(timeSlot.period) + "�? " + 
                             timeSlot.begin + "-" + timeSlot.end;
        if (!timeSlot.rest.empty()) {
            timeStr += " (休息: " + timeSlot.rest + ")";
        }
        SendMessageA(hTimeList, LB_ADDSTRING, 0, (LPARAM)timeStr.c_str());
    }
    y += 160;
    
    // Course schedule section
    CreateWindowA("STATIC", "课程安排", WS_VISIBLE | WS_CHILD | SS_LEFT,
                  20, y, 200, 20, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);
    y += 25;
    
    // Import schedule button
    HWND hImportSchedule = CreateWindowA("BUTTON", "导入课程�?, WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                                         30, y, 100, 25, hwnd, (HMENU)2006, GetModuleHandle(nullptr), nullptr);
    y += 35;
    
    // Schedule display (simplified)
    CreateWindowA("STATIC", "当前课程�?", WS_VISIBLE | WS_CHILD | SS_LEFT,
                  30, y, 100, 20, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);
    y += 25;
    
    HWND hScheduleList = CreateWindowA("LISTBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL,
                                       30, y, 500, 120, hwnd, (HMENU)2007, GetModuleHandle(nullptr), nullptr);
    
    // Populate schedule list
    for (const auto& day : currentConfig.lessons.schedule) {
        std::string dayStr = day.day + ": ";
        for (size_t i = 0; i < day.classes.size() && i < 5; ++i) { // Show first 5 classes
            if (i > 0) dayStr += ", ";
            dayStr += day.classes[i];
        }
        if (day.classes.size() > 5) {
            dayStr += "...";
        }
        SendMessageA(hScheduleList, LB_ADDSTRING, 0, (LPARAM)dayStr.c_str());
    }
    y += 130;
    
    // Bottom buttons
    HWND hSaveBtn = CreateWindowA("BUTTON", "保存", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                                  600, y, 80, 30, hwnd, (HMENU)IDOK, GetModuleHandle(nullptr), nullptr);
    HWND hCancelBtn = CreateWindowA("BUTTON", "取消", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                                    690, y, 80, 30, hwnd, (HMENU)IDCANCEL, GetModuleHandle(nullptr), nullptr);
    
    // Set up window procedure
    SetWindowLongPtrA(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(static_cast<WNDPROC>([](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) -> LRESULT WINAPI {
        switch (msg) {
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
            case 2003: // Add time slot
                {
                    // Simple dialog to add time slot
                    std::string timeSlot = "�? + std::to_string(currentConfig.lessons.times.schedule.size() + 1) + 
                                          "�? 08:00-08:40 (休息: 08:40-08:50)";
                    SendMessageA(GetDlgItem(hwnd, 2005), LB_ADDSTRING, 0, (LPARAM)timeSlot.c_str());
                    
                    // Add to actual config (simplified)
                    currentConfig.lessons.times.schedule.push_back({
                        (int)currentConfig.lessons.times.schedule.size() + 1,
                        "08:00", "08:40", "08:40-08:50"
                    });
                    break;
                }
            case 2004: // Smart guess
                {
                    MessageBoxA(hwnd, "智能推算功能：根据已有时间自动填充后续时间段", "智能推算", MB_OK | MB_ICONINFORMATION);
                    break;
                }
            case 2006: // Import schedule
                {
                    MessageBoxA(hwnd, "导入课程表功能：支持CSV格式导入", "导入课程�?, MB_OK | MB_ICONINFORMATION);
                    break;
                }
            case IDOK: // Save
                {
                    // Get display mode
                    currentConfig.lessons.displayMode = SendMessage(GetDlgItem(hwnd, 2001), BM_GETCHECK, 0, 0) ? "scroll" : "day";
                    
                    // Save configuration
                    if (SaveConfig()) {
                        MessageBoxA(hwnd, "课程表设置保存成�?, "成功", MB_OK | MB_ICONINFORMATION);
                    } else {
                        MessageBoxA(hwnd, "课程表设置保存失�?, "错误", MB_OK | MB_ICONERROR);
                    }
                    
                    DestroyWindow(hwnd);
                    break;
                }
            case IDCANCEL:
                DestroyWindow(hwnd);
                break;
            }
            break;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        default:
            return DefWindowProcA(hwnd, msg, wParam, lParam);
        }
        return 0;
    })));
    
    // Message loop for this dialog
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (msg.hwnd == hwnd || IsChild(hwnd, msg.hwnd)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            
            if (!IsWindow(hwnd)) {
                break;
            }
        } else {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}

// Show Events Settings Dialog
void ShowEventsSettingsDialog() {
    // Create a modal dialog window for events settings
    HWND hwnd = CreateWindowExA(
        WS_EX_DLGMODALFRAME | WS_EX_TOPMOST,
        "STATIC",
        "ClassPaper 事件设置",
        WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 600, 500,
        nullptr, nullptr, GetModuleHandle(nullptr), nullptr
    );
    
    if (!hwnd) {
        MessageBoxA(nullptr, "无法创建事件设置窗口", "错误", MB_OK | MB_ICONERROR);
        return;
    }
    
    // Create controls
    int y = 20;
    const int spacing = 30;
    
    // Events list section
    CreateWindowA("STATIC", "事件列表", WS_VISIBLE | WS_CHILD | SS_LEFT,
                  20, y, 200, 20, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);
    y += 25;
    
    // Events list
    HWND hEventsList = CreateWindowA("LISTBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL,
                                     20, y, 500, 200, hwnd, (HMENU)3001, GetModuleHandle(nullptr), nullptr);
    
    // Populate events list
    for (size_t i = 0; i < currentConfig.events.size(); ++i) {
        const auto& event = currentConfig.events[i];
        std::string eventStr = event.name + " - " + event.date.substr(0, 10); // Show date part only
        SendMessageA(hEventsList, LB_ADDSTRING, 0, (LPARAM)eventStr.c_str());
    }
    y += 210;
    
    // Add new event section
    CreateWindowA("STATIC", "添加新事�?, WS_VISIBLE | WS_CHILD | SS_LEFT,
                  20, y, 200, 20, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);
    y += 25;
    
    CreateWindowA("STATIC", "事件名称:", WS_VISIBLE | WS_CHILD | SS_LEFT,
                  30, y, 80, 20, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);
    HWND hEventName = CreateWindowA("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
                                    120, y, 200, 25, hwnd, (HMENU)3002, GetModuleHandle(nullptr), nullptr);
    y += 35;
    
    CreateWindowA("STATIC", "事件日期:", WS_VISIBLE | WS_CHILD | SS_LEFT,
                  30, y, 80, 20, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);
    HWND hEventDate = CreateWindowA("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
                                    120, y, 120, 25, hwnd, (HMENU)3003, GetModuleHandle(nullptr), nullptr);
    CreateWindowA("STATIC", "(YYYY-MM-DD)", WS_VISIBLE | WS_CHILD | SS_LEFT,
                  250, y, 100, 20, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);
    y += 35;
    
    CreateWindowA("STATIC", "事件时间:", WS_VISIBLE | WS_CHILD | SS_LEFT,
                  30, y, 80, 20, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);
    HWND hEventTime = CreateWindowA("EDIT", "00:00", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
                                    120, y, 80, 25, hwnd, (HMENU)3004, GetModuleHandle(nullptr), nullptr);
    CreateWindowA("STATIC", "(HH:MM)", WS_VISIBLE | WS_CHILD | SS_LEFT,
                  210, y, 100, 20, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);
    y += 35;
    
    // Buttons
    HWND hAddEvent = CreateWindowA("BUTTON", "添加事件", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                                   30, y, 80, 30, hwnd, (HMENU)3005, GetModuleHandle(nullptr), nullptr);
    HWND hDeleteEvent = CreateWindowA("BUTTON", "删除选中", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                                      120, y, 80, 30, hwnd, (HMENU)3006, GetModuleHandle(nullptr), nullptr);
    y += 40;
    
    // Bottom buttons
    HWND hSaveBtn = CreateWindowA("BUTTON", "保存", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                                  400, y, 80, 30, hwnd, (HMENU)IDOK, GetModuleHandle(nullptr), nullptr);
    HWND hCancelBtn = CreateWindowA("BUTTON", "取消", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                                    490, y, 80, 30, hwnd, (HMENU)IDCANCEL, GetModuleHandle(nullptr), nullptr);
    
    // Set up window procedure
    SetWindowLongPtrA(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(static_cast<WNDPROC>([](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) -> LRESULT WINAPI {
        switch (msg) {
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
            case 3005: // Add event
                {
                    char name[256], date[32], time[16];
                    GetDlgItemTextA(hwnd, 3002, name, sizeof(name));
                    GetDlgItemTextA(hwnd, 3003, date, sizeof(date));
                    GetDlgItemTextA(hwnd, 3004, time, sizeof(time));
                    
                    if (strlen(name) > 0 && strlen(date) > 0) {
                        // Create ISO datetime string
                        std::string datetime = std::string(date) + "T" + std::string(time) + ":00";
                        
                        // Add to config
                        currentConfig.events.push_back({std::string(name), datetime});
                        
                        // Add to list
                        std::string eventStr = std::string(name) + " - " + std::string(date);
                        SendMessageA(GetDlgItem(hwnd, 3001), LB_ADDSTRING, 0, (LPARAM)eventStr.c_str());
                        
                        // Clear input fields
                        SetDlgItemTextA(hwnd, 3002, "");
                        SetDlgItemTextA(hwnd, 3003, "");
                        SetDlgItemTextA(hwnd, 3004, "00:00");
                        
                        MessageBoxA(hwnd, "事件添加成功", "成功", MB_OK | MB_ICONINFORMATION);
                    } else {
                        MessageBoxA(hwnd, "请输入事件名称和日期", "错误", MB_OK | MB_ICONERROR);
                    }
                    break;
                }
            case 3006: // Delete event
                {
                    HWND hList = GetDlgItem(hwnd, 3001);
                    int selected = SendMessage(hList, LB_GETCURSEL, 0, 0);
                    if (selected != LB_ERR && selected < (int)currentConfig.events.size()) {
                        // Remove from config
                        currentConfig.events.erase(currentConfig.events.begin() + selected);
                        
                        // Remove from list
                        SendMessage(hList, LB_DELETESTRING, selected, 0);
                        
                        MessageBoxA(hwnd, "事件删除成功", "成功", MB_OK | MB_ICONINFORMATION);
                    } else {
                        MessageBoxA(hwnd, "请选择要删除的事件", "错误", MB_OK | MB_ICONERROR);
                    }
                    break;
                }
            case IDOK: // Save
                {
                    // Save configuration
                    if (SaveConfig()) {
                        MessageBoxA(hwnd, "事件设置保存成功", "成功", MB_OK | MB_ICONINFORMATION);
                    } else {
                        MessageBoxA(hwnd, "事件设置保存失败", "错误", MB_OK | MB_ICONERROR);
                    }
                    
                    DestroyWindow(hwnd);
                    break;
                }
            case IDCANCEL:
                DestroyWindow(hwnd);
                break;
            }
            break;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        default:
            return DefWindowProcA(hwnd, msg, wParam, lParam);
        }
        return 0;
    })));
    
    // Message loop for this dialog
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (msg.hwnd == hwnd || IsChild(hwnd, msg.hwnd)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            
            if (!IsWindow(hwnd)) {
                break;
            }
        } else {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}

// Show Wallpapers Settings Dialog
void ShowWallpapersSettingsDialog() {
    // Create a modal dialog window for wallpapers settings
    HWND hwnd = CreateWindowExA(
        WS_EX_DLGMODALFRAME | WS_EX_TOPMOST,
        "STATIC",
        "ClassPaper 壁纸设置",
        WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 700, 600,
        nullptr, nullptr, GetModuleHandle(nullptr), nullptr
    );
    
    if (!hwnd) {
        MessageBoxA(nullptr, "无法创建壁纸设置窗口", "错误", MB_OK | MB_ICONERROR);
        return;
    }
    
    // Create controls
    int y = 20;
    const int spacing = 30;
    
    // Wallpaper interval section
    CreateWindowA("STATIC", "壁纸切换设置", WS_VISIBLE | WS_CHILD | SS_LEFT,
                  20, y, 200, 20, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);
    y += 25;
    
    CreateWindowA("STATIC", "切换间隔(�?:", WS_VISIBLE | WS_CHILD | SS_LEFT,
                  30, y, 100, 20, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);
    HWND hInterval = CreateWindowA("EDIT", std::to_string(currentConfig.wallpaperInterval).c_str(),
                                   WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_NUMBER,
                                   140, y, 80, 25, hwnd, (HMENU)4001, GetModuleHandle(nullptr), nullptr);
    CreateWindowA("STATIC", "(建议不小�?�?", WS_VISIBLE | WS_CHILD | SS_LEFT,
                  230, y, 150, 20, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);
    y += spacing + 10;
    
    // Wallpapers list section
    CreateWindowA("STATIC", "壁纸列表", WS_VISIBLE | WS_CHILD | SS_LEFT,
                  20, y, 200, 20, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);
    y += 25;
    
    // Wallpapers list
    HWND hWallpapersList = CreateWindowA("LISTBOX", "", WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL,
                                         20, y, 600, 250, hwnd, (HMENU)4002, GetModuleHandle(nullptr), nullptr);
    
    // Populate wallpapers list
    for (size_t i = 0; i < currentConfig.wallpapers.size(); ++i) {
        const auto& wallpaper = currentConfig.wallpapers[i];
        SendMessageA(hWallpapersList, LB_ADDSTRING, 0, (LPARAM)wallpaper.c_str());
    }
    y += 260;
    
    // Add new wallpaper section
    CreateWindowA("STATIC", "添加壁纸", WS_VISIBLE | WS_CHILD | SS_LEFT,
                  20, y, 200, 20, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);
    y += 25;
    
    CreateWindowA("STATIC", "壁纸路径:", WS_VISIBLE | WS_CHILD | SS_LEFT,
                  30, y, 80, 20, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);
    HWND hWallpaperPath = CreateWindowA("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
                                        120, y, 300, 25, hwnd, (HMENU)4003, GetModuleHandle(nullptr), nullptr);
    HWND hBrowseWallpaper = CreateWindowA("BUTTON", "浏览", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                                          430, y, 60, 25, hwnd, (HMENU)4004, GetModuleHandle(nullptr), nullptr);
    y += 35;
    
    // Buttons
    HWND hAddWallpaper = CreateWindowA("BUTTON", "添加壁纸", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                                       30, y, 80, 30, hwnd, (HMENU)4005, GetModuleHandle(nullptr), nullptr);
    HWND hDeleteWallpaper = CreateWindowA("BUTTON", "删除选中", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                                          120, y, 80, 30, hwnd, (HMENU)4006, GetModuleHandle(nullptr), nullptr);
    HWND hScanWallpapers = CreateWindowA("BUTTON", "扫描文件�?, WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                                         210, y, 100, 30, hwnd, (HMENU)4007, GetModuleHandle(nullptr), nullptr);
    y += 40;
    
    // Bottom buttons
    HWND hSaveBtn = CreateWindowA("BUTTON", "保存", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                                  500, y, 80, 30, hwnd, (HMENU)IDOK, GetModuleHandle(nullptr), nullptr);
    HWND hCancelBtn = CreateWindowA("BUTTON", "取消", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                                    590, y, 80, 30, hwnd, (HMENU)IDCANCEL, GetModuleHandle(nullptr), nullptr);
    
    // Set up window procedure
    SetWindowLongPtrA(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(static_cast<WNDPROC>([](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) -> LRESULT WINAPI {
        switch (msg) {
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
            case 4004: // Browse wallpaper
                {
                    std::string file = SelectFile(hwnd, "Image Files\0*.jpg;*.jpeg;*.png;*.bmp;*.gif\0All Files\0*.*\0");
                    if (!file.empty()) {
                        SetDlgItemTextA(hwnd, 4003, file.c_str());
                    }
                    break;
                }
            case 4005: // Add wallpaper
                {
                    char path[512];
                    GetDlgItemTextA(hwnd, 4003, path, sizeof(path));
                    
                    if (strlen(path) > 0) {
                        // Add to config
                        currentConfig.wallpapers.push_back(std::string(path));
                        
                        // Add to list
                        SendMessageA(GetDlgItem(hwnd, 4002), LB_ADDSTRING, 0, (LPARAM)path);
                        
                        // Clear input field
                        SetDlgItemTextA(hwnd, 4003, "");
                        
                        MessageBoxA(hwnd, "壁纸添加成功", "成功", MB_OK | MB_ICONINFORMATION);
                    } else {
                        MessageBoxA(hwnd, "请输入壁纸路�?, "错误", MB_OK | MB_ICONERROR);
                    }
                    break;
                }
            case 4006: // Delete wallpaper
                {
                    HWND hList = GetDlgItem(hwnd, 4002);
                    int selected = SendMessage(hList, LB_GETCURSEL, 0, 0);
                    if (selected != LB_ERR && selected < (int)currentConfig.wallpapers.size()) {
                        // Remove from config
                        currentConfig.wallpapers.erase(currentConfig.wallpapers.begin() + selected);
                        
                        // Remove from list
                        SendMessage(hList, LB_DELETESTRING, selected, 0);
                        
                        MessageBoxA(hwnd, "壁纸删除成功", "成功", MB_OK | MB_ICONINFORMATION);
                    } else {
                        MessageBoxA(hwnd, "请选择要删除的壁纸", "错误", MB_OK | MB_ICONERROR);
                    }
                    break;
                }
            case 4007: // Scan wallpapers folder
                {
                    // Simple implementation - scan wallpaper folder
                    std::string wallpaperDir = "wallpaper";
                    if (std::filesystem::exists(wallpaperDir)) {
                        int addedCount = 0;
                        for (const auto& entry : std::filesystem::directory_iterator(wallpaperDir)) {
                            if (entry.is_regular_file()) {
                                std::string ext = entry.path().extension().string();
                                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                                
                                if (ext == ".jpg" || ext == ".jpeg" || ext == ".png" || 
                                    ext == ".bmp" || ext == ".gif") {
                                    std::string relativePath = entry.path().string();
                                    std::replace(relativePath.begin(), relativePath.end(), '\\', '/');
                                    
                                    // Check if already exists
                                    bool exists = false;
                                    for (const auto& existing : currentConfig.wallpapers) {
                                        if (existing == relativePath) {
                                            exists = true;
                                            break;
                                        }
                                    }
                                    
                                    if (!exists) {
                                        currentConfig.wallpapers.push_back(relativePath);
                                        SendMessageA(GetDlgItem(hwnd, 4002), LB_ADDSTRING, 0, (LPARAM)relativePath.c_str());
                                        addedCount++;
                                    }
                                }
                            }
                        }
                        
                        std::string message = "扫描完成，添加了 " + std::to_string(addedCount) + " 个壁�?;
                        MessageBoxA(hwnd, message.c_str(), "扫描结果", MB_OK | MB_ICONINFORMATION);
                    } else {
                        MessageBoxA(hwnd, "wallpaper 文件夹不存在", "错误", MB_OK | MB_ICONERROR);
                    }
                    break;
                }
            case IDOK: // Save
                {
                    // Get wallpaper interval
                    currentConfig.wallpaperInterval = GetDlgItemInt(hwnd, 4001, nullptr, FALSE);
                    if (currentConfig.wallpaperInterval < 5) {
                        currentConfig.wallpaperInterval = 5; // Minimum 5 seconds
                    }
                    
                    // Save configuration
                    if (SaveConfig()) {
                        MessageBoxA(hwnd, "壁纸设置保存成功", "成功", MB_OK | MB_ICONINFORMATION);
                    } else {
                        MessageBoxA(hwnd, "壁纸设置保存失败", "错误", MB_OK | MB_ICONERROR);
                    }
                    
                    DestroyWindow(hwnd);
                    break;
                }
            case IDCANCEL:
                DestroyWindow(hwnd);
                break;
            }
            break;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        default:
            return DefWindowProcA(hwnd, msg, wParam, lParam);
        }
        return 0;
    })));
    
    // Message loop for this dialog
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (msg.hwnd == hwnd || IsChild(hwnd, msg.hwnd)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            
            if (!IsWindow(hwnd)) {
                break;
            }
        } else {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}

// Show Notice Settings Dialog
void ShowNoticeSettingsDialog() {
    // Create a modal dialog window for notice settings
    HWND hwnd = CreateWindowExA(
        WS_EX_DLGMODALFRAME | WS_EX_TOPMOST,
        "STATIC",
        "ClassPaper 告示牌设�?,
        WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        nullptr, nullptr, GetModuleHandle(nullptr), nullptr
    );
    
    if (!hwnd) {
        MessageBoxA(nullptr, "无法创建告示牌设置窗�?, "错误", MB_OK | MB_ICONERROR);
        return;
    }
    
    // Create controls
    int y = 20;
    
    // Notice content section
    CreateWindowA("STATIC", "告示内容 (支持HTML)", WS_VISIBLE | WS_CHILD | SS_LEFT,
                  20, y, 200, 20, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);
    y += 25;
    
    // Notice content editor
    HWND hNoticeContent = CreateWindowExA(
        WS_EX_CLIENTEDGE,
        "EDIT",
        currentConfig.sth.c_str(),
        WS_VISIBLE | WS_CHILD | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_WANTRETURN,
        20, y, 720, 300,
        hwnd, (HMENU)5001, GetModuleHandle(nullptr), nullptr
    );
    y += 310;
    
    // Preview section
    CreateWindowA("STATIC", "预览 (简化显�?", WS_VISIBLE | WS_CHILD | SS_LEFT,
                  20, y, 200, 20, hwnd, nullptr, GetModuleHandle(nullptr), nullptr);
    y += 25;
    
    // Preview area (read-only edit control)
    HWND hPreview = CreateWindowExA(
        WS_EX_CLIENTEDGE,
        "EDIT",
        currentConfig.sth.c_str(),
        WS_VISIBLE | WS_CHILD | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
        20, y, 720, 150,
        hwnd, (HMENU)5002, GetModuleHandle(nullptr), nullptr
    );
    y += 160;
    
    // Update preview button
    HWND hUpdatePreview = CreateWindowA("BUTTON", "更新预览", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                                        20, y, 100, 30, hwnd, (HMENU)5003, GetModuleHandle(nullptr), nullptr);
    y += 40;
    
    // Bottom buttons
    HWND hSaveBtn = CreateWindowA("BUTTON", "保存", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                                  600, y, 80, 30, hwnd, (HMENU)IDOK, GetModuleHandle(nullptr), nullptr);
    HWND hCancelBtn = CreateWindowA("BUTTON", "取消", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                                    690, y, 80, 30, hwnd, (HMENU)IDCANCEL, GetModuleHandle(nullptr), nullptr);
    
    // Set up window procedure
    SetWindowLongPtrA(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(static_cast<WNDPROC>([](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) -> LRESULT WINAPI {
        switch (msg) {
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
            case 5003: // Update preview
                {
                    // Get content from editor
                    int length = GetWindowTextLengthA(GetDlgItem(hwnd, 5001));
                    if (length > 0) {
                        std::vector<char> buffer(length + 1);
                        GetDlgItemTextA(hwnd, 5001, buffer.data(), buffer.size());
                        
                        // Simple HTML tag removal for preview (basic implementation)
                        std::string content = buffer.data();
                        std::string preview = content;
                        
                        // Remove common HTML tags for preview
                        size_t pos = 0;
                        while ((pos = preview.find('<', pos)) != std::string::npos) {
                            size_t endPos = preview.find('>', pos);
                            if (endPos != std::string::npos) {
                                preview.erase(pos, endPos - pos + 1);
                            } else {
                                break;
                            }
                        }
                        
                        // Replace HTML entities
                        size_t entityPos = 0;
                        while ((entityPos = preview.find("&lt;", entityPos)) != std::string::npos) {
                            preview.replace(entityPos, 4, "<");
                            entityPos += 1;
                        }
                        while ((entityPos = preview.find("&gt;", entityPos)) != std::string::npos) {
                            preview.replace(entityPos, 4, ">");
                            entityPos += 1;
                        }
                        while ((entityPos = preview.find("&amp;", entityPos)) != std::string::npos) {
                            preview.replace(entityPos, 5, "&");
                            entityPos += 1;
                        }
                        
                        // Update preview
                        SetDlgItemTextA(hwnd, 5002, preview.c_str());
                    }
                    break;
                }
            case IDOK: // Save
                {
                    // Get content from editor
                    int length = GetWindowTextLengthA(GetDlgItem(hwnd, 5001));
                    if (length > 0) {
                        std::vector<char> buffer(length + 1);
                        GetDlgItemTextA(hwnd, 5001, buffer.data(), buffer.size());
                        currentConfig.sth = buffer.data();
                    } else {
                        currentConfig.sth = "";
                    }
                    
                    // Save configuration
                    if (SaveConfig()) {
                        MessageBoxA(hwnd, "告示牌设置保存成�?, "成功", MB_OK | MB_ICONINFORMATION);
                    } else {
                        MessageBoxA(hwnd, "告示牌设置保存失�?, "错误", MB_OK | MB_ICONERROR);
                    }
                    
                    DestroyWindow(hwnd);
                    break;
                }
            case IDCANCEL:
                DestroyWindow(hwnd);
                break;
            }
            break;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        default:
            return DefWindowProcA(hwnd, msg, wParam, lParam);
        }
        return 0;
    })));
    
    // Message loop for this dialog
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (msg.hwnd == hwnd || IsChild(hwnd, msg.hwnd)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            
            if (!IsWindow(hwnd)) {
                break;
            }
        } else {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}

// Modern Fluent Design main window procedure
LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HFONT hFont = nullptr;
    static HFONT hTitleFont = nullptr;
    static HBRUSH hBackgroundBrush = nullptr;
    static std::map<HWND, bool> buttonHoverStates;
    static std::map<HWND, bool> buttonPressStates;
    
    switch (msg) {
    case WM_CREATE:
        {
            // Initialize Fluent Design UI
            hFont = CreateFluentFont(10, false);
            hTitleFont = CreateFluentFont(24, true);
            hBackgroundBrush = CreateSolidBrush(FLUENT_BG_COLOR);
            
            // Create modern Fluent Design main window controls
            int y = 40;
            const int cardWidth = 520;
            const int cardHeight = 360;
            const int buttonWidth = 240;
            const int buttonHeight = 48;
            const int centerX = 300; // Assuming window width of 600
            
            // Main title with modern typography
            HWND hTitle = CreateWindowW(L"STATIC", L"ClassPaper",
                WS_VISIBLE | WS_CHILD | SS_CENTER,
                MODERN_MARGIN, y, 560, 40,
                hwnd, nullptr, GetModuleHandle(nullptr), nullptr);
            SendMessage(hTitle, WM_SETFONT, (WPARAM)hTitleFont, TRUE);
            y += 50;
            
            // Subtitle with secondary text color
            HWND hSubtitle = CreateWindowW(L"STATIC", L"现代化的课程管理设置工具",
                WS_VISIBLE | WS_CHILD | SS_CENTER,
                MODERN_MARGIN, y, 560, 24,
                hwnd, nullptr, GetModuleHandle(nullptr), nullptr);
            SendMessage(hSubtitle, WM_SETFONT, (WPARAM)CreateFluentFont(12, false), TRUE);
            y += 40;
            
            // Create modern Fluent Design card-style buttons
            struct FluentButtonInfo {
                const wchar_t* icon;
                const wchar_t* title;
                const wchar_t* description;
                int id;
                bool isAccent;
            };
            
            FluentButtonInfo buttons[] = {
                {L"⚙️", L"基本设置", L"进度条、周数、提示音、系统设�?, 1001, false},
                {L"📅", L"课程表设�?, L"时间安排、显示模式、课程管�?, 1002, false},
                {L"📝", L"事件管理", L"重要事件和日程安�?, 1003, false},
                {L"🖼�?, L"壁纸设置", L"壁纸管理和切换间�?, 1004, false},
                {L"📢", L"告示牌设�?, L"HTML内容编辑和预�?, 1005, true}
            };
            
            // Create card container
            int cardStartY = y;
            y += 20;
            
            for (int i = 0; i < 5; i++) {
                // Create card-style button with icon and description
                HWND hButton = CreateWindowW(L"BUTTON", L"",
                    WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
                    centerX - buttonWidth/2, y, buttonWidth, buttonHeight,
                    hwnd, (HMENU)(UINT_PTR)buttons[i].id, GetModuleHandle(nullptr), nullptr);
                
                // Store button info for custom drawing
                SetWindowLongPtrW(hButton, GWLP_USERDATA, (LONG_PTR)&buttons[i]);
                
                y += buttonHeight + 16;
            }
            
            y += 20;
            
            // Bottom action buttons with modern styling
            const int bottomButtonHeight = 36;
            HWND hSaveBtn = CreateWindowW(L"BUTTON", L"💾 保存配置",
                WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
                centerX - 160, y, 100, bottomButtonHeight,
                hwnd, (HMENU)1006, GetModuleHandle(nullptr), nullptr);
            
            HWND hReloadBtn = CreateWindowW(L"BUTTON", L"🔄 重新加载",
                WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
                centerX - 50, y, 100, bottomButtonHeight,
                hwnd, (HMENU)1007, GetModuleHandle(nullptr), nullptr);
            
            HWND hExitBtn = CreateWindowW(L"BUTTON", L"�?退�?,
                WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
                centerX + 60, y, 80, bottomButtonHeight,
                hwnd, (HMENU)1008, GetModuleHandle(nullptr), nullptr);
            
            return 0;
        }
        
    case WM_DRAWITEM:
        {
            LPDRAWITEMSTRUCT pDIS = (LPDRAWITEMSTRUCT)lParam;
            if (pDIS->CtlType == ODT_BUTTON) {
                RECT rect = pDIS->rcItem;
                HDC hdc = pDIS->hDC;
                bool isPressed = (pDIS->itemState & ODS_SELECTED) != 0;
                bool isHovered = buttonHoverStates[pDIS->hwndItem];
                
                // Get button info for main buttons
                FluentButtonInfo* buttonInfo = (FluentButtonInfo*)GetWindowLongPtrW(pDIS->hwndItem, GWLP_USERDATA);
                
                if (buttonInfo) {
                    // Draw main card-style button
                    DrawFluentCard(hdc, rect, 8);
                    
                    // Draw icon
                    RECT iconRect = {rect.left + 16, rect.top + 8, rect.left + 48, rect.top + 40};
                    SetBkMode(hdc, TRANSPARENT);
                    SetTextColor(hdc, buttonInfo->isAccent ? FLUENT_ACCENT_COLOR : FLUENT_TEXT_PRIMARY);
                    HFONT hIconFont = CreateFluentFont(20, false);
                    HFONT hOldFont = (HFONT)SelectObject(hdc, hIconFont);
                    DrawTextW(hdc, buttonInfo->icon, -1, &iconRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                    
                    // Draw title
                    RECT titleRect = {rect.left + 56, rect.top + 8, rect.right - 16, rect.top + 28};
                    SetTextColor(hdc, FLUENT_TEXT_PRIMARY);
                    SelectObject(hdc, CreateFluentFont(12, true));
                    DrawTextW(hdc, buttonInfo->title, -1, &titleRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
                    
                    // Draw description
                    RECT descRect = {rect.left + 56, rect.top + 28, rect.right - 16, rect.bottom - 8};
                    SetTextColor(hdc, FLUENT_TEXT_SECONDARY);
                    SelectObject(hdc, CreateFluentFont(10, false));
                    DrawTextW(hdc, buttonInfo->description, -1, &descRect, DT_LEFT | DT_TOP | DT_WORDBREAK);
                    
                    SelectObject(hdc, hOldFont);
                    DeleteObject(hIconFont);
                } else {
                    // Draw bottom buttons
                    wchar_t buttonText[256];
                    GetWindowTextW(pDIS->hwndItem, buttonText, 256);
                    
                    bool isAccent = (pDIS->CtlID == 1006); // Save button is accent
                    DrawFluentButton(hdc, rect, buttonText, isHovered, isPressed, isAccent);
                }
                
                return TRUE;
            }
            break;
        }
        
    case WM_CTLCOLORSTATIC:
        {
            HDC hdc = (HDC)wParam;
            HWND hControl = (HWND)lParam;
            
            // Check if it's the title (larger font)
            HFONT hControlFont = (HFONT)SendMessage(hControl, WM_GETFONT, 0, 0);
            if (hControlFont == hTitleFont) {
                SetTextColor(hdc, FLUENT_TEXT_PRIMARY);
            } else {
                SetTextColor(hdc, FLUENT_TEXT_SECONDARY);
            }
            
            SetBkColor(hdc, FLUENT_BG_COLOR);
            return (LRESULT)hBackgroundBrush;
        }
        
    case WM_MOUSEMOVE:
        {
            POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            HWND hChild = ChildWindowFromPoint(hwnd, pt);
            
            if (hChild && hChild != hwnd) {
                wchar_t className[256];
                GetClassNameW(hChild, className, 256);
                
                if (wcscmp(className, L"Button") == 0) {
                    if (!buttonHoverStates[hChild]) {
                        buttonHoverStates[hChild] = true;
                        InvalidateRect(hChild, nullptr, FALSE);
                        
                        // Track mouse leave
                        TRACKMOUSEEVENT tme = {};
                        tme.cbSize = sizeof(tme);
                        tme.dwFlags = TME_LEAVE;
                        tme.hwndTrack = hChild;
                        TrackMouseEvent(&tme);
                    }
                }
            }
            break;
        }
        
    case WM_MOUSELEAVE:
        {
            // Reset all hover states
            for (auto& pair : buttonHoverStates) {
                if (pair.second) {
                    pair.second = false;
                    InvalidateRect(pair.first, nullptr, FALSE);
                }
            }
            break;
        }
        
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case 1001: // Basic Settings
            ShowBasicSettingsDialog();
            break;
        case 1002: // Schedule Settings
            ShowScheduleSettingsDialog();
            break;
        case 1003: // Events Settings
            ShowEventsSettingsDialog();
            break;
        case 1004: // Wallpapers Settings
            ShowWallpapersSettingsDialog();
            break;
        case 1005: // Notice Settings
            ShowNoticeSettingsDialog();
            break;
        case 1006: // Save Config
            if (SaveConfig()) {
                MessageBoxW(hwnd, L"配置保存成功�?, L"成功", MB_OK | MB_ICONINFORMATION);
            } else {
                MessageBoxW(hwnd, L"配置保存失败�?, L"错误", MB_OK | MB_ICONERROR);
            }
            break;
        case 1007: // Reload Config
            if (LoadConfig()) {
                MessageBoxW(hwnd, L"配置重新加载成功�?, L"成功", MB_OK | MB_ICONINFORMATION);
            } else {
                MessageBoxW(hwnd, L"配置重新加载失败�?, L"错误", MB_OK | MB_ICONERROR);
            }
            break;
        case 1008: // Exit
            PostMessage(hwnd, WM_CLOSE, 0, 0);
            break;
        }
        break;
        
    case WM_CLOSE:
        if (hFont) DeleteObject(hFont);
        if (hTitleFont) DeleteObject(hTitleFont);
        if (hBackgroundBrush) DeleteObject(hBackgroundBrush);
        DestroyWindow(hwnd);
        break;
        
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
        
    default:
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
    return 0;
}

// Modern WinMain function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Set UTF-8 code page for console output (if needed)
    SetConsoleOutputCP(CP_UTF8);
    
    // Initialize COM for file dialogs
    CoInitialize(nullptr);
    
    // Initialize common controls
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_WIN95_CLASSES | ICC_STANDARD_CLASSES;
    InitCommonControlsEx(&icex);
    
    // Load current configuration
    LoadConfig();
    
    // Register window class
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = MainWindowProc;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(MODERN_BG_COLOR);
    wc.lpszClassName = L"ClassPaperSettingsMain";
    wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
    
    if (!RegisterClassExW(&wc)) {
        MessageBoxW(nullptr, L"窗口类注册失败！", L"错误", MB_OK | MB_ICONERROR);
        return 1;
    }
    
    // Create main window with modern styling
    HWND hwnd = CreateWindowExW(
        WS_EX_APPWINDOW, // Clean modern window
        L"ClassPaperSettingsMain",
        L"ClassPaper",
        WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME,
        CW_USEDEFAULT, CW_USEDEFAULT, 640, 580, // Larger size for card layout
        nullptr, nullptr, hInstance, nullptr
    );
    
    if (!hwnd) {
        DWORD error = GetLastError();
        wchar_t errorMsg[256];
        swprintf(errorMsg, 256, L"主窗口创建失败！错误代码: %lu", error);
        MessageBoxW(nullptr, errorMsg, L"错误", MB_OK | MB_ICONERROR);
        return 1;
    }
    
    // Debug: Show that window was created successfully
    MessageBoxW(nullptr, L"窗口创建成功，即将显�?, L"调试信息", MB_OK | MB_ICONINFORMATION);
    
    // Enable modern window effects (optional, comment out if causing issues)
    // BOOL enable = TRUE;
    // DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &enable, sizeof(enable));
    
    // Center window on screen
    RECT rect;
    GetWindowRect(hwnd, &rect);
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    SetWindowPos(hwnd, nullptr, 
        (screenWidth - width) / 2, (screenHeight - height) / 2,
        0, 0, SWP_NOSIZE | SWP_NOZORDER);
    
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    
    // Message loop
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    CoUninitialize();
    return (int)msg.wParam;
}
