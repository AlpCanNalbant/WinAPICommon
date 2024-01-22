// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#pragma once
#include <shlwapi.h>
#include <pathcch.h>
#include <fstream>
#include <filesystem>
#include <string_view>
#include <initializer_list>
#include <source_location>
#include <algorithm>
#include "Log.hpp"

namespace Wcm
{
    namespace Impl
    {
        template <Character T = std::filesystem::path::value_type>
        inline constexpr T Separator = static_cast<T>(std::filesystem::path::preferred_separator);
    }
    inline constexpr char Separator = Impl::Separator<char>;
    inline constexpr wchar_t WSeparator = Impl::Separator<wchar_t>;

    [[nodiscard]] std::filesystem::path ToBaseDirectory(const StringLike auto &path);
    [[nodiscard]] std::filesystem::path GetBaseDirectory();
    [[nodiscard]] std::filesystem::path GetSourceDirectory(const std::source_location &location = std::source_location::current());
    bool MakeDirectory(const StringLike auto &dir);
    bool MakeDirectories(const StringLike auto &dirs);
    template <StringLike T>
    [[nodiscard]] bool IsSameFile(const T &file1, const T &file2);
}

#include "FileSystem.inl"
