// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#pragma once

#include <pathcch.h>
#include <fstream>

#include "String.hpp"
#include "Concepts.hpp"

namespace WinCmn
{
    template <Character T = wchar_t>
    [[nodiscard]] std::basic_string<T> GetBaseDirectory();
    template <Character T>
    [[nodiscard]] bool IsSameFile(const T *const file1, const T *const file2);
}

#include "FileSystem.inl"
