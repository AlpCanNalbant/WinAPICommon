// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

namespace Wcm
{
    template <Character T>
    bool RegistryKey::SetString(std::basic_string_view<std::remove_pointer_t<T>> name, const RegistryStringType type, const T *stringData) const
    {
        if constexpr (const DWORD len = type != RegistryStringType::MultiSZ ? GetStringLength(stringData) + 1 : GetMultiStringLength(stringData, true);
                      std::is_same_v<T, wchar_t>)
        {
            if (RegSetValueExW(hKey_, name.data(), 0, static_cast<DWORD>(type), reinterpret_cast<LPCBYTE>(stringData), len * sizeof(T)) != ERROR_SUCCESS)
            {
                Log->Error("Value of registry key could not be set.", GetLastError()).Sub("Name", name);
                return false;
            }
        }
        else
        {
            if (RegSetValueExA(hKey_, name.data(), 0, static_cast<DWORD>(type), reinterpret_cast<LPCBYTE>(stringData), len * sizeof(T)) != ERROR_SUCCESS)
            {
                Log->Error("Value of registry key could not be set.", GetLastError()).Sub("Name", name);
                return false;
            }
        }
        return true;
    }
}
