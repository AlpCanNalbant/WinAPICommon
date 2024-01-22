// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#pragma once
#include "Log.hpp"
#include "RegistryKeyType.hpp"
#include "RegistryValueType.hpp"
#include "EventLogType.hpp"

namespace Wcm
{
    struct RegistryKey
    {
        RegistryKeyType Type;

        RegistryKey(HKEY hKey, const RegistryKeyType type);
        RegistryKey(const RegistryKey &) = default;
        RegistryKey(RegistryKey &&) = default;
        ~RegistryKey();
        [[nodiscard]] bool IsOpen() const;
        bool SetValue(std::wstring_view name, const RegistryValueType type, const BYTE *data, const DWORD dataSize) const;
        bool SetEventLogType(const EventLogType typeData, std::wstring_view name = L"TypesSupported", const RegistryValueType regType = RegistryValueType::DWord) const;
        template <Character T = wchar_t>
        bool SetString(std::basic_string_view<std::remove_pointer_t<T>> name, const RegistryStringType type, const T *stringData) const;
        bool Close();

        RegistryKey &operator=(const RegistryKey &) = default;
        RegistryKey &operator=(RegistryKey &&) = default;
        [[nodiscard]] explicit operator HKEY() const noexcept;

    private:
        HKEY hKey_;
    };
}

#include "RegistryKey.inl"
