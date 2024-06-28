// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#include "WinAPICommon.hpp"

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

    namespace Impl
    {
        std::shared_ptr<PROCESS_INFORMATION> CreateNewProcess(LPCWSTR app, LPWSTR args)
        {
            if (const auto procInfo = GetProcessInfo<WCHAR>(); CreateProcessW(app, args, nullptr, nullptr, FALSE, 0, nullptr, nullptr, const_cast<LPSTARTUPINFOW>(&procInfo.second), procInfo.first.get()))
            {
                return procInfo.first;
            }
            Wcm::Log->Error("Creation of process is failed.", GetLastError()).Sub("Process", app);
            return nullptr;
        }

        std::shared_ptr<PROCESS_INFORMATION> CreateNewProcess(LPCSTR app, LPSTR args)
        {
            if (const auto procInfo = GetProcessInfo<CHAR>(); CreateProcessA(app, args, nullptr, nullptr, FALSE, 0, nullptr, nullptr, const_cast<LPSTARTUPINFOA>(&procInfo.second), procInfo.first.get()))
            {
                return procInfo.first;
            }
            Wcm::Log->Error("Creation of process is failed.", GetLastError()).Sub("Process", app);
            return nullptr;
        }
    }
}
