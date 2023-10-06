// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#pragma once

#include "Log.hpp"
#include "Concepts.hpp"
#include "RegistryKeyType.hpp"
#include "RegistryValueType.hpp"
#include "EventLogType.hpp"

namespace WinCmn
{
    struct RegistryKey
    {
        RegistryKeyType Type;

        RegistryKey(HKEY hKey, const RegistryKeyType type);
        ~RegistryKey();
        [[nodiscard]] bool IsOpen() const;
        bool SetValue(const std::wstring &name, const RegistryValueType type, const BYTE *data, const DWORD dataSize) const;
        bool SetEventLogType(const EventLogType typeData, const std::wstring &name = L"TypesSupported", const RegistryValueType regType = RegistryValueType::DWord) const;
        template <Character T = wchar_t>
        bool SetString(const std::basic_string<std::remove_pointer_t<T>> &name, const RegistryStringType type, const T *stringData) const;
        bool Close();

        [[nodiscard]] explicit operator HKEY() const noexcept;

    private:
        HKEY hKey_;
    };
}

#include "RegistryKey.inl"
