// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#pragma once

#include <string>

namespace Wcm
{
    [[nodiscard]] std::wstring GetDate();
    [[nodiscard]] std::wstring GetDesktopName();
    [[nodiscard]] std::wstring GetPCName();
    bool EnablePrivilegeValue([[maybe_unused]] LPCTSTR lpszPrivilege, bool bEnablePrivilege);
    [[nodiscard]] HANDLE OpenProcessFromID(DWORD processID);
    void CloseWindow(HWND hWnd);
}
