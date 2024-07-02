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
        // Do not bload the log messages with this info.
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
