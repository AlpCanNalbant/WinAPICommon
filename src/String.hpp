// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#pragma once

#include <cstring>
#include <memory>
#include <concepts>
#include <locale>
#include <codecvt>
#include "Concepts.hpp"

namespace Wcm
{
    namespace Impl
    {
        using StringConverter = std::wstring_convert<std::conditional_t<sizeof(wchar_t) == 4, std::codecvt_utf8<wchar_t>, std::codecvt_utf8_utf16<wchar_t>>>;
        template <typename T>
        constexpr bool IsConvertibleWString = requires(T wide) { { (StringConverter{}).to_bytes(wide) } -> std::same_as<StringConverter::byte_string>; };
        template <typename T>
        constexpr bool IsConvertibleString = requires(T narrow) { { (StringConverter{}).from_bytes(narrow) } -> std::same_as<StringConverter::wide_string>; };
        template <typename T>
        constexpr bool IsByteString = std::disjunction_v<std::is_same<T, StringConverter::byte_string>, std::is_same<T, StringConverter::byte_string::value_type>, std::is_convertible<T, const StringConverter::byte_string::value_type *>>;
        template <typename T>
        constexpr bool IsWideString = std::disjunction_v<std::is_same<T, StringConverter::wide_string>, std::is_same<T, StringConverter::wide_string::value_type>, std::is_convertible<T, const StringConverter::wide_string::value_type *>>;
        template <typename T>
        requires IsByteString<T> || IsWideString<T>
        using ToStringIfResult = std::conditional_t<IsWideString<T>, StringConverter::byte_string, const StringConverter::byte_string &>;
        template <typename T>
        requires IsByteString<T> || IsWideString<T>
        using ToWStringIfResult = std::conditional_t<IsByteString<T>, StringConverter::wide_string, const StringConverter::wide_string &>;
    }

    template <Character T>
    [[nodiscard]] std::shared_ptr<T> ToBuffer(std::basic_string_view<T> str);
    [[nodiscard]] Impl::StringConverter::byte_string ToString(const auto &wide)
        requires Impl::IsConvertibleWString<decltype(wide)>;
    template <typename T>
    [[nodiscard]] Impl::ToStringIfResult<T> ToStringIf(const T &string);
    [[nodiscard]] Impl::StringConverter::wide_string ToWString(const auto &narrow)
        requires Impl::IsConvertibleString<decltype(narrow)>;
    template <typename T>
    [[nodiscard]] Impl::ToWStringIfResult<T> ToWStringIf(const T &string);
    template <Character T>
    [[nodiscard]] DWORD GetStringLength(const T *buffer, bool countNull = false);
    template <Character T>
    [[nodiscard]] DWORD GetMultiStringLength(const T *buffer, bool countNull = false);
}

#include "String.inl"
