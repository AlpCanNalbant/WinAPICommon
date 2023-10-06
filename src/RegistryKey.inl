// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

namespace WinCmn
{
    template <Character T>
    bool RegistryKey::SetString(const std::basic_string<std::remove_pointer_t<T>> &name, const RegistryStringType type, const T *stringData) const
    {
        if constexpr (const DWORD len = type != RegistryStringType::MultiSZ ? GetStringLength(stringData, true) : GetMultiStringLength(stringData, true);
                      std::is_same_v<T, wchar_t>)
        {
            if (RegSetValueExW(hKey_, name.c_str(), 0, static_cast<DWORD>(type), reinterpret_cast<LPCBYTE>(stringData), len * sizeof(T)) != ERROR_SUCCESS)
            {
                Log->Error(L"Value of registry key could not be set. Name is " + name, GetLastError());
                return false;
            }
        }
        else
        {
            if (RegSetValueExA(hKey_, name.c_str(), 0, static_cast<DWORD>(type), reinterpret_cast<LPCBYTE>(stringData), len * sizeof(T)) != ERROR_SUCCESS)
            {
                Log->Error(L"Value of registry key could not be set. Name is " + name, GetLastError());
                return false;
            }
        }
        return true;
    }
}
