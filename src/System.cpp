// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#include "WinAPICommon.hpp"

BOOL CALLBACK EnumWindowCallback(HWND, LPARAM);
HWND foundhWnd = NULL;
wchar_t const *windowTitleToFind = nullptr;

namespace Wcm
{
    bool EnablePrivilegeValue(LPCWSTR lpszPrivilege, bool bEnablePrivilege)
    {
        HANDLE hToken;
        LUID luid;
        TOKEN_PRIVILEGES tp;
        bool isSuccess = true;

        if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
        {
            Log->Error("OpenProcessToken error.", GetLastError());
            isSuccess = false;
        }

        if (!LookupPrivilegeValueW(NULL, lpszPrivilege, &luid)) // SE_DEBUG_NAME
        {
            Log->Error("LookupPrivilegeValue error.", GetLastError());
            isSuccess = false;
        }

        tp.PrivilegeCount = 1;
        tp.Privileges[0].Luid = luid;
        tp.Privileges[0].Attributes = bEnablePrivilege ? SE_PRIVILEGE_ENABLED : 0;

        if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES)NULL, (PDWORD)NULL))
        {
            Log->Error("AdjustTokenPrivileges error.", GetLastError());
            isSuccess = false;
        }

        if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)
        {
            Log->Error("The token does not have the specified privilege.", ERROR_NOT_ALL_ASSIGNED);
            isSuccess = false;
        }
        if (hToken)
        {
            CloseHandle(hToken);
        }

        return isSuccess;
    }

    HANDLE OpenProcessFromID(DWORD processID)
    {
        if (!EnablePrivilegeValue(SE_DEBUG_NAME, true))
        {
            Log->Error("Failed to obtain required privileges for openning the process.");
        }

        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, processID);
        if (hProcess == NULL)
        {
            Log->Error("Failed to open process.", GetLastError());
            return NULL;
        }

        return hProcess;
    }

    HWND FindHWND(std::string_view windowTitle)
    {
        return FindHWND(Wcm::ToWStringIf(windowTitle));
    }
    HWND FindHWND(std::wstring_view windowTitle)
    {
        foundhWnd = nullptr;
        windowTitleToFind = windowTitle.data();
        EnumWindows(EnumWindowCallback, 0);
        return foundhWnd;
    }

    DWORD GetCurrentSessionId()
    {
        WTS_SESSION_INFO *pSessionInfo;
        DWORD dwSessions = 0;
        if (!WTSEnumerateSessions(WTS_CURRENT_SERVER, 0, 1, &pSessionInfo, &dwSessions))
        {
            Wcm::Log->Error("Enumerating sessions failed.", GetLastError());
            return 0;
        }
        DWORD sessionId = 0;
        for (DWORD i = 0; i < dwSessions; ++i)
        {
            if (pSessionInfo[i].State == WTSActive)
            {
                sessionId = pSessionInfo[i].SessionId;
                break;
            }
        }
        WTSFreeMemory(pSessionInfo);
        return sessionId;
    }

    bool BringWindowToTop(HWND hWnd, bool keepTopmost)
    {
        return BringWindowToTop(hWnd, GetCurrentProcessId(), keepTopmost);
    }

    bool BringWindowToTop(HWND hWnd, DWORD dwProcessId, bool keepTopmost)
    {
        ShowWindow(hWnd, SW_SHOW);
        if (!SetFocus(hWnd))
        {
            Log->Error(L"Setting keyboard focus to the specified window is failed.", GetLastError());
        }
        {
            DWORD dwFailCount = 2;
            if (!SetWindowPos(hWnd, (!keepTopmost) ? HWND_TOP : HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSENDCHANGING | SWP_NOSIZE | SWP_SHOWWINDOW))
            {
                Log->Error(L"Setting window position is failed.", GetLastError()).Sub(L"IsKeepTopmostEnabled", keepTopmost ? L"True" : L"False");
                --dwFailCount;
                if (keepTopmost)
                {
                    return false;
                }
            }
            if (!::BringWindowToTop(hWnd))
            {
                Log->Error(L"Bringing the window to the top of the Z order is failed.", GetLastError());
                --dwFailCount;
                if (dwFailCount < 1)
                {
                    return false;
                }
            }
        }
        if (!AllowSetForegroundWindow(dwProcessId))
        {
            Log->Error(L"Enabling the specified process to set the foreground window is failed. This error can be ignored, the process may already be able to set the foreground window.", GetLastError()).Sub("ProcessID", std::to_string(dwProcessId));
        }
        if (!SetForegroundWindow(hWnd))
        {
            Log->Error(L"Window could not be brought to the foreground.", GetLastError());
        }
        return true;
    }

    bool TerminateProcessFromHwnd(HWND hWnd)
    {
        DWORD procId;
        if (!GetWindowThreadProcessId(hWnd, &procId))
        {
            Wcm::Log->Error(L"Process id cannot obtained.", GetLastError());
            return false;
        }
        HANDLE hProc = NULL;
        if ((hProc = OpenProcess(PROCESS_TERMINATE, TRUE, procId)) == NULL)
        {
            Wcm::Log->Error(L"Process handle cannot obtained.", GetLastError());
            return false;
        }
        DWORD exitCode;
        if (!GetExitCodeProcess(hProc, &exitCode))
        {
            Wcm::Log->Error(L"Process exit code cannot obtained.", GetLastError());
            exitCode = 123;
        }
        if (!TerminateProcess(hProc, exitCode))
        {
            Wcm::Log->Error(L"Process cannot terminated.", GetLastError());
            return false;
        }
        return true;
    }

    namespace Impl
    {
        std::shared_ptr<PROCESS_INFORMATION> CreateNewProcess(LPCWSTR app, LPWSTR args, DWORD creationFlags)
        {
            if (const auto procInfo = GetProcessInfo<WCHAR>(); CreateProcessW(app, args, nullptr, nullptr, FALSE, creationFlags, nullptr, nullptr, const_cast<LPSTARTUPINFOW>(&procInfo.second), procInfo.first.get()))
            {
                return procInfo.first;
            }
            Wcm::Log->Error("Creation of process is failed.", GetLastError()).Sub("Process", app);
            return nullptr;
        }

        std::shared_ptr<PROCESS_INFORMATION> CreateNewProcess(LPCSTR app, LPSTR args, DWORD creationFlags)
        {
            if (const auto procInfo = GetProcessInfo<CHAR>(); CreateProcessA(app, args, nullptr, nullptr, FALSE, creationFlags, nullptr, nullptr, const_cast<LPSTARTUPINFOA>(&procInfo.second), procInfo.first.get()))
            {
                return procInfo.first;
            }
            Wcm::Log->Error("Creation of process is failed.", GetLastError()).Sub("Process", app);
            return nullptr;
        }

        std::shared_ptr<PROCESS_INFORMATION> CreateNewProcess(HANDLE hToken, LPCWSTR app, LPWSTR args, std::wstring_view desktop, DWORD creationFlags)
        {
            if (const auto procInfo = GetProcessInfo<WCHAR>(desktop); CreateProcessAsUserW(hToken, app, args, nullptr, nullptr, FALSE, creationFlags, nullptr, nullptr, const_cast<LPSTARTUPINFOW>(&procInfo.second), procInfo.first.get()))
            {
                return procInfo.first;
            }
            Wcm::Log->Error("Creation of process is failed.", GetLastError()).Sub("Process", app);
            return nullptr;
        }

        std::shared_ptr<PROCESS_INFORMATION> CreateNewProcess(HANDLE hToken, LPCSTR app, LPSTR args, std::string_view desktop, DWORD creationFlags)
        {
            if (const auto procInfo = GetProcessInfo<CHAR>(desktop); CreateProcessAsUserA(hToken, app, args, nullptr, nullptr, FALSE, creationFlags, nullptr, nullptr, const_cast<LPSTARTUPINFOA>(&procInfo.second), procInfo.first.get()))
            {
                return procInfo.first;
            }
            Wcm::Log->Error("Creation of process is failed.", GetLastError()).Sub("Process", app);
            return nullptr;
        }
    }
}

BOOL CALLBACK EnumWindowCallback(HWND hWnd, [[maybe_unused]] LPARAM lparam)
{
    if (foundhWnd)
    {
        return FALSE;
    }

    int length = GetWindowTextLengthW(hWnd);
    if (length > 0)
    {
        WCHAR buffer[length + 1] = {'\0'};
        GetWindowTextW(hWnd, buffer, length + 1);
        std::wstring windowTitle{buffer};
        // Do not bloat the log messages with this info.
        // Wcm::Log->Info(std::wstring{L"Name of the current window handle is "} + buffer + L'.');
        if (windowTitle.find(windowTitleToFind) != std::wstring::npos)
        {
            foundhWnd = hWnd;
            Wcm::Log->Info(L"Window handle of the process is found!").Sub("WindowTitle", windowTitle);
            return FALSE;
        }
    }

    return TRUE;
}
