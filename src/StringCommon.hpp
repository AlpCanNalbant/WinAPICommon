// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#pragma once
#include <cstring>

namespace Wcm
{
    template <CharacterPointer T>
    [[nodiscard]] constexpr size_t GetStringLength(const T str) noexcept;
    template <Character T, size_t Size>
    [[nodiscard]] constexpr size_t GetStringLength(const T(&)[Size]) noexcept;
}

#include "StringCommon.inl"
