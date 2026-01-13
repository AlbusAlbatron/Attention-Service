#ifndef UNICODE
#define UNICODE
#endif

#ifndef _UNICODE
#define _UNICODE
#endif

//Define Menu handle codes
#define START_BUTTON 600


#include <stdio.h>
#include <windows.h>
#include <wchar.h>
#include <commctrl.h>

//Make hInstance global
HINSTANCE g_hInstance;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    // Register the window class.
    const wchar_t CLASS_NAME[] = L"MAIN WINDOW";

    WNDCLASS wc = { };

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    g_hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // Create the window.

    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"Attention Service",    // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );

    if (hwnd == NULL)
    {
        return 0;
    }


    ShowWindow(hwnd, nCmdShow);

    // Run the message loop.

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}



LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    
    case WM_CREATE: {
        CreateWindowW(L"BUTTON", L"Start Block", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 200, 200, 100, 50, hwnd, (HMENU)START_BUTTON, g_hInstance, NULL);

        HWND edit_area = CreateWindow(L"edit", NULL, WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL |
            WS_BORDER | ES_LEFT | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL,
            300, 300, 300, 50, hwnd, (HMENU)1, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

        break;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        SetBkColor(hdc, GetSysColor(COLOR_BTNFACE));
        SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));

        // All painting occurs here, between BeginPaint and EndPaint.

        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

        RECT rect;
        HDC wdc = GetDC(hwnd);
        GetClientRect(hwnd, &rect);
        SetTextColor(wdc, 0x00000000);
        SetBkMode(wdc, TRANSPARENT);
        rect.left = 100;
        rect.top = 100;
        DrawText(wdc, L"MENU", -1, &rect, DT_SINGLELINE | DT_NOCLIP);
        SetWindowText(hwnd, L"Type Here NOW!");
        ReleaseDC(hwnd, wdc);

        EndPaint(hwnd, &ps);
        break;
    }
    case WM_LBUTTONDOWN: {
        break;
    }
    case WM_SYSCOLORCHANGE: {
        InvalidateRect(hwnd, NULL, TRUE);
        break;
    }
    case WM_COMMAND: {
        HDC wdc = GetDC(hwnd);

        if (LOWORD(wParam) == START_BUTTON) {
            TextOut(wdc, 50, 50, L"Blocking has started", 20);
        }

        ReleaseDC(hwnd, wdc);
    }

    return 0;

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}