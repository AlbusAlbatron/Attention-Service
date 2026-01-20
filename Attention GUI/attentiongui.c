#ifndef UNICODE
#define UNICODE
#endif

#ifndef _UNICODE
#define _UNICODE
#endif

//Making it look like windows95
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

#ifndef DWMWA_CAPTION_COLOR
#define DWMWA_CAPTION_COLOR 35
#endif

//Define Menu handle codes
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
#include <Uxtheme.h>
#include "attention.h"
#include <commctrl.h>

//Make hInstance global
HINSTANCE g_hInstance;

//Win95 Grey colour
HBRUSH g_hWin95Grey;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

BOOL CALLBACK ApplyClassicStyle(HWND child, LPARAM hFont) {
    wchar_t buffer[32];

    // 1. Strip modern theme
    SetWindowTheme(child, L"", L"");

    // 2. Set the classic font
    if (hFont != 0) {
        SendMessage(child, WM_SETFONT, (WPARAM)hFont, TRUE);
    }

    // 3. If it's an Edit box, force the 3D frame to recalculate
    if (GetClassName(child, buffer, 32) && lstrcmpi(buffer, L"EDIT") == 0) {
        // SWP_FRAMECHANGED tells the window to re-draw its borders from scratch
        SetWindowPos(child, NULL, 0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
    }

    return TRUE;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    // Register the window class.
    const wchar_t CLASS_NAME[] = L"Attention_Service2";

    WNDCLASSEX wc = { sizeof(WNDCLASSEX) };

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.style = CS_HREDRAW | CS_VREDRAW;

    g_hInstance = hInstance;
    g_hWin95Grey = CreateSolidBrush(RGB(192, 192, 192));

    RegisterClassEx(&wc);

    // Create the window.

    HWND hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"Attention Service",    // Window text
        WS_POPUP | WS_VISIBLE | WS_SYSMENU,            // Window style

        // Size and position
        100, 100, 900, 700,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );


    if (hwnd == NULL)
    {
        return 0;
    }

    SetWindowTheme(hwnd, L"", L"");

    ShowWindow(hwnd, nCmdShow);


    // Run the message loop.
    MSG msg = { sizeof(MSG) };
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
        //Getting window dimensions
        RECT rc;
        GetClientRect(hwnd, &rc);
        int window_width = rc.right;

        
        //Classic Font
        HFONT hFont = CreateFont(-11, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY, VARIABLE_PITCH | FF_SWISS, L"MS Sans Serif");

        //Status bar
        HWND hStatus = CreateWindowEx(WS_EX_STATICEDGE, STATUSCLASSNAME, NULL,
            WS_CHILD | WS_VISIBLE,
            0, 0, 0, 0, hwnd, (HMENU)999, g_hInstance, NULL);

        //Close window button
        CreateWindowEx(0, L"BUTTON", L"r", // 'r' in Marlett font is the Close X
            WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
            window_width - 22, 5, 16, 14, hwnd, (HMENU)IDCLOSE, g_hInstance, NULL);

        //Process Block
        CreateWindowEx(0, L"BUTTON", L"Process Settings",
            WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
            10, 35, 250, 100, hwnd, 0, g_hInstance, NULL);

        CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
            20, 60, 150, 20, hwnd, (HMENU)PROCESS_INPUT_AREA, g_hInstance, NULL);

        CreateWindowEx(0, L"BUTTON", L"Add",
            WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
            20, 82, 60, 23, hwnd, (HMENU)ADD_PROCESS_BUTTON, g_hInstance, NULL);
        
        CreateWindowEx(0, L"BUTTON", L"Remove",
            WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
            83, 82, 60, 23, hwnd, (HMENU)REMOVE_PROCESS_BUTTON, g_hInstance, NULL);

        //Web Block
        CreateWindowEx(0, L"BUTTON", L"Web Settings",
            WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
            10, 145, 250, 100, hwnd, 0, g_hInstance, NULL);

        CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
            20, 170, 150, 20, hwnd, (HMENU)WEB_INPUT_AREA, g_hInstance, NULL);

        CreateWindowEx(0, L"BUTTON", L"Add",
            WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
            20, 192, 60, 23, hwnd, (HMENU)ADD_WEB_BUTTON, g_hInstance, NULL);        
        
        CreateWindowExW(0, L"BUTTON", L"REMOVE",
            WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
            83, 192, 60, 23, hwnd, (HMENU)REMOVE_WEB_BUTTON, g_hInstance, NULL);

        //Start
        CreateWindowEx(0, L"BUTTON", L"START",
            WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
            10, 255, 100, 32, hwnd, (HMENU)START_BUTTON, g_hInstance, NULL);
        
        //Timer

        //Apply style
        EnumChildWindows(hwnd, ApplyClassicStyle, (LPARAM)hFont);

        break;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT rc;
        GetClientRect(hwnd, &rc);

        // 1. Draw the Main Background (Win95 Grey)
        HBRUSH hbgBrush = CreateSolidBrush(RGB(192, 192, 192));
        FillRect(hdc, &ps.rcPaint, hbgBrush);
        DeleteObject(hbgBrush);

        // 2. Draw the Navy Blue Title Bar (Height: 18-20 pixels)
        RECT rcTitle = { rc.left + 2, rc.top + 2, rc.right - 2, rc.top + 20 };
        HBRUSH hTitleBrush = CreateSolidBrush(RGB(0, 0, 128));
        FillRect(hdc, &rcTitle, hTitleBrush);
        DeleteObject(hTitleBrush);

        // 3. Draw the Title Text (White and Bold)
        SetTextColor(hdc, RGB(255, 255, 255));
        SetBkMode(hdc, TRANSPARENT);

        // Use a small padding for the text
        rcTitle.left += 5;
        DrawText(hdc, L"Attention Service", -1, &rcTitle, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

        // 4. Draw the Outer 3D Border for the whole window
        // This gives the window that "thick" mechanical frame
        DrawEdge(hdc, &rc, EDGE_RAISED, BF_RECT);

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
        if (LOWORD(wParam) == IDCLOSE) {
            PostQuitMessage(0);
        }
        break;
    }

    case WM_SIZE: {
        //Resize status bar
        HWND hStatus = GetDlgItem(hwnd, 999);
        SendMessage(hStatus, WM_SIZE, 0, 0);
        break;
    }
    //Making the background grey
    case WM_ERASEBKGND:
    {
        HDC hdc = (HDC)wParam;
        RECT rect;
        GetClientRect(hwnd, &rect);

        //Makes background grey for win95 look
        HBRUSH hBrush = CreateSolidBrush(RGB(192, 192, 192));
        FillRect(hdc, &rect, hBrush);
        DeleteObject(hBrush);

        return (INT_PTR)g_hWin95Grey;
    }
    case WM_CTLCOLORDLG: {
        return (INT_PTR)g_hWin95Grey;;
    }
    case WM_CTLCOLORBTN:
    case WM_CTLCOLORSTATIC:
    {
        HDC hdc = (HDC)wParam;

        // 1. Set the background mode of the text to transparent 
        // so the grey shows through behind the letters.
        SetBkMode(hdc, TRANSPARENT);

        // 2. Set the text color to standard Windows black.
        SetTextColor(hdc, GetSysColor(COLOR_BTNTEXT));

        // 3. Return the grey brush. 
        // We use a static brush so we don't leak memory by creating a new one every second.
        static HBRUSH hBtnFaceBrush = NULL;
        if (hBtnFaceBrush == NULL) {
            hBtnFaceBrush = CreateSolidBrush(RGB(192, 192, 192));
        }

        return (INT_PTR)hBtnFaceBrush;
    }
    case WM_DRAWITEM:
    {
        LPDRAWITEMSTRUCT pdis = (LPDRAWITEMSTRUCT)lParam;
        HDC hdc = pdis->hDC;
        RECT rc = pdis->rcItem;
        wchar_t buffer[32];

        // 1. Draw the Base (Grey Face)
        HBRUSH hFace = CreateSolidBrush(RGB(192, 192, 192));
        FillRect(hdc, &rc, hFace);
        DeleteObject(hFace);

        // 2. Draw Borders
        if (pdis->CtlID == START_BUTTON) {
            // Default button black ring
            HPEN hPenBlack = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
            HGDIOBJ hOldPen = SelectObject(hdc, hPenBlack);
            Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);
            InflateRect(&rc, -1, -1); // Move inside the ring
            SelectObject(hdc, hOldPen);
            DeleteObject(hPenBlack);
        }

        // Draw the 3D edges (Raised or Sunken)
        DrawEdge(hdc, &rc, (pdis->itemState & ODS_SELECTED) ? EDGE_SUNKEN : EDGE_RAISED, BF_RECT);

        // 3. Draw Content (Text or Icon)
        SetBkMode(hdc, TRANSPARENT);
        if (pdis->itemState & ODS_SELECTED) OffsetRect(&rc, 1, 1);

        if (pdis->CtlID == IDCLOSE) {
            // Special Marlett font for Close Button
            HFONT hMarlett = CreateFont(13, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                SYMBOL_CHARSET, 0, 0, 0, 0, L"Marlett");
            HGDIOBJ hOld = SelectObject(hdc, hMarlett);
            DrawText(hdc, L"r", -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            SelectObject(hdc, hOld);
            DeleteObject(hMarlett);
        }
        else {
            // Regular text for START and other buttons
            GetWindowText(pdis->hwndItem, buffer, 32);
            DrawText(hdc, buffer, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }

        // 4. Draw Focus Rect (The dots)
        if (pdis->itemState & ODS_FOCUS && pdis->CtlID != IDCLOSE) {
            RECT rcFocus = rc;
            InflateRect(&rcFocus, -4, -4);
            DrawFocusRect(hdc, &rcFocus);
        }

        return TRUE;
    }
    case WM_NCHITTEST: {
        LRESULT hit = DefWindowProc(hwnd, uMsg, wParam, lParam);
        if (hit == HTCLIENT) {
            POINT pt = { LOWORD(lParam), HIWORD(lParam) };
            ScreenToClient(hwnd, &pt);
            // If the user clicks in our custom blue bar area (top 20 pixels)
            if (pt.y < 20) return HTCAPTION;
        }
        return hit;
    }
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}