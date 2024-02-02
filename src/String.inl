// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

namespace Wcm
{
    template <StringLike T>
    void ToQuoted(T &str, const CharacterOf<T> delim, const CharacterOf<T> escape)
    {
        auto data = const_cast<ToPointer<decltype(GetBuffer(str))>>(GetBuffer(str));
        if (auto end = End(data); (*data) == delim && (*End(data)) == delim)
        {
            auto dataRef = std::addressof(data);
            ++dataRef;
            *end = '\0';
        }
        data = std::quoted(data, delim, escape);
    }

    template <StringLike T>
    auto ToQuoted(const T &str, const CharacterOf<T> delim, const CharacterOf<T> escape)
    {
        auto view = ToStringView(str);
        if (view.starts_with(delim) && view.ends_with(delim))
        {
            view.remove_prefix(1);
            view.remove_suffix(1);
        }
        return std::quoted(view, delim, escape);
    }

    template <Character T>
    std::shared_ptr<T> ToBuffer(std::basic_string_view<T> str)
    {
        T *buffer = static_cast<T *>(std::malloc(sizeof(T) * (str.size() + 1)));
        std::memcpy(buffer, str.data(), sizeof(T) * (str.size() + 1));
        return std::shared_ptr<T>{buffer, [](T *p) { std::free(p); }};
    }

    template <StringLike T>
    auto GetData(T &&t)
    {
        if constexpr ( requires { std::ranges::data(t); })
        {
            return std::ranges::data(t);
        }
        else if constexpr ( requires { std::ranges::data(t.native()); })
        {
            return std::ranges::data(t.native());
        }
        else if constexpr (CharacterPointer<T>)
        {
            return t;
        }
    }

    template <StringLike T>
    std::basic_string_view<CharacterOf<T>> ToStringView(const T &str)
    {
        if constexpr (!std::same_as<std::remove_cvref_t<T>, std::filesystem::path>)
        {
            return {Begin(str), End(str)};
        }
        else
        {
            return {Begin(str.native()), End(str.native())};
        }
    }

    Impl::StringConverter::byte_string ToString(const auto &wide)
        requires Impl::IsConvertibleWString<decltype(wide)>
    {
        if constexpr (!WideCharacterStringView<decltype(wide)>)
        {
            return (Impl::StringConverter{}).to_bytes(wide);
        }
        else
        {
            return (Impl::StringConverter{}).to_bytes(wide.begin(), wide.end());
        }
    }

    template <typename T>
    Impl::ToStringIfResult<T> ToStringIf(const T &string)
    {
        if constexpr (Impl::IsWideString<T>)
        {
            if constexpr (!WideCharacterStringView<T>)
            {
                return (Impl::StringConverter{}).to_bytes(string);
            }
            else
            {
                return (Impl::StringConverter{}).to_bytes(string.begin(), string.end());
            }
        }
        else
        {
            return string;
        }
    }

    Impl::StringConverter::wide_string ToWString(const auto &narrow)
        requires Impl::IsConvertibleString<decltype(narrow)>
    {
        if constexpr (!ByteCharacterStringView<decltype(narrow)>)
        {
            return (Impl::StringConverter{}).from_bytes(narrow);
        }
        else
        {
            return (Impl::StringConverter{}).from_bytes(narrow.begin(), narrow.end());
        }
    }

    template <typename T>
    Impl::ToWStringIfResult<T> ToWStringIf(const T &string)
    {
        if constexpr (Impl::IsByteString<T>)
        {
            if constexpr (!ByteCharacterStringView<T>)
            {
                return (Impl::StringConverter{}).from_bytes(string);
            }
            else
            {
                return (Impl::StringConverter{}).from_bytes(string.begin(), string.end());
            }
        }
        else
        {
            return string;
        }
    }

    template <Character T>
    bool IsSameString(std::basic_string_view<T> str1, std::basic_string_view<T> str2, bool caseSensitive)
    {
        const auto length = str1.length();
        if (length != str2.length())
        {
            return false;
        }
        if (!caseSensitive)
        {
            if constexpr (std::is_same_v<std::remove_cvref_t<T>, wchar_t>)
            {
                return !wcsnicmp(str1.data(), str2.data(), length);
            }
            else if constexpr (std::is_same_v<std::remove_cvref_t<T>, char>)
            {
                return !strnicmp(str1.data(), str2.data(), length);
            }
        }
        if constexpr (std::is_same_v<std::remove_cvref_t<T>, wchar_t>)
        {
            return !std::wcsncmp(str1.data(), str2.data(), length);
        }
        else if constexpr (std::is_same_v<std::remove_cvref_t<T>, char>)
        {
            return !std::strncmp(str1.data(), str2.data(), length);
        }
        return std::ranges::equal(str1, str2, [](auto c1, auto c2)
            { return std::towlower(static_cast<std::wint_t>(c1)) == std::towlower(static_cast<std::wint_t>(c2)); });
    }

    template <CharacterPointer T>
    constexpr size_t GetStringLength(const T begin, const T end) noexcept
    {
        return end - begin;
    }

    template <Character T>
    DWORD GetMultiStringLength(const T *buffer, bool countNullTerminators)
    {
        DWORD len = countNullTerminators;
        for (int i = 0; buffer[i] || buffer[i + 1]; ++i)
        {
            len += static_cast<bool>(buffer[i]) + (!buffer[i] * countNullTerminators);
        }
        return len;
    }
}
