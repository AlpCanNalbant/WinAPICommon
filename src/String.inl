// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

namespace WinCmn
{
    template <Character T>
    std::shared_ptr<T> ToBuffer(const std::basic_string<T> &str)
    {
        return ToBuffer(static_cast<const std::basic_string<T> &&>(str));
    }

    template <Character T>
    std::shared_ptr<T> ToBuffer(const std::basic_string<T> &&str)
    {
        T *buffer = static_cast<T *>(std::malloc(sizeof(T) * (str.size() + 1)));
        std::memcpy(buffer, str.data(), sizeof(T) * (str.size() + 1));
        return std::shared_ptr<T>{buffer, [](T *p) { std::free(p); }};
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
