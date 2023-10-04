/*
// MIT License - Copyright (c) 2023 Alp Can Nalbant
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.
*/

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
