#include <stdio.h>
#include <windows.h>

#define WIDTH 800
#define HEIGHT 600
#define COLOR_WHITE 0xFFFFFF

struct Circle {
    double x;
    double y;
    float radius;
    float area;
};

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

int main() {
    WNDCLASSW wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandleW(NULL);
    wc.lpszClassName = L"RayTracingWindow";
    wc.hCursor = LoadCursorW(NULL, (LPCWSTR)IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    
    if (!RegisterClassW(&wc)) {
        return 1;
    }
    
    HWND hwnd = CreateWindowW(
        L"RayTracingWindow",
        L"Ray Tracer",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        WIDTH, HEIGHT,
        NULL,
        NULL,
        GetModuleHandleW(NULL),
        NULL
    );
    
    if (hwnd == NULL) {
        return 1;
    }
    
    ShowWindow(hwnd, SW_SHOW);
    
    MSG msg = {0};
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    
    return 0;
}