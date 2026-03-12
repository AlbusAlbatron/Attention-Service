#ifndef UNICODE
#define UNICODE
#endif

#ifndef _UNICODE
#define _UNICODE
#endif

// Menu handle codes
#define START_BUTTON 600

#define PROCESS_INPUT_AREA 601
#define ADD_PROCESS_BUTTON 602
#define REMOVE_PROCESS_BUTTON 603

#define WEB_INPUT_AREA 604
#define ADD_WEB_BUTTON 605
#define REMOVE_WEB_BUTTON 606

#define TIMER_SET_INPUT 607
#define TIMER_SET_BUTTON 608

#include <stdio.h>
#include <windows.h>
#include <wchar.h>

HINSTANCE g_hInstance;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    const wchar_t CLASS_NAME[] = L"Attention_Service2";

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    g_hInstance = hInstance;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"Attention Service",
        WS_OVERLAPPEDWINDOW,
        100, 100, 900, 700,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hwnd == NULL)
    {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    MSG msg = { 0 };
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
        // Process Block
        CreateWindowEx(0, L"BUTTON", L"Process Settings",
            WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
            10, 10, 250, 100, hwnd, 0, g_hInstance, NULL);

        CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
            20, 35, 150, 20, hwnd, (HMENU)PROCESS_INPUT_AREA, g_hInstance, NULL);

        CreateWindowEx(0, L"BUTTON", L"Add",
            WS_CHILD | WS_VISIBLE,
            20, 60, 60, 25, hwnd, (HMENU)ADD_PROCESS_BUTTON, g_hInstance, NULL);

        CreateWindowEx(0, L"BUTTON", L"Remove",
            WS_CHILD | WS_VISIBLE,
            85, 60, 60, 25, hwnd, (HMENU)REMOVE_PROCESS_BUTTON, g_hInstance, NULL);

        // Web Block
        CreateWindowEx(0, L"BUTTON", L"Web Settings",
            WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
            10, 120, 250, 100, hwnd, 0, g_hInstance, NULL);

        CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
            20, 145, 150, 20, hwnd, (HMENU)WEB_INPUT_AREA, g_hInstance, NULL);

        CreateWindowEx(0, L"BUTTON", L"Add",
            WS_CHILD | WS_VISIBLE,
            20, 170, 60, 25, hwnd, (HMENU)ADD_WEB_BUTTON, g_hInstance, NULL);

        CreateWindowEx(0, L"BUTTON", L"Remove",
            WS_CHILD | WS_VISIBLE,
            85, 170, 60, 25, hwnd, (HMENU)REMOVE_WEB_BUTTON, g_hInstance, NULL);

        // Start Button
        CreateWindowEx(0, L"BUTTON", L"START",
            WS_CHILD | WS_VISIBLE,
            10, 230, 100, 35, hwnd, (HMENU)START_BUTTON, g_hInstance, NULL);

        break;
    }

    case WM_COMMAND: {
        switch (LOWORD(wParam)) {
        case START_BUTTON:
            MessageBox(hwnd, L"Start button clicked", L"Info", MB_OK);
            break;
        case ADD_PROCESS_BUTTON:
            MessageBox(hwnd, L"Add Process clicked", L"Info", MB_OK);
            break;
        case REMOVE_PROCESS_BUTTON:
            MessageBox(hwnd, L"Remove Process clicked", L"Info", MB_OK);
            break;
        case ADD_WEB_BUTTON:
            MessageBox(hwnd, L"Add Web clicked", L"Info", MB_OK);
            break;
        case REMOVE_WEB_BUTTON:
            MessageBox(hwnd, L"Remove Web clicked", L"Info", MB_OK);
            break;
        }
        break;
    }

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}