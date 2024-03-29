// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#include "Registry.hpp"

namespace Wcm
{
    RegistryKey OpenRegistryKey(const RegistryKeyType keyType, std::wstring_view subKeyName)
    {
        HKEY hKey = ToKeyHandle(keyType);
        LPCWSTR lpSubKey = subKeyName.data();

        const auto isSuccess = RegCreateKeyExW(hKey, lpSubKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL);
        if (isSuccess != ERROR_SUCCESS)
        {
            Log->Error("Registry key could not be opened.", GetLastError());
            return {nullptr, keyType};
        }
        return {hKey, keyType};
    }

    HKEY ToKeyHandle(const RegistryKeyType branch)
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
            Log->Error("Invalid registry key type.");
            return nullptr;
        };
    }
}
