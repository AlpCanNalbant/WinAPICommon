// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#include "WinAPICommon.hpp"

namespace WinCmn
{
    RegistryKey::RegistryKey(HKEY hKey, const RegistryKeyType type)
        : Type{type}, hKey_{hKey} {}

    RegistryKey::~RegistryKey()
    {
        if (IsOpen())
        {
            Close();
        }
    }

    bool RegistryKey::IsOpen() const
    {
        return hKey_ != nullptr;
    }

    bool RegistryKey::SetValue(const std::wstring &name, const RegistryValueType type, const BYTE *data, const DWORD dataSize) const
    {
        const auto isSuccess = RegSetValueExW(hKey_, name.c_str(), 0, static_cast<DWORD>(type), data, dataSize);
        if (isSuccess != ERROR_SUCCESS)
        {
            Log->Error(L"Value of registry key could not be set. Name is " + name, GetLastError());
            return false;
        }
        return true;
    }

    bool RegistryKey::SetEventLogType(const EventLogType typeData, const std::wstring &name, const RegistryValueType regType) const
    {
        return SetValue(name, regType, reinterpret_cast<LPCBYTE>(&typeData), sizeof(typeData));
    }

    bool RegistryKey::Close()
    {
        const auto isSuccess = RegCloseKey(hKey_);
        if (isSuccess != ERROR_SUCCESS)
        {
            Log->Error(L"Registry key could not be closed.", GetLastError());
            return false;
        }
        hKey_ = nullptr;
        return true;
    }

    RegistryKey::operator HKEY() const noexcept
    {
        return hKey_;
    }
}
