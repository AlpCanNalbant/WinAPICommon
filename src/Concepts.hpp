// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#pragma once

#include <cstddef>
#include <utility>
#include <type_traits>

namespace Wcm
{
    namespace Impl
    {
        template <typename T>
        inline constexpr bool IsCharacter =
            std::disjunction_v<std::is_same<T, char>, std::is_same<T, signed char>, std::is_same<T, unsigned char>,
                               std::is_same<T, wchar_t>, std::is_same<T, char8_t>, std::is_same<T, char16_t>, std::is_same<T, char32_t>>;
    }

    template <size_t Size, typename... T>
    inline constexpr bool IsTypesEqualTo = ((sizeof...(T)) == Size);

    template <typename T>
    inline constexpr bool IsStringView = std::disjunction_v<std::is_same<std::remove_cvref_t<T>, std::string_view>, std::is_same<std::remove_cvref_t<T>, std::wstring_view>>;

    template <typename T>
    concept Character = Impl::template IsCharacter<std::remove_cvref_t<T>>;

    template <typename... T>
    concept StringViewConvertible = std::conjunction_v<std::disjunction<std::is_convertible<T, std::string_view>, std::is_convertible<T, std::wstring_view>>...>;
}
