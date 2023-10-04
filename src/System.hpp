/*
// MIT License - Copyright (c) 2023 Alp Can Nalbant
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.
*/

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
