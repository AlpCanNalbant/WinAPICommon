// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#pragma once

#include <cstring>
#include <memory>
#include "Concepts.hpp"

namespace WinCmn
{
    template <Character T>
    [[nodiscard]] std::shared_ptr<T> ToBuffer(const std::basic_string<T> &str);
    template <Character T>
    [[nodiscard]] std::shared_ptr<T> ToBuffer(const std::basic_string<T> &&str);
    template <Character T>
    [[nodiscard]] DWORD GetStringLength(const T *buffer, bool countNull = false);
    template <Character T>
    [[nodiscard]] DWORD GetMultiStringLength(const T *buffer, bool countNull = false);
}

#include "String.inl"
