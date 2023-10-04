/*
// MIT License - Copyright (c) 2023 Alp Can Nalbant
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.
*/

#pragma once

#include <type_traits>
#include <concepts>
#include <string>

namespace WCmn
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
