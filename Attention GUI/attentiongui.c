#ifndef UNICODE
#define UNICODE
#endif

#ifndef _UNICODE
#define _UNICODE
#endif

//Menu handle codes
#define START_BUTTON 600

#define EDIT_PROCESS_BUTTON 601
#define ADD_PROCESS_BUTTON 602
#define REMOVE_PROCESS_BUTTON 603

#define EDIT_WEB_BUTTON 604
#define ADD_WEB_BUTTON 605
#define REMOVE_WEB_BUTTON 606

#define TIMER_SET_INPUT 607
#define TIMER_SET_BUTTON 608

#include "attention.h"
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
        //Process Block
        CreateWindowEx(0, L"BUTTON", L"Process Blocker",
            WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
            10, 10, 250, 100, hwnd, 0, g_hInstance, NULL);

        CreateWindowEx(WS_EX_CLIENTEDGE, L"BUTTON", L"Edit Process Blocklist",
            WS_CHILD | WS_VISIBLE,
            20, 35, 150, 20, hwnd, (HMENU)EDIT_PROCESS_BUTTON, g_hInstance, NULL);

        //Web Block
        CreateWindowEx(0, L"BUTTON", L"Web Blocker",
            WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
            10, 120, 250, 100, hwnd, 0, g_hInstance, NULL);

        CreateWindowEx(WS_EX_CLIENTEDGE, L"BUTTON", L"Edit Web Blocklist",
            WS_CHILD | WS_VISIBLE,
            20, 145, 150, 20, hwnd, (HMENU)EDIT_WEB_BUTTON, g_hInstance, NULL);

        //Start Button
        CreateWindowEx(0, L"BUTTON", L"START",
            WS_CHILD | WS_VISIBLE,
            10, 230, 100, 35, hwnd, (HMENU)START_BUTTON, g_hInstance, NULL);

        //Timer Set
        CreateWindowEx(0, L"BUTTON", L"Timer Settings",
            WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
            270, 10, 250, 100, hwnd, 0, g_hInstance, NULL);

        break;
    }

    case WM_COMMAND: {
        switch (LOWORD(wParam)) {
        case START_BUTTON:
        {
            system("sc start AttentionService");
            MessageBox(hwnd, L"Blocking has started.", L"Info", MB_OK);
            break;
        }
        case EDIT_WEB_BUTTON:
        {
            ShellExecute(NULL, L"open", L"notepad.exe", L"C:\\ProgramData\\AttentionService\\host_file_edit.txt", NULL, SW_SHOW);
            MessageBox(hwnd, L"Edit Web Block clicked", L"Info", MB_OK);
            break;
        }
        case EDIT_PROCESS_BUTTON:
        {
            ShellExecute(NULL, L"open",L"notepad.exe", L"C:\\ProgramData\\AttentionService\\process_blocklist.txt", NULL, SW_SHOW);
            MessageBox(hwnd, L"Edit Process Block clicked", L"Info", MB_OK);
            break;
        }
        }
        break;
    }

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}