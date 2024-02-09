#include <iostream>
#include <windows.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

int main() {
    // 注册窗口类
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "MyHiddenWindow";
    wc.style = CS_OWNDC;  // 使用CS_OWNERDC样式
    RegisterClassEx(&wc);

    // 创建窗口
    HWND hwnd = CreateWindowEx(
        WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,  // 设置WS_EX_TOOLWINDOW和WS_EX_NOACTIVATE样式
        "MyHiddenWindow",
        "",
        0,
        CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, GetModuleHandle(NULL), NULL
    );
    if (hwnd == NULL) {
        std::cerr << "Failed to create window!" << std::endl;
        return 1;
    }

    ShowWindow(hwnd, SW_SHOW);

    SetTimer(hwnd, 0, 5 * 1000, [](HWND hWnd, UINT, UINT_PTR, DWORD) {
        KillTimer(hWnd, 0);
        SendMessage(hWnd, WM_CLOSE, 0, 0);
    });

    // 消息循环
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
