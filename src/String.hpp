// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#pragma once
#include <memory>
#include <locale>
#include <codecvt>
#include <wchar.h>
#include <cwctype>
#include <string_view>
#include <type_traits>
#include "Ranges.hpp"
#include "StringCommon.hpp"

namespace Wcm
{
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
            using Type = std::add_const_t<std::decay_t<T>>;
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
    }

    template <StringLike T>
    void ToQuoted(T &str, const CharacterOf<T> delim = '"', const CharacterOf<T> escape = '\\');
    template <StringLike T>
    [[nodiscard]] auto ToQuoted(const T &str, const CharacterOf<T> delim = '"', const CharacterOf<T> escape = '\\');
    template <Character T>
    [[nodiscard]] std::shared_ptr<T> ToBuffer(std::basic_string_view<T> str);
    template <StringLike T>
    [[nodiscard]] auto GetData(T &&t);
    template <StringLike T>
    [[nodiscard]] std::basic_string_view<CharacterOf<T>> ToStringView(const T &str);
    [[nodiscard]] Impl::StringConverter::byte_string ToString(const auto &wide)
        requires Impl::IsConvertibleWString<decltype(wide)>;
    template <typename T>
    [[nodiscard]] Impl::ToStringIfResult<T> ToStringIf(const T &string);
    [[nodiscard]] Impl::StringConverter::wide_string ToWString(const auto &narrow)
        requires Impl::IsConvertibleString<decltype(narrow)>;
    template <typename T>
    [[nodiscard]] Impl::ToWStringIfResult<T> ToWStringIf(const T &string);
    template <Character T>
    [[nodiscard]] bool IsSameString(std::basic_string_view<T> str1, std::basic_string_view<T> str2, bool caseSensitive = false);
    template <CharacterPointer T>
    [[nodiscard]] constexpr size_t GetStringLength(const T begin, const T end) noexcept;
    template <Character T>
    [[nodiscard]] DWORD GetMultiStringLength(const T *buffer, bool countNullTerminators = false);
}

#include "String.inl"
