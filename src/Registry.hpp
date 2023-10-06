// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#pragma once

#include "RegistryKey.hpp"

namespace WinCmn
{
    [[nodiscard]] RegistryKey OpenRegistryKey(const RegistryKeyType keyType, const std::wstring &subKeyName);
    [[nodiscard]] HKEY ToKeyHandle(const RegistryKeyType branch);
}
