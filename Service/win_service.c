#define UNICODE
#define _UNICODE

include <windows.h>
#include <stdio.h>

// Service name
#define SERVICE_NAME L"AttentionService"

// Global variables
SERVICE_STATUS g_ServiceStatus = { 0 };
SERVICE_STATUS_HANDLE g_StatusHandle = NULL;
HANDLE g_ServiceStopEvent = INVALID_HANDLE_VALUE;

// Function declarations
VOID WINAPI ServiceMain(DWORD argc, LPTSTR* argv);
VOID WINAPI ServiceCtrlHandler(DWORD CtrlCode);
DWORD WINAPI ServiceWorkerThread(LPVOID lpParam);

// Entry point - using wmain for Unicode Windows
int wmain(int argc, wchar_t* argv[]) {
    // Define the service
    SERVICE_TABLE_ENTRY ServiceTable[] = {
        {SERVICE_NAME, (LPSERVICE_MAIN_FUNCTION)ServiceMain},
        {NULL, NULL}
    };

    // Start the service control dispatcher
    // This connects your program to the Windows Service Control Manager
    if (StartServiceCtrlDispatcher(ServiceTable) == FALSE) {
        return GetLastError();
    }

    return 0;
}

// This is called when the service starts
VOID WINAPI ServiceMain(DWORD argc, LPTSTR* argv) {
    // Register the control handler (for stop/start commands)
    g_StatusHandle = RegisterServiceCtrlHandler(SERVICE_NAME, ServiceCtrlHandler);

    if (g_StatusHandle == NULL) {
        return;
    }

    // Set up the service status structure
    g_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    g_ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
    g_ServiceStatus.dwControlsAccepted = 0;
    g_ServiceStatus.dwWin32ExitCode = 0;
    g_ServiceStatus.dwServiceSpecificExitCode = 0;
    g_ServiceStatus.dwCheckPoint = 0;

    // Tell Windows we're starting
    SetServiceStatus(g_StatusHandle, &g_ServiceStatus);

    // Create stop event (to signal when to stop)
    g_ServiceStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (g_ServiceStopEvent == NULL) {
        g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
        g_ServiceStatus.dwWin32ExitCode = GetLastError();
        SetServiceStatus(g_StatusHandle, &g_ServiceStatus);
        return;
    }

    // Tell Windows we're now running
    g_ServiceStatus.dwCurrentState = SERVICE_RUNNING;
    g_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    SetServiceStatus(g_StatusHandle, &g_ServiceStatus);

    // Start the worker thread (this is where your actual work happens)
    HANDLE hThread = CreateThread(NULL, 0, ServiceWorkerThread, NULL, 0, NULL);

    // Wait until the service is told to stop
    WaitForSingleObject(g_ServiceStopEvent, INFINITE);

    // Clean up
    CloseHandle(hThread);
    CloseHandle(g_ServiceStopEvent);

    // Tell Windows we've stopped
    g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
    SetServiceStatus(g_StatusHandle, &g_ServiceStatus);
}

// This handles stop/start commands from Windows
VOID WINAPI ServiceCtrlHandler(DWORD CtrlCode) {
    switch (CtrlCode) {
    case SERVICE_CONTROL_STOP:
        if (g_ServiceStatus.dwCurrentState != SERVICE_RUNNING)
            break;

        // Tell Windows we're stopping
        g_ServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
        SetServiceStatus(g_StatusHandle, &g_ServiceStatus);

        // Signal the worker thread to stop
        SetEvent(g_ServiceStopEvent);
        break;

    default:
        break;
    }
}

// THIS IS WHERE YOUR CODE GOES
// This function runs continuously while the service is active
DWORD WINAPI ServiceWorkerThread(LPVOID lpParam) {
    // This loop runs until the service is stopped
    while (WaitForSingleObject(g_ServiceStopEvent, 0) != WAIT_OBJECT_0) {

        // PUT YOUR BLOCKING/MONITORING CODE HERE
        // For example:
        // - Check for blocked processes
        // - Monitor websites
        // - Check timers

        // For now, just sleep for 1 second between checks
        Sleep(1000);
    }

    return ERROR_SUCCESS;
}