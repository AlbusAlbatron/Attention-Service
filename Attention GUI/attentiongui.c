#ifndef UNICODE
#define UNICODE
#endif

#ifndef _UNICODE
#define _UNICODE
#endif

//Menu handle codes
#define START_BUTTON 600

#define EDIT_PROCESS_BUTTON 601

#define EDIT_WEB_BUTTON 602

#define TIMER_EDIT_BUTTON 603

#define CREATE_SERVICE_BUTTON 604

#define SERVICE_STATUS_TEXT 605

#define TIME_LEFT_TEXT 606

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#include "attention.h"
#include <stdio.h>
#include <windows.h>
#include <wchar.h>

HINSTANCE g_hInstance;

//Time left on service
int g_timerRemaining = 0;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    const wchar_t CLASS_NAME[] = L"Attention_Service";


    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    g_hInstance = hInstance;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"Attention Service",
        WS_OVERLAPPEDWINDOW,
        100, 100, 300, 400,
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

int timer_check(int* timer) {
    wchar_t buffer[BUFFER_SIZE];
    FILE* fp = _wfopen(ATTENTION_SERVICE_CONFIG, L"r");

    //Get time in minutes from config file
    while (fgetws(buffer, BUFFER_SIZE, fp)) {
        if (wcsstr(buffer, L"timer_minutes=")) {
            swscanf_s(buffer, L"timer_minutes=%d", timer);
        }
    }

    return 0;

}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        //_CrtDumpMemoryLeaks();
        KillTimer(hwnd, 1);
        KillTimer(hwnd, 2);
        PostQuitMessage(0);
        return 0;

    case WM_CREATE: {
        //Set timer to check service status every 2 seconds
        SetTimer(hwnd, 1, 2000, NULL);

        //Service status text
        CreateWindowEx(0, L"STATIC", L"Status: Stopped",
            WS_CHILD | WS_VISIBLE,
            20, 300, 150, 18, hwnd, (HMENU)SERVICE_STATUS_TEXT, g_hInstance, NULL);

        //Process Block
        CreateWindowEx(0, L"BUTTON", L"Edit Process Blocklist",
            WS_CHILD | WS_VISIBLE,
            20, 35, 150, 50, hwnd, (HMENU)EDIT_PROCESS_BUTTON, g_hInstance, NULL);

        //Web Block
        CreateWindowEx(0, L"BUTTON", L"Edit Web Blocklist",
            WS_CHILD | WS_VISIBLE,
            20, 95, 150, 50, hwnd, (HMENU)EDIT_WEB_BUTTON, g_hInstance, NULL);

        //Start Button
        CreateWindowEx(0, L"BUTTON", L"START BLOCK",
            WS_CHILD | WS_VISIBLE,
            20, 230, 200, 50, hwnd, (HMENU)START_BUTTON, g_hInstance, NULL);

        //Timer Set
        CreateWindowEx(0, L"BUTTON", L"Edit Timer",
            WS_CHILD | WS_VISIBLE,
            20, 155, 150, 50, hwnd, (HMENU)TIMER_EDIT_BUTTON, g_hInstance, NULL);

        //Create windows service
        CreateWindowEx(0, L"BUTTON", L"Install Service",
            WS_CHILD | WS_VISIBLE,
            0, 0, 100, 20, hwnd, (HMENU)CREATE_SERVICE_BUTTON, g_hInstance, NULL);

        //Time left
        CreateWindowEx(0, L"STATIC", L"Minutes Remaining: --",
            WS_CHILD | WS_VISIBLE,
            20, 320, 150, 18, hwnd, (HMENU)TIME_LEFT_TEXT, g_hInstance, NULL);

        //Check if service is running or stopped
        SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
        SC_HANDLE hService = OpenService(hSCManager, L"AttentionService", SERVICE_QUERY_STATUS);

        if (hService) {
            SERVICE_STATUS service_status;
            if (QueryServiceStatus(hService, &service_status)) {
                if (service_status.dwCurrentState == SERVICE_RUNNING) {
                    SetWindowText(GetDlgItem(hwnd, SERVICE_STATUS_TEXT), L"Status: Running");
                }
                else if (service_status.dwCurrentState == SERVICE_STOPPED) {
                    SetWindowText(GetDlgItem(hwnd, SERVICE_STATUS_TEXT), L"Status: Stopped");
                }
            }
            CloseServiceHandle(hService);
        }
        CloseServiceHandle(hSCManager);


        break;
    }

    case WM_COMMAND: {
        switch (LOWORD(wParam)) {
        case START_BUTTON:
        {
            //Check if service is running or stopped
            SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
            SC_HANDLE hService = OpenService(hSCManager, L"AttentionService", SERVICE_START);
            if (!hService) {
                DWORD error = GetLastError();
                if (error == ERROR_SERVICE_DOES_NOT_EXIST) {
                    MessageBox(NULL, L"Service is not installed. Please press install service.", L"Error", MB_OK);
                }
                else {
                    MessageBox(NULL, L"Failed to open service.", L"Error", MB_OK);
                }
            }
            else {
                if (!StartService(hService, 0, NULL)) {
                    DWORD error = GetLastError();
                    if (error == ERROR_SERVICE_ALREADY_RUNNING) {
                        MessageBox(NULL, L"Service is already running", L"Info", MB_OK);
                    }
                    else {
                        MessageBox(NULL, L"Failed to start service", L"Error", MB_OK);
                    }
                }
                else {
                    timer_check(&g_timerRemaining);

                    SetTimer(hwnd, 2, 60000, NULL);

                    wchar_t minutes_remaining[256];
                    swprintf(minutes_remaining, 256, L"Minutes Remaining: %d", g_timerRemaining);

                    SetWindowText(GetDlgItem(hwnd, TIME_LEFT_TEXT), minutes_remaining);

                    MessageBox(NULL, L"Service started!", L"Success", MB_OK);
                }
                CloseServiceHandle(hService);
            }
            CloseServiceHandle(hSCManager);
            
            break;
        }

        case EDIT_WEB_BUTTON:
        {
            //Check that files exist
            create_required_files();

            ShellExecute(NULL, L"open", L"notepad.exe", L"C:\\ProgramData\\AttentionService\\host_file_edit.txt", NULL, SW_SHOW);
            break;
        }
        case EDIT_PROCESS_BUTTON:
        {
            //Check that files exist
            create_required_files();

            ShellExecute(NULL, L"open",L"notepad.exe", L"C:\\ProgramData\\AttentionService\\process_blocklist.txt", NULL, SW_SHOW);
            break;
        }
        case TIMER_EDIT_BUTTON:
        {
            //Check that files exist
            create_required_files();

            ShellExecute(NULL, L"open", L"notepad.exe", L"C:\\ProgramData\\AttentionService\\cfg.txt", NULL, SW_SHOW);
            break;
        }

        case CREATE_SERVICE_BUTTON:
        {
            //Create programdata files
            create_required_files();

            //Get path of gui exe
            wchar_t exePath[MAX_PATH];
            wchar_t servicePath[MAX_PATH];

            GetModuleFileName(NULL, exePath, MAX_PATH);

            //Remove the gui exe and just have folder path
            wchar_t* lastSlash = wcsrchr(exePath, L'\\');
            if (lastSlash) {
                *lastSlash = L'\0';
            }

            // Build path to service exe
            swprintf(servicePath, MAX_PATH, L"%s\\AttService.exe", exePath);

            SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
            SC_HANDLE hService = CreateService(
                hSCManager,
                L"AttentionService",
                L"Attention Service",
                SERVICE_ALL_ACCESS,
                SERVICE_WIN32_OWN_PROCESS,
                SERVICE_DEMAND_START,
                SERVICE_ERROR_NORMAL,
                servicePath,
                NULL, NULL, NULL, NULL, NULL);

            if (hService == NULL) {
                DWORD error = GetLastError();

                if (error == ERROR_SERVICE_EXISTS) {
                    MessageBox(NULL, L"Service already exists.", L"Error", MB_OK);
                }
                else if (error == ERROR_ACCESS_DENIED) {
                    MessageBox(NULL, L"Administrator privileges required to install the service.", L"Error", MB_OK);
                }
                else {
                    MessageBox(NULL, L"Failed to install service.", L"Error", MB_OK);
                }
            }
            else {
                MessageBox(NULL, L"Service has been installed.", L"Success", MB_OK);
            }

            CloseServiceHandle(hService);
            CloseServiceHandle(hSCManager);

            break;
        }

        }
        break;
    }

    case WM_PAINT:
    {
        HDC hdc;
        PAINTSTRUCT ps;

        hdc = BeginPaint(hwnd, &ps);
        EndPaint(hwnd, &ps);
        break;
    }
    case WM_SIZE:
    {
        HDC hdc;
        PAINTSTRUCT ps;
        hdc = BeginPaint(hwnd, &ps);
        EndPaint(hwnd, &ps);
        break;
    }
    case WM_TIMER:
    {
        switch (wParam) {
            //Check if service is running or is stopped
            case 1:
            {
                SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
                SC_HANDLE hService = OpenService(hSCManager, L"AttentionService", SERVICE_QUERY_STATUS);

                if (hService) {
                    SERVICE_STATUS service_status;
                    if (QueryServiceStatus(hService, &service_status)) {
                        if (service_status.dwCurrentState == SERVICE_RUNNING) {
                            SetWindowText(GetDlgItem(hwnd, SERVICE_STATUS_TEXT), L"Status: Running");
                        }
                        else if (service_status.dwCurrentState == SERVICE_STOPPED) {
                            SetWindowText(GetDlgItem(hwnd, SERVICE_STATUS_TEXT), L"Status: Stopped");
                        }
                    }
                    CloseServiceHandle(hService);
                }
                CloseServiceHandle(hSCManager);
                break;
            }
            //Check and update time left on service
            case 2:
            {
                g_timerRemaining--;

                if (g_timerRemaining <= 0) {
                    SetWindowText(GetDlgItem(hwnd, TIME_LEFT_TEXT), L"Minutes Remaining: --");
                    KillTimer(hwnd, 2);
                    break;
                }

                wchar_t minutes_remaining[256];
                swprintf(minutes_remaining, 256, L"Minutes Remaining: %d", g_timerRemaining);

                SetWindowText(GetDlgItem(hwnd, TIME_LEFT_TEXT), minutes_remaining);
                break;
            }
        }
    }


    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}