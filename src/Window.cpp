// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#include "WinAPICommon.hpp"

namespace WinCmn
{
    void SafeClose(HWND hWnd)
    {
        SendMessageW(hWnd, WM_CLOSE, 0, 0);

        DWORD accessDeniedErrCode = 5;
        if (GetLastError() == accessDeniedErrCode)
        {
            WinCmn::Log->Error(L"Failed to send close message.", accessDeniedErrCode);
        }
    }
}
