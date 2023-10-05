// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#include "Window.hpp"
#include "WinAPICommon.hpp"

namespace WCmn::Modules
{
    void Window::Close(HWND hWnd) const
    {
        SendMessageW(hWnd, WM_CLOSE, 0, 0);

        DWORD accessDeniedErrCode = 5;
        if (GetLastError() == accessDeniedErrCode)
        {
            WCmn::Log->Error(L"Failed to send close message.", accessDeniedErrCode);
        }
    }
}
