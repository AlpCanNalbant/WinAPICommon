// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#pragma once
#include <concepts>
#include "SystemCommon.hpp"
#include "Log.hpp"

namespace Wcm
{
    bool EnablePrivilegeValue([[maybe_unused]] LPCWSTR lpszPrivilege, bool bEnablePrivilege);
    [[nodiscard]] HANDLE OpenProcessFromID(DWORD processID);
    template <StringLike T>
    std::shared_ptr<PROCESS_INFORMATION> Execute(const T &app, const T &args = {});
    template <StringLike T>
    std::shared_ptr<void> RunCommand(const T &command, HWND hWnd, bool runAsAdmin = false);
    template <StringLike T>
    std::shared_ptr<void> RunCommand(const T &command, bool runAsAdmin = false);
    [[nodiscard]] HWND FindHWND(std::string_view windowTitle);
    [[nodiscard]] HWND FindHWND(std::wstring_view windowTitle);

    namespace Impl
    {
        template <Character T>
        [[nodiscard]] auto GetProcessInfo();
        std::shared_ptr<PROCESS_INFORMATION> CreateNewProcess(LPCWSTR app, LPWSTR args);
        std::shared_ptr<PROCESS_INFORMATION> CreateNewProcess(LPCSTR app, LPSTR args);
    }
}

#include "System.inl"
