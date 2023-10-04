/*
// MIT License - Copyright (c) 2023 Alp Can Nalbant
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.
*/

#pragma once

#include <windows.h>
#include <string>

#include "IModule.hpp"
#include "RegistryKey.hpp"

namespace WCmn::Modules
{
    class Registry : public IModule
    {
    public:
        [[nodiscard]] RegistryKey OpenKey(const RegistryKeyType keyType, const std::wstring &subKeyName) const;

    private:
        [[nodiscard]] HKEY ToKeyHandle(const RegistryKeyType branch) const;
    };
}
