/*
// MIT License - Copyright (c) 2023 Alp Can Nalbant
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.
*/

namespace WCmn::Modules
{
    template <Character T>
    T *String::ToBuffer(const std::basic_string<T> &str) const
    {
        const auto size = str.size();
        T buffer[size] = {'\0'};

        if constexpr (std::same_as<T, wchar_t>)
        {
            wcsncpy(buffer, str.c_str(), size);
            return buffer;
        }
        else
        {
            strncpy(buffer, str.c_str(), size);
            return buffer;
        }
    }

    template <Character T>
    DWORD String::GetLength(const T *string, bool isMulti) const
    {
        return GetLength(string, false, isMulti);
    }

    template <Character T>
    DWORD String::GetLengthCountNulls(const T *string, bool isMulti) const
    {
        return GetLength(string, true, isMulti);
    }

    template <Character T>
    DWORD String::GetLength(const T *string, bool isCountNulls, bool isMulti) const
    {
        slt_.IsCountNulls = isCountNulls;
        slt_.IsMultiString = isMulti;
        for (int i = 0; slt_(string[i]); ++i)
            ;
        return slt_.GetLength();
    }
}
