/*
// MIT License - Copyright (c) 2023 Alp Can Nalbant
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.
*/

namespace WCmn
{
    template <Character TChr>
    bool RegistryKey::SetString(const std::basic_string<std::remove_pointer_t<TChr>> &name, const RegistryStringType type, const TChr *stringData) const
    {
        const WCmn::StringLoopTerminator<wchar_t, true> slt{true, type == RegistryStringType::MultiSZ};
        for (int i = 0; slt(stringData[i]); ++i)
            ;
        if constexpr (std::same_as<TChr, wchar_t>)
        {
            if (RegSetValueExW(hKey_, name.c_str(), 0, static_cast<DWORD>(type), reinterpret_cast<const BYTE *>(stringData), slt.GetLength() * sizeof(TChr)) != ERROR_SUCCESS)
            {
                Log->Error(L"Value of registry key could not be set. Name is " + name, GetLastError());
                return false;
            }
        }
        else
        {
            if (RegSetValueExA(hKey_, name.c_str(), 0, static_cast<DWORD>(type), reinterpret_cast<const BYTE *>(stringData), slt.GetLength() * sizeof(TChr)) != ERROR_SUCCESS)
            {
                Log->Error(L"Value of registry key could not be set. Name is " + name, GetLastError());
                return false;
            }
        }
        return true;
    }
}
