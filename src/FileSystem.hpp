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

#define FILE_ATTRIBUTE_NOT_FOUND 0x00000000

namespace Wcm
{
    namespace Impl
    {
        template <Character T = std::filesystem::path::value_type>
        inline constexpr T Separator = static_cast<T>(std::filesystem::path::preferred_separator);

        bool GetFileHandlesReadonly(const StringLike auto &srcFile, const StringLike auto &destFile, _Out_ HANDLE &hSrcFile, _Out_ HANDLE &hDestFile);
        template <StringLike T>
            requires std::disjunction_v<std::is_same<CharacterOf<T>, CHAR>, std::is_same<CharacterOf<T>, WCHAR>>
        [[nodiscard]] DWORD GetFileAttribs(const T &file);
    }
    inline constexpr char Separator = Impl::Separator<char>;
    inline constexpr wchar_t WSeparator = Impl::Separator<wchar_t>;

    [[nodiscard]] std::filesystem::path ToBaseDirectory(const StringLike auto &path);
    template <StringLike T>
    [[nodiscard]] std::filesystem::path ToPathName(const T &path);
    [[nodiscard]] std::filesystem::path CutPath(const std::filesystem::path &path, const std::filesystem::path &itemName, bool inReverse = false);
    [[nodiscard]] std::filesystem::path GetBaseDirectory();
    [[nodiscard]] std::filesystem::path GetSourceDirectory(const std::source_location &location = std::source_location::current());
    template <StringLike T>
    [[nodiscard]] bool IsFileExists(const T &file);
    template <StringLike T>
    [[nodiscard]] bool IsDirectoryExists(const T &dir);
    [[nodiscard]] bool IsFileNewer(const StringLike auto &srcFile, const StringLike auto &destFile);
    bool UpdateFileContent(const StringLike auto &srcFile, const StringLike auto &destFile);
    bool UpdateFileContent(const StringLike auto &srcFile, const StringLike auto &destFile, bool &isError);
    bool WriteToFile(const StringLike auto &text, const std::filesystem::path &outputFile, const bool append = false);
    bool SearchTextLineByLine(const std::filesystem::path &file, const StringLike auto &searchText, CharacterString auto &foundLine)
        requires std::same_as<CharacterOf<decltype(searchText)>, CharacterOf<decltype(foundLine)>>;
    template <StringLike T>
    bool MakeDirectory(const T &dir);
    template <StringLike T>
    bool MakeDirectories(const T &dirs);
    template <StringLike T>
    [[nodiscard]] bool IsSameFile(const T &srcFile, const T &destFile);
    [[nodiscard]] bool IsSameFile(const StringLike auto &srcFile, const StringLike auto &destFile);
}

#include "FileSystem.inl"
