// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#pragma once

#include <pathcch.h>

#include "Concepts.hpp"

namespace WinCmn
{
    template <Character T = wchar_t>
    [[nodiscard]] std::basic_string<T> GetBaseDirectory();
}

#include "Path.inl"
