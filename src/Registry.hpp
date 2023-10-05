// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

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
