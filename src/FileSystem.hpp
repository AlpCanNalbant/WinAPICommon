// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#pragma once
#include <fstream>
#include <filesystem>
#include <string_view>
#include <initializer_list>
#include <source_location>
#include <algorithm>
#include <shlwapi.h>
#include <pathcch.h>
#include <winbase.h>
#include "Log.hpp"

namespace Wcm
{
    namespace Impl
    {
        bool GetFileHandlesReadonly(const StringLike auto &srcFile, const StringLike auto &destFile, _Out_ HANDLE &hSrcFile, _Out_ HANDLE &hDestFile);
        template <Character T = std::filesystem::path::value_type>
        inline constexpr T Separator = static_cast<T>(std::filesystem::path::preferred_separator);
    }
    inline constexpr char Separator = Impl::Separator<char>;
    inline constexpr wchar_t WSeparator = Impl::Separator<wchar_t>;

    [[nodiscard]] std::filesystem::path ToBaseDirectory(const StringLike auto &path);
    [[nodiscard]] std::filesystem::path GetBaseDirectory();
    [[nodiscard]] std::filesystem::path GetSourceDirectory(const std::source_location &location = std::source_location::current());
    template <StringLike T>
    [[nodiscard]] bool IsFileExists(const T &file);
    [[nodiscard]] bool IsFileNewer(const StringLike auto &srcFile, const StringLike auto &destFile);
    bool UpdateFileContent(const StringLike auto &srcFile, const StringLike auto &destFile);
    template <StringLike T>
    bool MakeDirectory(const T &dir);
    template <StringLike T>
    bool MakeDirectories(const T &dirs);
    template <StringLike T>
        requires std::same_as<CharacterOf<T>, CharacterOf<T>>
    [[nodiscard]] bool IsSameFile(const T &srcFile, const T &destFile);
    [[nodiscard]] bool IsSameFile(const StringLike auto &srcFile, const StringLike auto &destFile);
}

#include "FileSystem.inl"
