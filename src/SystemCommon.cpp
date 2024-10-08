// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#include "Log.hpp"

namespace Wcm
{
    std::wstring GetDate()
    {
        const auto now = std::chrono::system_clock::now();
        const std::time_t t_c = std::chrono::system_clock::to_time_t(now);
        const std::wstring dateStr{_wctime64(&t_c)};
        return dateStr.substr(0, dateStr.find_last_of(L'\n'));
    }

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
            Log->Error("Failed to retrieving the computer name.", GetLastError());
        }
        return {lpBuffer};
    }

    HWND GetParentOr(HWND hWndOwner, HWND hWndDefault)
    {
        if (HWND hWndParent = GetParent(hWndOwner); hWndParent != NULL && hWndParent != hWndOwner)
        {
            return hWndParent;
        }
        return hWndDefault;
    }

    void CloseWindow(HWND hWnd)
    {
        SendMessageW(hWnd, WM_CLOSE, 0, 0);

        DWORD accessDeniedErrCode = 5;
        if (GetLastError() == accessDeniedErrCode)
        {
            Log->Error("Failed to send close message.", accessDeniedErrCode);
        }
    }
}
