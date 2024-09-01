// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

namespace Wcm
{
    /* DEPRECATED
    template <StringLike T>
    std::basic_string_view<CharacterOf<T>> SetQuoted(T &str, const CharacterOf<T> delim, const CharacterOf<T> escape)
    {
        const auto begin = const_cast<ToPointer<decltype(std::ranges::data(ToStringView(str)))>>(std::ranges::data(ToStringView(str)));
        auto data = begin;
        if (auto end = End(data); (*data) == delim && (*End(data)) == delim)
        {
            auto dataRef = std::addressof(data);
            ++dataRef;
            *end = '\0';
        }
        std::basic_stringstream<CharacterOf<T>> ss;
        ss << std::quoted(data, delim, escape);
        for (const auto c : ss.str())
        {
            *data++ = c;
        }
        *data = '\0';
        return {begin, data};
    }
    */

    template <StringLike T>
    std::basic_string<CharacterOf<T>> ToQuoted(const T &str, const CharacterOf<T> delim, const CharacterOf<T> escape)
    {
        auto view = ToStringView(str);
        while (view.starts_with(delim) && view.ends_with(delim))
        {
            view.remove_prefix(1);
            view.remove_suffix(1);
        }
        std::basic_stringstream<CharacterOf<T>> ss;
        ss << std::quoted(view, delim, escape);
        return ss.str();
    }

    template <StringLike T>
    std::basic_string<CharacterOf<T>> ToUnquoted(const T &str, const CharacterOf<T> delim, const CharacterOf<T> escape)
    {
        auto view = ToStringView(str);
        std::basic_string<CharacterOf<T>> quoted;
        for (bool isEscapeSeqFound = false; const auto &c : view)
        {
            if (c == delim)
            {
                if (&c == &(*(str.cbegin())))
                {
                    continue;
                }
                else if (&c == &(*(str.cend() - 1)))
                {
                    break;
                }
            }
            if (c == escape && !isEscapeSeqFound)
            {
                isEscapeSeqFound = true;
                continue;
            }
            quoted += c;
            isEscapeSeqFound = false;
        }
        return quoted;
    }

    template <StringLike T>
    std::vector<std::basic_string<CharacterOf<T>>> ToUnquoteds(const T &str, const CharacterOf<T> delim, const CharacterOf<T> escape)
    {
        auto view = ToStringView(str);
        std::vector<std::basic_string<CharacterOf<T>>> quoteds;
        for (bool isEscapeSeqFound = false, isQuotesFound = false; const auto c : view)
        {
            if (c == delim)
            {
                if (!isQuotesFound)
                {
                    quoteds.resize(quoteds.size() + 1);
                }
                isQuotesFound = !isQuotesFound;
                continue;
            }
            if (isQuotesFound)
            {
                if (c == escape && !isEscapeSeqFound)
                {
                    isEscapeSeqFound = true;
                    continue;
                }
                quoteds.back() += c;
                isEscapeSeqFound = false;
            }
        }
        return quoteds;
    }

    template <Character T>
    std::shared_ptr<T> ToBuffer(std::basic_string_view<T> str)
    {
        T *buffer = static_cast<T *>(std::malloc(sizeof(T) * (str.size() + 1)));
        std::memcpy(buffer, str.data(), sizeof(T) * (str.size() + 1));
        return std::shared_ptr<T>{buffer, [](T *p)
                                  { std::free(p); }};
    }

    template <Character T>
    std::shared_ptr<T> StringAppend(const T *str, const T *subStr)
    {
        const auto len = GetStringLength(str);
        const auto lenSub = GetStringLength(subStr);
        T *buf = static_cast<T *>(std::malloc(len + lenSub + 1));
        std::memcpy(buf, str, sizeof(T) * len);
        std::memcpy(buf + len, subStr, sizeof(T) * (lenSub + 1));
        return std::shared_ptr<T>{buf, [](T *p)
                                  { std::free(p); }};
    }

    template <StringLike T>
    constexpr std::basic_string_view<CharacterOf<T>> ToStringView(const T &str)
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

    auto ToString(std::integral auto num)
    {
        return Impl::ToString<char>(num);
    }

    template <std::integral auto N>
    auto ToString()
    {
        return Impl::ToString<char, N>();
    }

    auto ToWString(std::integral auto num)
    {
        return Impl::ToString<wchar_t>(num);
    }

    template <std::integral auto N>
    auto ToWString()
    {
        return Impl::ToString<wchar_t, N>();
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

    namespace Impl
    {
        template <StringLike T>
        constexpr auto GetDataT::operator()(T &&t) const
        {
            if constexpr (requires { std::ranges::data(t); })
            {
                return std::ranges::data(t);
            }
            else if constexpr (requires { std::ranges::data(t.native()); })
            {
                return std::ranges::data(t.native());
            }
            else if constexpr (CharacterPointer<T>)
            {
                return t;
            }
        }

        constexpr bool UnorderedContainsT::operator()(const CharacterStringAny auto &lhs, const CharacterStringAny auto &rhs) const noexcept
        {
#ifdef WCM_CPP23
            if (lhs.size() >= rhs.size())
            {
                return lhs.contains(rhs);
            }
            else
            {
                return rhs.contains(lhs);
            }
#else
            if (lhs.size() >= rhs.size())
            {
                return lhs.find(rhs) != std::string::npos;
            }
            else
            {
                return rhs.find(lhs) != std::string::npos;
            }
#endif
        }

        constexpr bool UnorderedContainsT::operator()(CharacterRawString auto &&lhs, CharacterRawString auto &&rhs) const noexcept
        {
            if constexpr (Array<decltype(lhs)> && Array<decltype(rhs)>)
            {
                return UnorderedContains(lhs, rhs, LengthOf<decltype(lhs)>, LengthOf<decltype(rhs)>);
            }
            else if constexpr (Array<decltype(lhs)>)
            {
                return UnorderedContains(lhs, rhs, LengthOf<decltype(lhs)>, GetStringLength(rhs));
            }
            else if constexpr (Array<decltype(rhs)>)
            {
                return UnorderedContains(lhs, rhs, GetStringLength(lhs), LengthOf<decltype(rhs)>);
            }
            else
            {
                return UnorderedContains(lhs, rhs, GetStringLength(lhs), GetStringLength(rhs));
            }
        }

        constexpr bool UnorderedContainsT::operator()(IgnoreArrayLengthT, CharacterRawString auto &&lhs, CharacterRawString auto &&rhs) const noexcept
        {
            return UnorderedContains(lhs, rhs, GetStringLength(lhs), GetStringLength(rhs));
        }

        template <Character T>
        constexpr bool UnorderedContainsT::UnorderedContains(const T *lhs, const T *rhs, const size_t lhsLen, const size_t rhsLen) const noexcept
        {
            if (rhsLen < lhsLen)
            {
                bool isEqual;
                auto i = 0uz;
                if (!std::is_constant_evaluated())
                {
                    do
                    {
                        isEqual = std::memcmp(lhs, rhs, sizeof(T) * rhsLen) == 0;
                        ++lhs;
                    } while (!isEqual && ((++i) + rhsLen) <= lhsLen);
                }
                else
                {
                    do
                    {
                        isEqual = std::char_traits<T>::compare(lhs, rhs, rhsLen) == 0;
                        ++lhs;
                    } while (!isEqual && ((++i) + rhsLen) <= lhsLen);
                }
                return isEqual;
            }
            else if (lhsLen < rhsLen)
            {
                bool isEqual;
                auto i = 0uz;
                if (!std::is_constant_evaluated())
                {
                    do
                    {
                        isEqual = std::memcmp(lhs, rhs, sizeof(T) * lhsLen) == 0;
                        ++rhs;
                    } while (!isEqual && ((++i) + lhsLen) <= rhsLen);
                }
                else
                {
                    do
                    {
                        isEqual = std::char_traits<T>::compare(lhs, rhs, lhsLen) == 0;
                        ++rhs;
                    } while (!isEqual && ((++i) + lhsLen) <= rhsLen);
                }
                return isEqual;
            }
            else
            {
                if (!std::is_constant_evaluated())
                {
                    return std::memcmp(lhs, rhs, sizeof(T) * lhsLen) == 0;
                }
                return std::char_traits<T>::compare(lhs, rhs, lhsLen);
            }
        }

        constexpr bool ContainsT::operator()(const CharacterStringAny auto &lhs, const CharacterStringAny auto &rhs) const noexcept
        {
#ifdef WCM_CPP23
            return lhs.contains(rhs);
#else
            return lhs.find(rhs) != std::string::npos;
#endif
        }

        constexpr bool ContainsT::operator()(CharacterRawString auto &&lhs, CharacterRawString auto &&rhs) const noexcept
        {
            if constexpr (Array<decltype(lhs)> && Array<decltype(rhs)>)
            {
                return Contains(lhs, rhs, LengthOf<decltype(lhs)>, LengthOf<decltype(rhs)>);
            }
            else if constexpr (Array<decltype(lhs)>)
            {
                return Contains(lhs, rhs, LengthOf<decltype(lhs)>, GetStringLength(rhs));
            }
            else if constexpr (Array<decltype(rhs)>)
            {
                return Contains(lhs, rhs, GetStringLength(lhs), LengthOf<decltype(rhs)>);
            }
            else
            {
                return Contains(lhs, rhs, GetStringLength(lhs), GetStringLength(rhs));
            }
        }

        constexpr bool ContainsT::operator()(IgnoreArrayLengthT, CharacterRawString auto &&lhs, CharacterRawString auto &&rhs) const noexcept
        {
            return Contains(lhs, rhs, GetStringLength(lhs), GetStringLength(rhs));
        }

        template <Character T>
        constexpr bool ContainsT::Contains(const T *lhs, const T *const rhs, const size_t lhsLen, const size_t rhsLen) const noexcept
        {
            if (rhsLen < lhsLen)
            {
                bool isEqual;
                auto i = 0uz;
                if (!std::is_constant_evaluated())
                {
                    do
                    {
                        isEqual = std::memcmp(lhs, rhs, sizeof(T) * rhsLen) == 0;
                        ++lhs;
                    } while (!isEqual && ((++i) + rhsLen) <= lhsLen);
                }
                else
                {
                    do
                    {
                        isEqual = std::char_traits<T>::compare(lhs, rhs, rhsLen) == 0;
                        ++lhs;
                    } while (!isEqual && ((++i) + rhsLen) <= lhsLen);
                }
                return isEqual;
            }
            else
            {
                if (!std::is_constant_evaluated())
                {
                    return std::memcmp(lhs, rhs, sizeof(T) * lhsLen) == 0;
                }
                return std::char_traits<T>::compare(lhs, rhs, lhsLen);
            }
        }

        template <StringLike T, StringLike U>
            requires std::same_as<CharacterOf<T>, CharacterOf<U>>
        constexpr bool IsSameStringT::operator()(const T &lhs, const U &rhs) const noexcept
        {
            return this->operator()(lhs, rhs, false);
        }

        template <StringLike T, StringLike U>
            requires std::same_as<CharacterOf<T>, CharacterOf<U>>
        constexpr bool IsSameStringT::operator()(const T &lhs, const U &rhs, const size_t length, const bool caseSensitive) const noexcept
        {
            return IsSameString(ToStringView(lhs), ToStringView(rhs), length, caseSensitive);
        }

        template <StringLike T, StringLike U>
            requires std::same_as<CharacterOf<T>, CharacterOf<U>>
        constexpr bool IsSameStringT::operator()(const T &lhs, const U &rhs, const std::same_as<bool> auto caseSensitive) const noexcept
        {
            return IsSameString(ToStringView(lhs), ToStringView(rhs), caseSensitive);
        }

        template <Character T>
            requires std::same_as<T, T>
        constexpr bool IsSameStringT::IsSameString(std::basic_string_view<T> lhs, std::basic_string_view<T> rhs, const std::same_as<bool> auto caseSensitive) const noexcept
        {
            return lhs.length() == rhs.length() && IsSameString(lhs, rhs, lhs.length(), caseSensitive);
        }

        template <Character T>
            requires std::same_as<T, T>
        constexpr bool IsSameStringT::IsSameString(std::basic_string_view<T> lhs, std::basic_string_view<T> rhs, const size_t length, const bool caseSensitive) const noexcept
        {
            if constexpr (std::is_same_v<wchar_t, T>)
            {
                return (!caseSensitive ? _wcsnicmp(lhs.data(), rhs.data(), length) : std::wcsncmp(lhs.data(), rhs.data(), length)) == 0;
            }
            else if constexpr (std::is_same_v<char, T>)
            {
                return (!caseSensitive ? _strnicmp(lhs.data(), rhs.data(), length) : std::strncmp(lhs.data(), rhs.data(), length)) == 0;
            }
            // When type is only the plain char(signed or unsigned implementation defined) should first be converted to unsigned char,
            // other char types including char8_t(always unsigned) doesn't need to be converted.
            return !caseSensitive ? std::ranges::equal(lhs, rhs, [](const T c1, const T c2)
                                                       { return std::char_traits<T>::eq_int_type(std::toupper(c1, std::locale("en_US.utf8")),
                                                                                                 std::toupper(c2, std::locale("en_US.utf8"))); })
                                  : std::char_traits<T>::compare(lhs.data(), rhs.data(), length) == 0;
        }

        template <typename C, std::integral auto N>
            requires(std::same_as<C, char> || std::same_as<C, wchar_t>)
        auto ToString()
        {
            std::array<char, ([]() consteval -> std::size_t
                              {
        if (auto n = N; n != 0)
        {
        std::size_t count = 0;
        while (n != 0)
        {
            n = n / 10;
            ++count;
        }
        return count + (N < 0) + 1;
        }
        return 1 + (N < 0) + 1; })()>
                res;
            if (const auto [ptr, ec] = std::to_chars(res.data(), res.data() + res.size(), N); ec != std::errc())
            {
                if constexpr (const auto errMsg = std::make_error_code(ec).message(); std::same_as<C, char>)
                {
                    std::array<char, STRSAFE_MAX_CCH> err;
                    std::memcpy(err.data(), errMsg.c_str(), errMsg.size() + 1);
                    return err;
                }
                else
                {
                    std::array<wchar_t, STRSAFE_MAX_CCH> errw;
                    std::mbstowcs(errw.data(), errMsg.c_str(), errMsg.size() + 1);
                    return errw;
                }
            }
            res.back() = '\0';
            if constexpr (std::same_as<C, char>)
            {
                return res;
            }
            else
            {
                std::array<C, res.size()> resw;
                std::mbstowcs(resw.data(), res.data(), res.size());
                return resw;
            }
        }

        template <typename C>
            requires(std::same_as<C, char> || std::same_as<C, wchar_t>)
        auto ToString(std::integral auto num)
        {
            const int len = (([num]() mutable -> int
                              {
        if (num != 0)
        {
        int count = 0;
        while (num != 0)
        {
            num = num / 10;
            ++count;
        }
        return count + (num < 0) + 1;
        }
        return 1 + (num < 0) + 1; })());
            char *buf = static_cast<char *>(std::malloc(len));
            if (const auto [ptr, ec] = std::to_chars(buf, buf + len, num); ec != std::errc())
            {
                if constexpr (const auto errMsg = std::make_error_code(ec).message(); std::same_as<C, char>)
                {
                    char *err = static_cast<char *>(std::malloc(errMsg.size() + 1));
                    std::memcpy(err, errMsg.c_str(), errMsg.size() + 1);
                    return std::shared_ptr<char>{err, [](char *p)
                                                 { std::free(p); }};
                }
                else
                {
                    wchar_t *errw = static_cast<wchar_t *>(std::malloc(sizeof(wchar_t) * errMsg.size() + 1));
                    std::mbstowcs(errw, errMsg.c_str(), errMsg.size() + 1);
                    return std::shared_ptr<wchar_t>{errw, [](wchar_t *p)
                                                    { std::free(p); }};
                }
            }
            (*(buf + len)) = '\0';
            if constexpr (std::same_as<C, char>)
            {
                return std::shared_ptr<char>{buf, [](char *p)
                                             { std::free(p); }};
            }
            else
            {
                wchar_t *bufw = static_cast<wchar_t *>(std::malloc(sizeof(wchar_t) * len));
                std::mbstowcs(bufw, buf, len);
                return std::shared_ptr<wchar_t>{bufw, [](wchar_t *p)
                                                { std::free(p); }};
            }
        }
    }
}
