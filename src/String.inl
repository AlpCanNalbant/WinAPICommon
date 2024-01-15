// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

namespace Wcm
{
    template <Character T>
    std::shared_ptr<T> ToBuffer(std::basic_string_view<T> str)
    {
        T *buffer = static_cast<T *>(std::malloc(sizeof(T) * (str.size() + 1)));
        std::memcpy(buffer, str.data(), sizeof(T) * (str.size() + 1));
        return std::shared_ptr<T>{buffer, [](T *p) { std::free(p); }};
    }

    Impl::StringConverter::byte_string ToString(const auto &wide)
        requires Impl::IsConvertibleWString<decltype(wide)>
    {
        return (Impl::StringConverter{}).to_bytes(wide);
    }

    template <typename T>
    Impl::ToStringIfResult<T> ToStringIf(const T &string)
    {
        if constexpr (Impl::IsWideString<T>)
        {
            return (Impl::StringConverter{}).to_bytes(string);
        }
        else
        {
            return string;
        }
    }

    Impl::StringConverter::wide_string ToWString(const auto &narrow)
        requires Impl::IsConvertibleString<decltype(narrow)>
    {
        return (Impl::StringConverter{}).from_bytes(narrow);
    }

    template <typename T>
    Impl::ToWStringIfResult<T> ToWStringIf(const T &string)
    {
        if constexpr (Impl::IsByteString<T>)
        {
            return (Impl::StringConverter{}).from_bytes(string);
        }
        else
        {
            return string;
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
