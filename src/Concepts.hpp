// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#pragma once

#include <type_traits>
#include <concepts>
#include <string>

namespace WinCmn
{
    namespace Impl
    {
        template <typename T>
        inline constexpr bool IsCharacter =
            std::disjunction_v<std::is_same<T, char>, std::is_same<T, signed char>, std::is_same<T, unsigned char>,
                               std::is_same<T, wchar_t>, std::is_same<T, char8_t>, std::is_same<T, char16_t>, std::is_same<T, char32_t>>;
    }

    template <typename T>
    concept Character = Impl::template IsCharacter<std::remove_cvref_t<T>>;
}
