// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#pragma once

#include <pathcch.h>
#include <fstream>
#include <filesystem>
#include <string_view>
#include <initializer_list>
#include <source_location>

#include "String.hpp"
#include "Log.hpp"
#include "Concepts.hpp"

namespace Wcm
{
    namespace Impl
    {
        template <Character T = std::filesystem::path::value_type>
        inline constexpr T Separator = static_cast<T>(std::filesystem::path::preferred_separator);
    }

    inline constexpr char Separator = Impl::Separator<char>;
    inline constexpr wchar_t WSeparator = Impl::Separator<wchar_t>;

    template <Character T = wchar_t>
    [[nodiscard]] std::filesystem::path GetBaseDirectory();
    [[nodiscard]] std::filesystem::path GetSourceDirectory(const std::source_location &location = std::source_location::current());
    template <Character T>
    [[nodiscard]] bool IsSameFile(const T *const file1, const T *const file2);
    template <Character T>
    [[nodiscard]] bool IsSameFile(const std::basic_string<T> &file1, const std::basic_string<T> &file2);
}

#include "FileSystem.inl"
