// Call JavaScript from C++ Example

// Include the WebUI header
#include "webui.hpp"

#include "toml.hpp"

// Include C++ STD
#include <iostream>
#include <stdexcept>
#include <string>
#include <random>
#include <filesystem>

// Windows headers
#include <windows.h>
#include <tchar.h>
#include <Shobjidl.h>
#include <shellapi.h>

// 壁纸设置相关
class WallpaperManager
{
private:
    HWND workerw = nullptr;

    static BOOL CALLBACK FindWorkerWProc(HWND handle, LPARAM lparam)
    {
        auto *manager = reinterpret_cast<WallpaperManager *>(lparam);
        HWND defview = FindWindowEx(handle, nullptr, _T("SHELLDLL_DefView"), nullptr);

        if (defview)
        {
            manager->workerw = FindWindowEx(nullptr, handle, _T("WorkerW"), nullptr);
            return FALSE; // 停止枚举
        }
        return TRUE;
    }

public:
    bool SetAsWallpaper(HWND appHwnd)
    {
        if (!appHwnd)
            return false;

        // 获取Progman窗口
        HWND progman = FindWindow(_T("Progman"), nullptr);
        if (!progman)
            return false;

        // 发送消息创建WorkerW窗口
        DWORD_PTR result;
        SendMessageTimeout(progman, 0x052c, 0, 0, SMTO_NORMAL, 1000, &result);

        // 查找WorkerW窗口
        EnumWindows(FindWorkerWProc, reinterpret_cast<LPARAM>(this));

        if (workerw)
        {
            ShowWindow(workerw, SW_HIDE);
        }

        // 设置为桌面子窗口
        SetParent(appHwnd, progman);
        return true;
    }
};

// COM GUIDs for taskbar operations
extern "C"
{
    const GUID CLSID_TaskbarList = {0x56FDF344, 0xFD6D, 0x11D0, {0x95, 0x8A, 0x00, 0x60, 0x97, 0xC9, 0xA0, 0x90}};
    const GUID IID_ITaskbarList = {0x56FDF342, 0xFD6D, 0x11D0, {0x95, 0x8A, 0x00, 0x60, 0x97, 0xC9, 0xA0, 0x90}};
    const GUID IID_ITaskbarList2 = {0x602D4995, 0xB13A, 0x429b, {0xA6, 0x6E, 0x19, 0x35, 0xE4, 0x4F, 0x43, 0x17}};
    const GUID IID_ITaskbarList3 = {0xEA1AFB91, 0x9E28, 0x4B86, {0x90, 0xE9, 0x9E, 0x9F, 0x8A, 0x5E, 0xEF, 0xAF}};
}

class ComInitializer
{
public:
    ComInitializer() { CoInitialize(nullptr); }
    ~ComInitializer() { CoUninitialize(); }
};

// 窗口管理器
class WindowManager
{
private:
    ComInitializer com;

    template <typename T>
    bool CreateTaskbarInterface(T **ppInterface, const GUID &iid)
    {
        return SUCCEEDED(CoCreateInstance(CLSID_TaskbarList, nullptr, CLSCTX_INPROC_SERVER,
                                          iid, reinterpret_cast<void **>(ppInterface)));
    }

public:
    // 从任务栏隐藏窗口（不影响托盘图标）
    bool HideFromTaskbar(HWND hwnd)
    {
        if (!hwnd)
            return false;

        // 方法1：修改窗口样式
        LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
        exStyle |= WS_EX_TOOLWINDOW; // 添加工具窗口样式
        exStyle &= ~WS_EX_APPWINDOW; // 移除应用窗口样式
        SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle);

        // 方法2：使用TaskbarList API作为备用
        // bool success = false;
        ITaskbarList3 *taskbar3 = nullptr;
        if (CreateTaskbarInterface(&taskbar3, IID_ITaskbarList3))
        {
            taskbar3->HrInit();
            taskbar3->UnregisterTab(hwnd);
            taskbar3->Release();
            // success = true;
        }

        // 刷新窗口显示
        ShowWindow(hwnd, SW_HIDE);
        ShowWindow(hwnd, SW_SHOW);

        return true;
    }

    // 显示在任务栏
    bool ShowInTaskbar(HWND hwnd)
    {
        if (!hwnd)
            return false;

        // 恢复窗口样式
        LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
        exStyle &= ~WS_EX_TOOLWINDOW; // 移除工具窗口样式
        exStyle |= WS_EX_APPWINDOW;   // 添加应用窗口样式
        SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle);

        // 使用TaskbarList API
        ITaskbarList *taskbar = nullptr;
        if (CreateTaskbarInterface(&taskbar, IID_ITaskbarList))
        {
            taskbar->HrInit();
            taskbar->AddTab(hwnd);
            taskbar->Release();

            // 刷新窗口显示
            ShowWindow(hwnd, SW_HIDE);
            ShowWindow(hwnd, SW_SHOW);
            return true;
        }

        return false;
    }

    // 设置窗口为无焦点（不会抢夺焦点）
    bool SetNoActivate(HWND hwnd)
    {
        if (!hwnd)
            return false;

        LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
        exStyle |= WS_EX_NOACTIVATE;
        SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle);

        return true;
    }
};

struct EnumWindowsData
{
    std::string keyword;
    HWND hwnd = nullptr;
};

BOOL CALLBACK EnumWindowsProc2(HWND hwnd, LPARAM lParam)
{
    EnumWindowsData *data = reinterpret_cast<EnumWindowsData *>(lParam);

    char buffer[256];
    GetWindowTextA(hwnd, buffer, sizeof(buffer));

    std::string title(buffer);

    if (title.find(data->keyword) != std::string::npos)
    {
        data->hwnd = hwnd;
        return FALSE; // 停止枚举
    }

    return TRUE;
}

std::string generateRandomCharacters(int count)
{
    static const std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, characters.size() - 1);

    std::string result;
    result.reserve(count);
    for (int i = 0; i < count; ++i)
    {
        result += characters[dis(gen)];
    }

    return result;
}

// 全局对象
webui::window my_window;
HWND bw_hwnd = nullptr;
WallpaperManager wallpaper_manager;
WindowManager window_manager;

bool isHttpUrl(const std::string &url)
{
    return url.find("http://") == 0 || url.find("https://") == 0;
}

std::string normalizeLocalPath(const std::string &url)
{
    std::string normalized = url;

    // 移除所有res/前缀
    size_t res_pos = normalized.find("res/");
    while (res_pos != std::string::npos)
    {
        normalized.erase(res_pos, 4);
        res_pos = normalized.find("res/");
    }

    return normalized;
}

int funcmain()
{
    std::string url = "./res/index.html";

    try
    {
        const auto config = toml::parse("config.toml");

        // 使用toml::find_or来获取值，如果不存在则使用默认值
        url = toml::find_or(config, "default", "url", url);

        if (url.empty())
        {
            throw std::runtime_error("URL cannot be empty in config file");
        }

        // 处理本地路径
        if (!isHttpUrl(url))
        {
            url = normalizeLocalPath(url);

            const auto base_path = std::filesystem::current_path();
            const auto res_path = base_path / "res";
            const auto full_path = std::filesystem::weakly_canonical(res_path / url);

            // 安全检查：确保路径在res目录内
            if (full_path.string().find(res_path.string()) != 0)
            {
                throw std::runtime_error("Illegal path access: " + full_path.string());
            }

            my_window.set_root_folder(res_path.string());

            // 检查文件是否存在
            if (!std::filesystem::exists(full_path))
            {
                std::cerr << "Warning: File not found " << full_path << std::endl;
            }
        }
    }
    catch (const toml::exception &e)
    {
        std::cerr << "TOML parsing error: " << e.what() << std::endl;
        return 1;
    }
    catch (const std::exception &e)
    {
        std::cerr << "File access error: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "Loading: " << url << std::endl;

    // 配置并显示窗口
    my_window.set_root_folder("res/");
    my_window.set_kiosk(true);
    my_window.show(url);

    // 设置随机窗口标题
    const std::string wname = "classpaper" + generateRandomCharacters(6);
    char response[64];

    if (!my_window.script("document.title=\"" + wname + "\"; return \"ok\";", 0, response, 64))
    {
        if (!my_window.is_shown())
        {
            std::cout << "Window closed." << std::endl;
        }
        else
        {
            std::cout << "JavaScript error: " << response << std::endl;
        }
    }

    Sleep(300);

    // 查找窗口并设置为桌面背景
    EnumWindowsData data{wname, nullptr};
    EnumWindows(EnumWindowsProc2, reinterpret_cast<LPARAM>(&data));

    if (data.hwnd)
    {
        std::cout << "Window found: " << data.hwnd << std::endl;

        // 设置为桌面壁纸
        if (wallpaper_manager.SetAsWallpaper(data.hwnd))
        {
            std::cout << "Set as wallpaper successfully" << std::endl;
        }

        Sleep(300);

        // 从任务栏隐藏
        if (window_manager.HideFromTaskbar(data.hwnd))
        {
            std::cout << "Hidden from taskbar successfully" << std::endl;
        }

        // 设置为无焦点窗口
        window_manager.SetNoActivate(data.hwnd);

        Sleep(5000);
        window_manager.HideFromTaskbar(data.hwnd); // 再次确保隐藏
        bw_hwnd = data.hwnd;
    }
    else
    {
        std::cout << "Window not found" << std::endl;
    }

    return 0;
}

#include <type_traits>
#include "res.hpp"
#include <thread>

#define WM_TRAYICON (WM_APP + 1)
#define ID_TRAYICON 1001

#define IDM_RELOAD 1001
#define IDM_SETTINGS 1002
#define IDM_RESTART 1003
#define IDM_EXIT 1004
#define IDM_SETDESKTOP 1005

class TrayIcon
{
private:
    NOTIFYICONDATA m_data{};
    HWND m_hwnd{};

public:
    explicit TrayIcon(HWND hwnd) : m_hwnd(hwnd)
    {
        m_data.cbSize = sizeof(NOTIFYICONDATA);
        m_data.hWnd = hwnd;
        m_data.uID = ID_TRAYICON;
        m_data.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
        m_data.uCallbackMessage = WM_TRAYICON;
        m_data.hIcon = CreateIconFromResource(favicon_ico, favicon_ico_len, TRUE, 0x00030000);
        lstrcpyn(m_data.szTip, TEXT("ClassPaper"), sizeof(m_data.szTip) / sizeof(TCHAR));
        Shell_NotifyIcon(NIM_ADD, &m_data);
    }

    ~TrayIcon()
    {
        Shell_NotifyIcon(NIM_DELETE, &m_data);
        if (m_data.hIcon)
        {
            DestroyIcon(m_data.hIcon);
        }
    }
};

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static const UINT WM_TASKBARCREATED = RegisterWindowMessage(TEXT("TaskbarCreated"));

    switch (uMsg)
    {
    case WM_CREATE:
    {
        // 初始化系统托盘
        static TrayIcon tray(hwnd);
        break;
    }
    case WM_COMMAND:
    {
        // 处理右键菜单选项
        switch (LOWORD(wParam))
        {
        case IDM_RELOAD:
            std::cout << "Reloading page..." << std::endl;
            my_window.run("location.reload(true);");

            if (!my_window.is_shown())
            {
                std::cout << "Window closed." << std::endl;
            }
            else
            {
                Sleep(300);
                if (bw_hwnd)
                {
                    window_manager.HideFromTaskbar(bw_hwnd);
                }
            }
            break;

        case IDM_SETDESKTOP:
            std::cout << "Setting desktop wallpaper..." << std::endl;

            if (bw_hwnd)
            {
                if (wallpaper_manager.SetAsWallpaper(bw_hwnd))
                {
                    std::cout << "Set as wallpaper successfully" << std::endl;
                    Sleep(300);
                    window_manager.HideFromTaskbar(bw_hwnd);
                    window_manager.SetNoActivate(bw_hwnd);
                    Sleep(5000);
                    window_manager.HideFromTaskbar(bw_hwnd);
                }
                else
                {
                    std::cout << "Failed to set wallpaper" << std::endl;
                }
            }
            else
            {
                std::cout << "Window handle not found" << std::endl;
            }
            break;

        case IDM_SETTINGS:
        {
            std::cout << "Opening settings..." << std::endl;

            TCHAR szFilePath[MAX_PATH];
            GetModuleFileName(nullptr, szFilePath, MAX_PATH);

            // 修改后的路径处理
            std::filesystem::path exePath(szFilePath);
            std::string strSettingPath = (exePath.parent_path() / "settings.exe").string();

            // 修改调用方式
            HINSTANCE hResult = ShellExecuteW(nullptr, L"open",
                                              std::filesystem::path(strSettingPath).wstring().c_str(),
                                              nullptr, nullptr, SW_SHOWDEFAULT);

            if (reinterpret_cast<intptr_t>(hResult) <= 32)
            {
                std::cerr << "Failed to open settings! Error code: "
                          << reinterpret_cast<intptr_t>(hResult) << std::endl;
            }
            break;
        }
        case IDM_RESTART:
        {
            std::cout << "Restarting program..." << std::endl;

            TCHAR szFilePath[MAX_PATH];
            GetModuleFileName(nullptr, szFilePath, MAX_PATH);

            STARTUPINFO si = {sizeof(si)};
            PROCESS_INFORMATION pi;

            if (CreateProcess(szFilePath, nullptr, nullptr, nullptr, FALSE, 0,
                              nullptr, nullptr, &si, &pi))
            {
                CloseHandle(pi.hThread);
                CloseHandle(pi.hProcess);
                std::cout << "Program restarted successfully" << std::endl;
            }
            else
            {
                std::cout << "Failed to restart program" << std::endl;
            }

            PostQuitMessage(0);
            break;
        }
        case IDM_EXIT:
            PostQuitMessage(0);
            break;
        }

        break;
    }
    case WM_TRAYICON:
    {
        switch (LOWORD(lParam))
        {
        case WM_RBUTTONUP: // 使用RBUTTONUP而不是RBUTTONDOWN，响应更快
        {
            POINT cursorPos;
            GetCursorPos(&cursorPos);

            HMENU hPopupMenu = CreatePopupMenu();
            if (hPopupMenu)
            {
                // 快速构建菜单
                AppendMenu(hPopupMenu, MF_STRING, IDM_RELOAD, TEXT("页面重载"));
                AppendMenu(hPopupMenu, MF_STRING, IDM_SETDESKTOP, TEXT("桌面穿透"));
                AppendMenu(hPopupMenu, MF_STRING, IDM_SETTINGS, TEXT("设置"));
                AppendMenu(hPopupMenu, MF_SEPARATOR, 0, nullptr);
                AppendMenu(hPopupMenu, MF_STRING, IDM_RESTART, TEXT("重启"));
                AppendMenu(hPopupMenu, MF_STRING, IDM_EXIT, TEXT("退出"));

                // 优化菜单显示 - 减少延迟
                SetForegroundWindow(hwnd);

                // 使用同步模式和优化的标志
                UINT result = TrackPopupMenu(hPopupMenu,
                                             TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY,
                                             cursorPos.x, cursorPos.y, 0, hwnd, nullptr);

                DestroyMenu(hPopupMenu);

                // 如果用户选择了菜单项，直接处理
                if (result != 0)
                {
                    SendMessage(hwnd, WM_COMMAND, result, 0);
                }
            }
            break;
        }
        case WM_LBUTTONDOWN:
        {
            // 左键单击也显示菜单
            PostMessage(hwnd, WM_TRAYICON, wParam, MAKELPARAM(WM_RBUTTONUP, HIWORD(lParam)));
            break;
        }
        case WM_LBUTTONDBLCLK:
        {
            // 双击重新加载
            SendMessage(hwnd, WM_COMMAND, IDM_RELOAD, 0);
            break;
        }
        }
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        // 防止Explorer.exe崩溃后托盘图标消失
        if (uMsg == WM_TASKBARCREATED)
            SendMessage(hwnd, WM_CREATE, wParam, lParam);
        break;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// 在 WinMain 函数中调整初始化顺序
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // 注册窗口类
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = TEXT("TrayMenu");
    RegisterClass(&wc);

    // 注册窗口类后添加
    std::cout << "[系统] 窗口类注册完成，开始创建消息窗口..." << std::endl;
    HWND hWnd = CreateWindowEx(0, wc.lpszClassName, TEXT(""), 0, 0, 0, 0, 0, NULL, NULL, hInstance, NULL);
    if (!hWnd) {
        std::cerr << "[错误] 无法创建消息窗口! 错误代码: " << GetLastError() << std::endl;
        return 1;
    }
    std::cout << "[系统] 消息窗口已创建，句柄: " << hWnd << std::endl;

    // 在独立线程中运行主逻辑
    std::thread mainThread([](){
        funcmain();
    });
    mainThread.detach();  // 分离线程避免阻塞消息循环

    // 立即进入消息循环
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    webui::exit();
    return static_cast<int>(msg.wParam);
}