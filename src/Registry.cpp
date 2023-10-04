/*
// MIT License - Copyright (c) 2023 Alp Can Nalbant
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.
*/

#include "Registry.hpp"
#include "Log.hpp"

namespace WCmn::Modules
{
    RegistryKey Registry::OpenKey(const RegistryKeyType keyType, const std::wstring &subKeyName) const
    {
        HKEY hKey = ToKeyHandle(keyType);
        LPCWSTR lpSubKey = subKeyName.c_str();

        const auto isSuccess = RegCreateKeyExW(hKey, lpSubKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL);
        if (isSuccess != ERROR_SUCCESS)
        {
            Log->Error(L"Registry key could not be opened.", GetLastError());
            return {nullptr, keyType};
        }
        return {hKey, keyType};
    }

    HKEY Registry::ToKeyHandle(const RegistryKeyType branch) const
    {
        switch (branch)
        {
        case RegistryKeyType::ClassesRoot:
            return HKEY_CLASSES_ROOT;
        case RegistryKeyType::CurrentConfig:
            return HKEY_CURRENT_CONFIG;
        case RegistryKeyType::CurrentUser:
            return HKEY_CURRENT_USER;
        case RegistryKeyType::LocalMachine:
            return HKEY_LOCAL_MACHINE;
        case RegistryKeyType::Users:
            return HKEY_USERS;
        default:
            Log->Error(L"Invalid registry key type.");
            return nullptr;
        };
    }
}
