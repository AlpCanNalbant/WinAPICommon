// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#pragma once
#include <memory>
#include <locale>
#include <codecvt>
#include <wchar.h>
#include <cwctype>
#include <cwchar>
#include <string_view>
#include <type_traits>
#include <strsafe.h>
#include "Ranges.hpp"
#include "StringCommon.hpp"
#include "TypeTraits.hpp"

namespace Wcm
{
    template <typename T>
    struct CharacterOfT
    {
    };
    template <typename T>
        requires requires { typename std::remove_cvref_t<T>::value_type; }
    struct CharacterOfT<T>
    {
        using Type = typename std::remove_cvref_t<T>::value_type;
    };
    template <typename T>
        requires CharacterPointer<T> || CharacterArray<T>
    struct CharacterOfT<T>
    {
        using Type = RemoveAll<T>;
    };
    template <Character T>
    struct CharacterOfT<T>
    {
        using Type = std::remove_cvref_t<T>;
    };

    template <typename T>
    using CharacterOf = CharacterOfT<T>::Type;

    enum class StringCopyResult : HRESULT
    {
        Succeeded = S_OK,
        InvalidParameter = STRSAFE_E_INVALID_PARAMETER,
        InsufficientBuffer = STRSAFE_E_INSUFFICIENT_BUFFER
    };

    namespace Impl
    {
        using StringConverter =
            std::wstring_convert<std::conditional_t<sizeof(wchar_t) == 4, std::codecvt_utf8<wchar_t>, std::codecvt_utf8_utf16<wchar_t>>>;

        template <typename T>
        constexpr bool IsConvertibleWString =
            requires(T wide) { { (StringConverter{}).to_bytes(wide) } -> std::same_as<StringConverter::byte_string>; };
        template <typename T>
        constexpr bool IsConvertibleString =
            requires(T narrow) { { (StringConverter{}).from_bytes(narrow) } -> std::same_as<StringConverter::wide_string>; };

        template <typename T>
        constexpr bool IsByteString =
            std::disjunction_v<std::is_convertible<T, StringConverter::byte_string>, std::is_same<std::remove_cvref_t<T>, StringConverter::byte_string::value_type>,
                               std::is_convertible<std::decay_t<T>, const StringConverter::byte_string::value_type *>, std::is_same<std::remove_cvref_t<T>, std::string_view>>;
        template <typename T>
        constexpr bool IsWideString =
            std::disjunction_v<std::is_convertible<T, StringConverter::wide_string>, std::is_same<std::remove_cvref_t<T>, StringConverter::wide_string::value_type>,
                               std::is_convertible<std::decay_t<T>, const StringConverter::wide_string::value_type *>, std::is_same<std::remove_cvref_t<T>, std::wstring_view>>;

        template <typename T>
        struct ToStringIfResultT
        {
            using Type = std::add_lvalue_reference_t<std::add_const_t<T>>;
        };
        template <typename T>
            requires IsWideString<T>
        struct ToStringIfResultT<T>
        {
            using Type = StringConverter::byte_string;
        };
        template <typename T>
            requires IsByteString<T> && std::is_pointer_v<std::decay_t<T>>
        struct ToStringIfResultT<T>
        {
            using Type = std::decay_t<std::add_const_t<T>>;
        };

        template <typename T>
        struct ToWStringIfResultT
        {
            using Type = std::add_lvalue_reference_t<std::add_const_t<T>>;
        };
        template <typename T>
            requires IsByteString<T>
        struct ToWStringIfResultT<T>
        {
            using Type = StringConverter::wide_string;
        };
        template <typename T>
            requires IsWideString<T> && std::is_pointer_v<std::decay_t<T>>
        struct ToWStringIfResultT<T>
        {
            using Type = std::decay_t<std::add_const_t<T>>;
        };

        template <typename T>
        using ToStringIfResult = ToStringIfResultT<T>::Type;
        template <typename T>
        using ToWStringIfResult = ToWStringIfResultT<T>::Type;

        struct IgnoreArrayLengthT
        {
            explicit IgnoreArrayLengthT() = default;
        };
        inline constexpr IgnoreArrayLengthT IgnoreArrayLength{};

        struct UnorderedContainsT final
        {
            [[nodiscard]] constexpr bool operator()(const CharacterStringAny auto &lhs, const CharacterStringAny auto &rhs) const noexcept;
            [[nodiscard]] bool operator()(const std::filesystem::path &lhs, const std::filesystem::path &rhs) const noexcept;
            [[nodiscard]] constexpr bool operator()(CharacterRawString auto &&lhs, CharacterRawString auto &&rhs) const noexcept;
            [[nodiscard]] constexpr bool operator()(IgnoreArrayLengthT, CharacterRawString auto &&lhs, CharacterRawString auto &&rhs) const noexcept;

        private:
            template <Character T>
            [[nodiscard]] constexpr bool UnorderedContains(const T *lhs, const T *rhs, const size_t lhsLen, const size_t rhsLen) const noexcept;
        };

        struct ContainsT final
        {
            [[nodiscard]] constexpr bool operator()(const CharacterStringAny auto &lhs, const CharacterStringAny auto &rhs) const noexcept;
            [[nodiscard]] bool operator()(const std::filesystem::path &lhs, const std::filesystem::path &rhs) const noexcept;
            [[nodiscard]] constexpr bool operator()(CharacterRawString auto &&lhs, CharacterRawString auto &&rhs) const noexcept;
            [[nodiscard]] constexpr bool operator()(IgnoreArrayLengthT, CharacterRawString auto &&lhs, CharacterRawString auto &&rhs) const noexcept;

        private:
            template <Character T>
            [[nodiscard]] constexpr bool Contains(const T *lhs, const T *const rhs, const size_t lhsLen, const size_t rhsLen) const noexcept;
        };

        struct IsSameStringT final
        {
            template <StringLike T, StringLike U>
                requires std::same_as<CharacterOf<T>, CharacterOf<U>>
            [[nodiscard]] constexpr bool operator()(const T &lhs, const U &rhs) const noexcept;
            template <StringLike T, StringLike U>
                requires std::same_as<CharacterOf<T>, CharacterOf<U>>
            [[nodiscard]] constexpr bool operator()(const T &lhs, const U &rhs, const std::same_as<bool> auto caseSensitive) const noexcept;
            template <StringLike T, StringLike U>
                requires std::same_as<CharacterOf<T>, CharacterOf<U>>
            [[nodiscard]] constexpr bool operator()(const T &lhs, const U &rhs, const size_t length, const bool caseSensitive = false) const noexcept;

        private:
            template <Character T>
                requires std::same_as<T, T>
            [[nodiscard]] constexpr bool IsSameString(std::basic_string_view<T> lhs, std::basic_string_view<T> rhs, const std::same_as<bool> auto caseSensitive) const noexcept;
            template <Character T>
                requires std::same_as<T, T>
            [[nodiscard]] constexpr bool IsSameString(std::basic_string_view<T> lhs, std::basic_string_view<T> rhs, const size_t length, const bool caseSensitive) const noexcept;
        };
    }

    template <CharacterRawString T>
    inline constexpr auto LengthOf = std::conditional_t<CountOf<T> != -1, std::integral_constant<size_t, CountOf<T>>, std::integral_constant<int, 0>>::value - 1;

    inline constexpr Impl::UnorderedContainsT UnorderedContains{};
    inline constexpr Impl::ContainsT Contains{};
    inline constexpr Impl::IsSameStringT IsSameString{};

    // template <StringLike T>
    // std::basic_string_view<CharacterOf<T>> SetQuoted(T &str, const CharacterOf<T> delim = '"', const CharacterOf<T> escape = '\\');
    template <StringLike T>
    [[nodiscard]] std::basic_string<CharacterOf<T>> ToQuoted(const T &str, const CharacterOf<T> delim = '"', const CharacterOf<T> escape = '\\');
    template <StringLike T>
    [[nodiscard]] std::basic_string<CharacterOf<T>> ToUnquoted(const T &str, const CharacterOf<T> delim = '"', const CharacterOf<T> escape = '\\');
    template <StringLike T>
    [[nodiscard]] std::vector<std::basic_string<CharacterOf<T>>> ToUnquoteds(const T &str, const CharacterOf<T> delim = '"', const CharacterOf<T> escape = '\\');
    template <Character T>
    [[nodiscard]] std::shared_ptr<T> ToBuffer(std::basic_string_view<T> str);
    StringCopyResult StringCopy(LPTSTR dest, LPCTSTR src);
    template <StringLike T>
    [[nodiscard]] auto GetData(T &&t);
    template <StringLike T>
    [[nodiscard]] constexpr std::basic_string_view<CharacterOf<T>> ToStringView(const T &str);
    [[nodiscard]] Impl::StringConverter::byte_string ToString(const auto &wide)
        requires Impl::IsConvertibleWString<decltype(wide)>;
    template <typename T>
    [[nodiscard]] Impl::ToStringIfResult<T> ToStringIf(const T &string);
    [[nodiscard]] Impl::StringConverter::wide_string ToWString(const auto &narrow)
        requires Impl::IsConvertibleString<decltype(narrow)>;
    template <typename T>
    [[nodiscard]] Impl::ToWStringIfResult<T> ToWStringIf(const T &string);
}

#include "String.inl"
