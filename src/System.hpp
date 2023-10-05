// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#pragma once

#include <string>
#include <windows.h>

#include "IModule.hpp"

namespace WCmn::Modules
{
    class System : public IModule
    {
    public:
        [[nodiscard]] std::wstring GetDesktopName() const;
        [[nodiscard]] std::wstring GetPCName() const;
        bool EnablePrivilegeValue([[maybe_unused]] LPCTSTR lpszPrivilege, bool bEnablePrivilege) const;
        [[nodiscard]] HANDLE OpenProcessFromID(DWORD processID) const;
    };
}
