// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#pragma once
#include <concepts>
#include "String.hpp"

namespace Wcm
{
    [[nodiscard]] std::wstring GetDate();
    [[nodiscard]] std::wstring GetDesktopName();
    [[nodiscard]] std::wstring GetPCName();
    bool EnablePrivilegeValue([[maybe_unused]] LPCWSTR lpszPrivilege, bool bEnablePrivilege);
    [[nodiscard]] HANDLE OpenProcessFromID(DWORD processID);
    template <StringLike T>
    std::shared_ptr<PROCESS_INFORMATION> Execute(const T &app, const T &args = {});
    void CloseWindow(HWND hWnd);

    namespace Impl
    {
        template <Character T>
        [[nodiscard]] auto GetProcessInfo();
        std::shared_ptr<PROCESS_INFORMATION> CreateNewProcess(LPCWSTR app, LPWSTR args);
        std::shared_ptr<PROCESS_INFORMATION> CreateNewProcess(LPCSTR app, LPSTR args);
    }
}

#include "System.inl"
