// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#pragma once
#include "RegistryKey.hpp"

namespace Wcm
{
    [[nodiscard]] RegistryKey OpenRegistryKey(const RegistryKeyType keyType, std::wstring_view subKeyName);
    [[nodiscard]] HKEY ToKeyHandle(const RegistryKeyType branch);
}
