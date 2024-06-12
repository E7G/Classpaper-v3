// Call JavaScript from C++ Example

// Include the WebUI header
#include "webui.hpp"
#include "inicpp.hpp"



// Include C++ STD
#include <iostream>
#include <stdexcept>
#include <string>
#include <random>

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <tchar.h>
#include <Shobjidl.h>


#include <Windows.h>
#include <shellapi.h>

HWND workerw=NULL;     //�ڶ���WorkerW���ھ��
 
inline BOOL CALLBACK EnumWindowsProc1(HWND handle,LPARAM lparam)
{
    //��ȡ��һ��WorkerW����
    HWND defview = FindWindowEx(handle, 0, _T("SHELLDLL_DefView"), NULL);
 
    if (defview != NULL) //�ҵ���һ��WorkerW����
    {
        //��ȡ�ڶ���WorkerW���ڵĴ��ھ��
        workerw = FindWindowEx(0, handle, _T("WorkerW"), 0);
    }
    return true;
}
 
//����myAppHwndΪ�㿪���Ĵ��ڳ���Ĵ��ھ��
void SetDesktop(HWND myAppHwnd)
{
    int result;
    HWND windowHandle = FindWindow(_T("Progman"), NULL);
    SendMessageTimeout(windowHandle, 0x052c, 0 ,0, SMTO_NORMAL, 0x3e8,(PDWORD_PTR)&result);
 
    //ö�ٴ���
    EnumWindows(EnumWindowsProc1,(LPARAM)NULL);
 
    //���صڶ���WorkerW���ڣ�����ProgmanΪ������ʱ��Ҫ����������أ�
    //��Ȼ���򴰿ڻᱻ�ڶ���WorkerW����
    ShowWindow(workerw,SW_HIDE);
 
    SetParent(myAppHwnd,windowHandle);
}



extern "C" {const GUID CLSID_TaskbarList = {0x56FDF344, 0xFD6D, 0x11D0, 
{0x95, 0x8A, 0x00, 0x60, 0x97, 0xC9, 0xA0, 0x90} };
                          const GUID IID_ITaskbarList  = {0x56FDF342, 0xFD6D, 
0x11D0, {0x95, 0x8A, 0x00, 0x60, 0x97, 0xC9, 0xA0, 0x90} }; }

extern "C" {const GUID IID_ITaskbarList2 = {0x602D4995, 0xB13A, 0x429b, {0xA6, 
0x6E, 0x19, 0x35, 0xE4, 0x4F, 0x43, 0x17} }; }

extern "C" {const GUID IID_ITaskbarList3 = { 0xEA1AFB91, 0x9E28, 0x4B86, {0x90, 
0xE9, 0x9E, 0x9F, 0x8A, 0x5E, 0xEF, 0xAF} };}


void UnregisterTab(HWND tab)
{
    LPVOID lp = NULL ;
    CoInitialize(lp);//��ʼ��COM�⣺û�����������ز�������

    ITaskbarList3 *taskbar;

    if(!(tab))
        return;

    if(S_OK != CoCreateInstance(CLSID_TaskbarList, 0, CLSCTX_INPROC_SERVER, IID_ITaskbarList3, (void**)&taskbar))
        return;
    taskbar->HrInit();
    taskbar->UnregisterTab(tab);
    taskbar->Release();
}


BOOL ShowInTaskbar(HWND hWnd, BOOL bShow)
{
    LPVOID lp = NULL ;
    CoInitialize(lp);//��ʼ��COM�⣺û�����������ز�������
 
 
    HRESULT hr; 
    ITaskbarList* pTaskbarList;
    hr = CoCreateInstance( CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER,  
        IID_ITaskbarList, (void**)&pTaskbarList );
    if(SUCCEEDED(hr))
    {
        pTaskbarList->HrInit();
        if(bShow)
            pTaskbarList->AddTab(hWnd);
        else
            pTaskbarList->DeleteTab(hWnd);
        pTaskbarList->Release();
        return TRUE;
    }
 
    return FALSE;
}

void deltab(HWND test_hwnd){
       ShowWindow(test_hwnd, SW_HIDE);

       //DWORD dwExStyle = GetWindowLong(test_hwnd, GWL_EXSTYLE);
     
       //dwExStyle &= ~(WS_VISIBLE);
     
       //dwExStyle |= WS_EX_TOOLWINDOW;   

       //dwExStyle &= ~(WS_EX_APPWINDOW);
     
       SetWindowLong(test_hwnd, GWL_EXSTYLE, WS_EX_TOOLWINDOW);

       // ʹ���ڲ��ܻ�ȡ����
    SetWindowLong(test_hwnd, GWL_EXSTYLE, GetWindowLong(test_hwnd, GWL_EXSTYLE) | WS_EX_NOACTIVATE);
     
     //DWORD dwStyle = GetWindowLong(test_hwnd, GWL_STYLE);
     
     //dwStyle &= ~(WS_CHILD);
     
       //dwStyle |= WS_POPUP;   

     
       SetWindowLong(test_hwnd, GWL_STYLE, WS_POPUP);
     
       ShowWindow(test_hwnd, SW_SHOW);
     
     
       UpdateWindow(test_hwnd);


       UnregisterTab(test_hwnd);

       ShowInTaskbar(test_hwnd, FALSE);


       printf("deltab worked\n");
 
      return;
}

struct EnumWindowsData {
    std::string keyword;
    HWND hwnd = nullptr;
};

BOOL CALLBACK EnumWindowsProc2(HWND hwnd, LPARAM lParam) {
    EnumWindowsData* data = reinterpret_cast<EnumWindowsData*>(lParam);

    char buffer[256];
    GetWindowTextA(hwnd, buffer, sizeof(buffer));

    std::string title(buffer);

    if (title.find(data->keyword) != std::string::npos) {
        data->hwnd = hwnd;
        return FALSE;  // ֹͣö��
    }

    return TRUE;
}



std::string generateRandomCharacters(int count) {
    std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, characters.size() - 1);

    std::string result;
    for (int i = 0; i < count; ++i) {
        result += characters[dis(gen)];
    }

    return result;
}



webui::window my_window;
HWND bw_hwnd;

int funcmain() {



    // Load the INI file.
    inicpp::IniManager _ini("config.ini");
    
    // Check if the key exists.
    if (!_ini["default"].isKeyExist("url"))
    {
        std::cout << "config.ini default.url is not exist!" << std::endl;
    }

    std::cout << "Loading : " << _ini["default"]["url"] << std::endl;

    // Create a window
    //webui::window my_window;

    // Bind HTML elements with C++ functions
    //my_window.bind("MyButton1", my_function_count);
    //my_window.bind("MyButton2", my_function_exit);

    // Show the window
    //my_window.show(my_html); // my_window.show_browser(my_html, Chrome);
    my_window.set_root_folder("res/");
    my_window.set_kiosk(true);
    my_window.show(_ini["default"]["url"]);

    //my_window.run("document.title=ClassPaper");

    char response[64];
    std::string wname="classpaper"+generateRandomCharacters(6);

    // Run JavaScript
    if (!my_window.script("document.title=\""+wname+"\"; return \"ok\";", 0, response, 64)) {

        if (!my_window.is_shown())
            std::cout << "Window closed." << std::endl;
        else
            std::cout << "JavaScript Error: " << response << std::endl;
        
    }
        


    Sleep(300);

    EnumWindowsData data;

    data.keyword = wname;

    EnumWindows(EnumWindowsProc2, reinterpret_cast<LPARAM>(&data));

    if (data.hwnd != nullptr) {
        std::cout << "found :  " << data.hwnd << std::endl;
        SetDesktop(data.hwnd);
        Sleep(300);
        deltab(data.hwnd);
        Sleep(1000*5);
        deltab(data.hwnd);
        bw_hwnd=data.hwnd;
    } else {
        std::cout << "not found" << std::endl;
    }



    // Wait until all windows get closed
    //webui::wait();



    return 0;
}

#include <type_traits> 

#define WM_TRAYICON (WM_APP + 1)
#define ID_TRAYICON 1001

#define IDM_RELOAD 1001
#define IDM_SETTINGS 1002
#define IDM_RESTART 1003
#define IDM_EXIT 1004
#define IDM_SETDESKTOP 1005

NOTIFYICONDATA g_notifyIconData;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

    // ��ȡ��ǰ��ִ���ļ���·��
    TCHAR szFilePath[MAX_PATH];
    GetModuleFileName(NULL, szFilePath, MAX_PATH);

    // ��ȡ��ִ���ļ����ڵ�Ŀ¼·��
    std::string strDirectory(szFilePath);
    std::size_t found;
    std::string strSettingPath;
    HINSTANCE hResult;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    EnumWindowsData data;


    switch (uMsg)
    {
        case WM_CREATE:
        {
            // ����ϵͳ����ͼ��
            g_notifyIconData.cbSize = sizeof(NOTIFYICONDATA);
            g_notifyIconData.hWnd = hwnd;
            g_notifyIconData.uID = ID_TRAYICON;
            g_notifyIconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
            g_notifyIconData.uCallbackMessage = WM_TRAYICON;
            g_notifyIconData.hIcon = LoadIcon(NULL, IDI_APPLICATION);
            lstrcpyn(g_notifyIconData.szTip, TEXT("ClassPaper"), sizeof(g_notifyIconData.szTip) / sizeof(TCHAR));
            Shell_NotifyIcon(NIM_ADD, &g_notifyIconData);

            break;
        }
        case WM_COMMAND:
        {
            // �����Ҽ��˵�ѡ��
            switch (LOWORD(wParam))
            {
                case IDM_RELOAD:
                    // ҳ�������߼�
                    //MessageBox(NULL, TEXT("ִ��ҳ�����ز���"), TEXT("ClassPaper"), MB_OK);

    // Run JavaScript
    my_window.run("location.reload(true);");

        if (!my_window.is_shown())
            std::cout << "Window closed." << std::endl;
        
        Sleep(300);
        deltab(bw_hwnd);

                    break;

                 case IDM_SETDESKTOP:
                    // ҳ�������߼�
                    //MessageBox(NULL, TEXT("ִ�����洩͸����"), TEXT("ClassPaper"), MB_OK);

        

    if (bw_hwnd != nullptr) {
        std::cout << "found :  " << bw_hwnd << std::endl;
        SetDesktop(bw_hwnd);
        Sleep(300);
        deltab(bw_hwnd);
        Sleep(1000*5);
        deltab(bw_hwnd);
    } else {
        std::cout << "not found" << std::endl;
    }
        

                    break;



                case IDM_SETTINGS:
                    // �����߼�
                    //MessageBox(NULL, TEXT("�����ý���"), TEXT("ClassPaper"), MB_OK);

    std::cout << "�����ó���..." << std::endl;

    found = strDirectory.find_last_of("/\\");
    strDirectory = strDirectory.substr(0, found);

    // �������ó��������·��
    strSettingPath = strDirectory + "\\setting.exe";

    // ���� ShellExecute �����������ó���
    hResult = ShellExecute(NULL, "open", strSettingPath.c_str(), NULL, NULL, SW_SHOWDEFAULT);

    if (static_cast<int>(reinterpret_cast<intptr_t>(hResult)) <= 32)
    {
        std::cerr << "�޷������ó��򣡴�����룺" << static_cast<int>(reinterpret_cast<intptr_t>(hResult)) << std::endl;
    }

                    break;
                case IDM_RESTART:
                    // �����߼�
                    //MessageBox(NULL, TEXT("ִ����������"), TEXT("ClassPaper"), MB_OK);

    std::cout << "ִ����������..." << std::endl;



    // �����½�����ִ����������
     si = { sizeof(si) };
    if (CreateProcess(szFilePath, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
    {
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
    }

                    Shell_NotifyIcon(NIM_DELETE, &g_notifyIconData);
                    PostQuitMessage(0);
                    break;
                case IDM_EXIT:
                    // �˳��߼�
                    Shell_NotifyIcon(NIM_DELETE, &g_notifyIconData);
                    PostQuitMessage(0);
                    break;
            }

            break;
        }
        case WM_TRAYICON:
        {
            // ��������ͼ����Ϣ
            if (LOWORD(lParam) == WM_RBUTTONDOWN)
            {
                // �����Ҽ��˵�
                POINT cursorPos;
                GetCursorPos(&cursorPos);

                deltab(bw_hwnd);

                HMENU hPopupMenu = CreatePopupMenu();
                AppendMenu(hPopupMenu, MF_STRING, IDM_RELOAD, TEXT("ҳ������"));
                AppendMenu(hPopupMenu, MF_STRING, IDM_SETDESKTOP, TEXT("���洩͸"));
                AppendMenu(hPopupMenu, MF_STRING, IDM_SETTINGS, TEXT("����"));
                AppendMenu(hPopupMenu, MF_STRING, IDM_RESTART, TEXT("����"));
                AppendMenu(hPopupMenu, MF_SEPARATOR, 0, NULL);
                AppendMenu(hPopupMenu, MF_STRING, IDM_EXIT, TEXT("�˳�"));

                SetForegroundWindow(hwnd);
                TrackPopupMenu(hPopupMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, cursorPos.x, cursorPos.y, 0, hwnd, NULL);
                PostMessage(hwnd, WM_NULL, 0, 0);
            }

            break;
        }
        case WM_DESTROY:
        {
            // �Ƴ�����ͼ�겢�˳�Ӧ�ó���
            Shell_NotifyIcon(NIM_DELETE, &g_notifyIconData);
            PostQuitMessage(0);

            break;
        }
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // ע�ᴰ����
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = TEXT("TrayMenu");
    RegisterClass(&wc);

     CreateWindowEx(0, wc.lpszClassName, TEXT(""), 0, 0, 0, 0, 0, NULL, NULL, hInstance, NULL);

    

    funcmain();


    

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    webui::exit();
    
    return static_cast<int>(msg.wParam);
}




