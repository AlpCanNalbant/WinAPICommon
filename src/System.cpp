// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#include "System.hpp"
#include "WinAPICommon.hpp"

namespace WinCmn
{
    std::wstring GetDesktopName()
    {
        std::wstring deskName = L"default";
        HDESK inputDesktop = OpenInputDesktop(0, false, GENERIC_ALL);

        WCHAR buffer[256] = {'\0'};
        DWORD lengthNeeded;
        if (GetUserObjectInformationW(inputDesktop, UOI_NAME, buffer, sizeof(buffer), &lengthNeeded))
        {
            deskName = {buffer};
        }

        CloseDesktop(inputDesktop);
        return deskName;
    }

    std::wstring GetPCName()
    {
        WCHAR lpBuffer[256] = {'\0'};
        DWORD nSize = sizeof(lpBuffer);
        if (!GetComputerNameW(lpBuffer, &nSize))
        {
            WinCmn::Log->Error(L"Failed to retrieving the computer name.", GetLastError());
        }
        return {lpBuffer};
    }

    bool EnablePrivilegeValue(LPCTSTR lpszPrivilege, bool bEnablePrivilege)
    {
        HANDLE hToken;
        LUID luid;
        TOKEN_PRIVILEGES tp;
        bool isSuccess = true;

        if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
        {
            WinCmn::Log->Error(L"OpenProcessToken error.", GetLastError());
            isSuccess = false;
        }

        if (!LookupPrivilegeValueW(NULL, lpszPrivilege, &luid)) // SE_DEBUG_NAME
        {
            WinCmn::Log->Error(L"LookupPrivilegeValue error.", GetLastError());
            isSuccess = false;
        }

        tp.PrivilegeCount = 1;
        tp.Privileges[0].Luid = luid;
        tp.Privileges[0].Attributes = bEnablePrivilege ? SE_PRIVILEGE_ENABLED : 0;

        if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES)NULL, (PDWORD)NULL))
        {
            WinCmn::Log->Error(L"AdjustTokenPrivileges error.", GetLastError());
            isSuccess = false;
        }

        if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)
        {
            WinCmn::Log->Error(L"The token does not have the specified privilege.", ERROR_NOT_ALL_ASSIGNED);
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
        if (EnablePrivilegeValue(SE_DEBUG_NAME, true))
        {
            WinCmn::Log->Error(L"Failed to obtain required privileges for openning the process.");
        }

        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, processID);
        if (hProcess == NULL)
        {
            WinCmn::Log->Error(L"Failed to open process.", GetLastError());
            return NULL;
        }

        return hProcess;
    }
}
