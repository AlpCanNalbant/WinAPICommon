// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

namespace WinCmn
{
    template <Character T>
    T *ToBuffer(const std::basic_string<T> &str)
    {
        const auto size = str.size();
        T buffer[size] = {'\0'};

        if constexpr (std::is_same_v<T, wchar_t>)
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
    DWORD GetStringLength(const T *buffer, bool countNull)
    {
        DWORD i = 0;
        while (buffer[i])
        {
            ++i;
        }
        return i + countNull;
    }

    template <Character T>
    DWORD GetMultiStringLength(const T *buffer, bool countNull)
    {
        DWORD len = countNull;
        for (int i = 0; buffer[i] || buffer[i + 1]; ++i)
        {
            len += static_cast<bool>(buffer[i]) + (!buffer[i] * countNull);
        }
        return len;
    }
}
