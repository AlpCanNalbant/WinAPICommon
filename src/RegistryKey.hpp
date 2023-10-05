// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#pragma once

#include <windows.h>

#include "Log.hpp"
#include "StringLoopTerminator.hpp"
#include "RegistryKeyType.hpp"
#include "RegistryValueType.hpp"
#include "EventLogType.hpp"

namespace WCmn
{
    struct RegistryKey
    {
        RegistryKeyType Type;

        RegistryKey(HKEY hKey, const RegistryKeyType type);
        ~RegistryKey();
        [[nodiscard]] bool IsOpen() const;
        bool SetValue(const std::wstring &name, const RegistryValueType type, const BYTE *data, const DWORD dataSize) const;
        bool SetEventLogType(const EventLogType type, const std::wstring &name = L"TypesSupported", const RegistryValueType regType = RegistryValueType::DWord) const;
        template <Character TChr = wchar_t>
        bool SetString(const std::basic_string<std::remove_pointer_t<TChr>> &name, const RegistryStringType type, const TChr *stringData) const;
        bool Close();

        [[nodiscard]] explicit operator HKEY() const noexcept;

    private:
        HKEY hKey_;
    };
}

#include "RegistryKey.inl"
