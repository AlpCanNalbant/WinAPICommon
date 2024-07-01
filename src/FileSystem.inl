// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

namespace Wcm
{
    std::filesystem::path ToBaseDirectory(const StringLike auto &path)
    {
#ifdef WCM_UNICODE
        // If wstring_view or wchar_t is passed as path argument do not construct from it instead emplace from it.
        // Instead store the data itself, make a copy from it. Data may edited from in PathCchRemoveFileSpec and PathRemoveFileSpecA functions.
        std::wstring pathString;
        pathString += ToWStringIf(ToStringView(path));
        auto buffer = const_cast<PWSTR>(pathString.data());
        std::replace(RBegin(buffer), REnd(buffer), L'/', L'\\');
        if (PathCchRemoveFileSpec(buffer, pathString.length()) != S_OK)
        {
            pathString = buffer; // The data pointed to may have changed, so update it before any operation with it.
            Log->Error("Failed to remove last element of path.", GetLastError()).Sub("Path", pathString);
            return pathString.substr(0, pathString.find_last_of(L'\\'));
        }
#else
        std::string pathString;
        pathString += ToStringIf(ToStringView(path));
        auto buffer = const_cast<LPSTR>(pathString.data());
        std::replace(RBegin(buffer), REnd(buffer), '/', '\\');
        if (!PathRemoveFileSpecA(buffer))
        {
            pathString = buffer;
            Log->Error("Failed to remove last element of path.", GetLastError()).Sub("Path", pathString);
            return pathString.substr(0, pathString.find_last_of('\\'));
        }
#endif
        return buffer;
    }

    template <StringLike T>
    std::filesystem::path ToPathName(const T &path)
    {
        auto pathView = ToStringView(path);
        while (pathView.ends_with('\\') || pathView.ends_with('/'))
        {
            pathView.remove_suffix(1); // Remove last slashes from given path.
        }
        if constexpr (WideCharacter<CharacterOf<T>>)
        {
            return pathView.substr(pathView.find_last_of(L"\\/") + 1uz);
        }
        else
        {
            return pathView.substr(pathView.find_last_of("\\/") + 1uz);
        }
    }

    template <StringLike T>
    bool IsFileExists(const T &file)
    {
        if constexpr (requires { { Impl::GetFileAttribs(file) } -> std::same_as<DWORD>; })
        {
            if (const DWORD dwFileAttribs = Impl::GetFileAttribs(file); dwFileAttribs != INVALID_FILE_ATTRIBUTES)
            {
                return (dwFileAttribs != FILE_ATTRIBUTE_NOT_FOUND) && !(dwFileAttribs & FILE_ATTRIBUTE_DIRECTORY);
            }
        }
        return std::filesystem::is_regular_file(file);
    }

    template <StringLike T>
    bool IsDirectoryExists(const T &dir)
    {
        if constexpr (requires { { Impl::GetFileAttribs(dir) } -> std::same_as<DWORD>; })
        {
            if (const DWORD dwFileAttribs = Impl::GetFileAttribs(dir); dwFileAttribs != INVALID_FILE_ATTRIBUTES)
            {
                return (dwFileAttribs != FILE_ATTRIBUTE_NOT_FOUND) && (dwFileAttribs & FILE_ATTRIBUTE_DIRECTORY);
            }
        }
        return std::filesystem::is_directory(dir);
    }

    bool IsFileNewer(const StringLike auto &srcFile, const StringLike auto &destFile)
    {
        if (!IsFileExists(srcFile))
        {
            Log->Error("Source comparison file is not found.").Sub("SourceFile", srcFile);
            return false;
        }
        if (!IsFileExists(destFile))
        {
            Log->Error("Destination comparison file is not found.").Sub("DestinationFile", destFile);
            return false;
        }
        // First of all we need to compare the last write times.
        if (HANDLE hSrcFile, hDestFile; Impl::GetFileHandlesReadonly(srcFile, destFile, hSrcFile, hDestFile))
        {
            FILE_BASIC_INFO fbiSrcFileInfo, fbiDestFileInfo;
            if (!GetFileInformationByHandleEx(hSrcFile, FileBasicInfo, &fbiSrcFileInfo, sizeof(fbiSrcFileInfo)))
            {
                Log->Error("Source file last write time is cannot retrieved.", GetLastError()).Sub("SourceFile", srcFile);
                fbiSrcFileInfo = {};
            }
            if (!GetFileInformationByHandleEx(hDestFile, FileBasicInfo, &fbiDestFileInfo, sizeof(fbiDestFileInfo)))
            {
                Log->Error("Destination file last write time is cannot retrieved.", GetLastError()).Sub("DestinationFile", destFile);
                fbiDestFileInfo = {};
            }
            return fbiSrcFileInfo.LastWriteTime.QuadPart > fbiDestFileInfo.LastWriteTime.QuadPart; // Source file is newer than the dest file.
        }
        return false;
    }

    bool UpdateFileContent(const StringLike auto &srcFile, const StringLike auto &destFile)
    {
        bool dummy;
        return UpdateFileContent(srcFile, destFile, dummy);
    }

    bool UpdateFileContent(const StringLike auto &srcFile, const StringLike auto &destFile, bool &isError)
    {
        isError = false;
        if (!IsFileExists(srcFile))
        {
            Log->Error("Source file is not found.").Sub("SourceFile", srcFile);
            isError = true;
            return false;
        }
        // Also compare their contents if the last write time of the destination file is older.
        if (IsFileExists(destFile) && (!IsFileNewer(srcFile, destFile) || IsSameFile(srcFile, destFile)))
        {
            return false;
        }
        if (const auto baseDir = ToBaseDirectory(destFile); !IsDirectoryExists(baseDir))
        {
            if (!MakeDirectories(baseDir))
            {
                isError = true;
                return false;
            }
        }
        COPYFILE2_EXTENDED_PARAMETERS params{};
        params.dwSize = sizeof(params);
        if (!SUCCEEDED(CopyFile2(ToWStringIf(ToStringView(srcFile)).data(), ToWStringIf(ToStringView(destFile)).data(), &params)))
        {
            Log->Error("Cannot overwrite the destination file with the source file.", GetLastError()).Sub("SourceFile", srcFile).Sub("DestinationFile", destFile);
            isError = true;
            return false;
        }
        return true;
    }

    bool SearchTextLineByLine(const std::filesystem::path &file, const StringLike auto &searchText, CharacterString auto &foundLine)
        requires std::same_as<CharacterOf<decltype(searchText)>, CharacterOf<decltype(foundLine)>>
    {
        using Char = CharacterOf<decltype(searchText)>;
        std::basic_fstream<Char> ifs;
        ifs.open(file);
        if (!ifs.is_open())
        {
            // Win32 codes is running under implementation of the ifstream and this windows only library so GetLastError() can be used when any error happened.
            Wcm::Log->Error("Could not open the file to search.", GetLastError());
            return false;
        }

        constexpr auto strToUpper = [](CharacterStringView auto src, std::basic_string<Char> &dest)
        {
            std::ranges::transform(src.cbegin(), src.cend(), dest.begin(),
                                   [](const Char c)
                                   { return std::toupper(static_cast<unsigned char>(c), std::locale("en_US.utf8")); });
        };

        std::basic_string<Char> insensSrchText;
        strToUpper(ToStringView(searchText), insensSrchText);

        std::basic_string<Char> insensLine;
        while (ifs)
        {
            std::getline(ifs, foundLine);

            strToUpper(ToStringView(foundLine), insensLine);
            if (insensLine.find(insensSrchText) != std::basic_string<Char>::npos)
            {
                return true;
            }
        }

        return false;
    }

    template <StringLike T>
    bool MakeDirectory(const T &dir)
    {
#define WCM_FILESYSTEM_MAKEDIRECTORY_CREATE_ERROR_LOG(dir) Log->Error("Failed to create directory.", GetLastError()).Sub("Directory", dir)
        if constexpr (WideCharacterPointer<T>)
        {
            if (!CreateDirectoryW(ToStringView(dir).data(), nullptr) && GetLastError() != ERROR_ALREADY_EXISTS)
            {
                WCM_FILESYSTEM_MAKEDIRECTORY_CREATE_ERROR_LOG(dir);
                return false;
            }
        }
        else if constexpr (ByteCharacterPointer<T>)
        {
            if (!CreateDirectoryA(ToStringView(dir).data(), nullptr) && GetLastError() != ERROR_ALREADY_EXISTS)
            {
                WCM_FILESYSTEM_MAKEDIRECTORY_CREATE_ERROR_LOG(dir);
                return false;
            }
        }
        else
        {
            if constexpr (WideCharacter<CharacterOf<T>>)
            {
                if (!CreateDirectoryW(ToStringView(dir).data(), nullptr) && GetLastError() != ERROR_ALREADY_EXISTS)
                {
                    WCM_FILESYSTEM_MAKEDIRECTORY_CREATE_ERROR_LOG(dir);
                    return false;
                }
            }
            else
            {
                if (!CreateDirectoryA(ToStringView(dir).data(), nullptr) && GetLastError() != ERROR_ALREADY_EXISTS)
                {
                    WCM_FILESYSTEM_MAKEDIRECTORY_CREATE_ERROR_LOG(dir);
                    return false;
                }
            }
        }
        return true;
    }

    template <StringLike T>
    bool MakeDirectories(const T &dirs)
    {
        std::basic_string<CharacterOf<T>> current;
        const auto dirsView = ToStringView(dirs);
        const auto isSeparator = [](auto c) -> bool
        { return c == '/' || c == '\\'; };
        const auto getFind = [&isSeparator](auto begin, auto end)
        { return std::find_if(std::find_if_not(begin, end, isSeparator), end, isSeparator); };
        auto pos = Begin(dirsView);
        const auto end = End(dirsView);
        current.reserve(dirsView.length());
        while (pos != end)
        {
            const auto find = getFind(pos, end);
            current.append(pos, find);
            if (!MakeDirectory(current))
            {
                return false;
            }
            pos = find;
        }
        return true;
    }

    template <StringLike T>
    bool IsSameFile(const T &srcFile, const T &destFile)
    {
#define WCM_FILESYSTEM_ISSAMEFILE_OPEN_ERROR_LOG(file) Log->Error("Equivalence comparison file is cannot opened.").Sub("ComparisonFile", file)
        using Chr = CharacterOf<T>;
        typename std::basic_string_view<Chr>::const_pointer files[] = {ToStringView(srcFile).data(), ToStringView(destFile).data()};
        std::basic_ifstream<Chr> stream1{files[0], std::basic_ifstream<Chr>::binary | std::basic_ifstream<Chr>::ate};
        if (!stream1.is_open())
        {
            WCM_FILESYSTEM_ISSAMEFILE_OPEN_ERROR_LOG(files[0]);
            return false;
        }
        std::basic_ifstream<Chr> stream2{files[1], std::basic_ifstream<Chr>::binary | std::basic_ifstream<Chr>::ate};
        if (!stream2.is_open())
        {
            WCM_FILESYSTEM_ISSAMEFILE_OPEN_ERROR_LOG(files[1]);
            return false;
        }
        if (stream1.tellg() != stream2.tellg())
        {
            return false;
        }
        stream1.seekg(0, std::basic_ifstream<Chr>::beg);
        stream2.seekg(0, std::basic_ifstream<Chr>::beg);
        return std::equal(std::istreambuf_iterator<Chr>{stream1.rdbuf()}, std::istreambuf_iterator<Chr>{}, std::istreambuf_iterator<Chr>{stream2.rdbuf()});
    }

    bool IsSameFile(const StringLike auto &srcFile, const StringLike auto &destFile)
    {
        HANDLE hSrcFile, hDestFile;
        auto result = Impl::GetFileHandlesReadonly(srcFile, destFile, hSrcFile, hDestFile);
        if (!result)
        {
            return false;
        }
        FILE_ID_INFO fiiSrcFileInfo, fiiDestFileInfo;
        if (!GetFileInformationByHandleEx(hSrcFile, FileIdInfo, &fiiSrcFileInfo, sizeof(FILE_ID_INFO)))
        {
            Log->Error("ID of the source file is cannot obtained.", GetLastError()).Sub("SourceFile", srcFile);
            result = false;
        }
        if (!GetFileInformationByHandleEx(hDestFile, FileIdInfo, &fiiDestFileInfo, sizeof(FILE_ID_INFO)))
        {
            Log->Error("ID of the destination file is cannot obtained.", GetLastError()).Sub("DestinationFile", destFile);
            result = false;
        }
        if (result)
        {
            return !std::memcmp(&fiiSrcFileInfo, &fiiDestFileInfo, sizeof(FILE_ID_INFO));
        }
        return false;
    }

    namespace Impl
    {
        bool GetFileHandlesReadonly(const StringLike auto &srcFile, const StringLike auto &destFile, _Out_ HANDLE &hSrcFile, _Out_ HANDLE &hDestFile)
        {
            if ((hSrcFile = CreateFileW(reinterpret_cast<LPCWSTR>(ToWStringIf(ToStringView(srcFile)).data()),
                                        FILE_READ_ATTRIBUTES, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr)) == INVALID_HANDLE_VALUE)
            {
                Wcm::Log->Error("Handle of the source file is cannot be obtained.", GetLastError()).Sub("SourceFile", srcFile);
                return false;
            }
            if ((hDestFile = CreateFileW(reinterpret_cast<LPCWSTR>(ToWStringIf(ToStringView(destFile)).data()),
                                         FILE_READ_ATTRIBUTES, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr)) == INVALID_HANDLE_VALUE)
            {
                Wcm::Log->Error("Handle of the destination file is cannot be obtained.", GetLastError()).Sub("DestinationFile", destFile);
                return false;
            }
            return true;
        }

        template <StringLike T>
            requires std::disjunction_v<std::is_same<CharacterOf<T>, CHAR>, std::is_same<CharacterOf<T>, WCHAR>>
        DWORD GetFileAttribs(const T &file)
        {
            if constexpr (std::same_as<CharacterOf<T>, CHAR>)
            {
                if (const DWORD dwFileAttribs = GetFileAttributesA(ToStringView(file).data()); dwFileAttribs != INVALID_FILE_ATTRIBUTES)
                {
                    return dwFileAttribs;
                }
                if (const DWORD dwErrCode = GetLastError(); dwErrCode == 2 || dwErrCode == 3)
                {
                    return FILE_ATTRIBUTE_NOT_FOUND;
                }
                else
                {
                    Wcm::Log->Error("Atrributes of the assocaited file is cannot obtained.", dwErrCode).Sub("File", file);
                    return INVALID_FILE_ATTRIBUTES;
                }
            }
            else
            {
                if (const DWORD dwFileAttribs = GetFileAttributesW(ToStringView(file).data()); dwFileAttribs != INVALID_FILE_ATTRIBUTES)
                {
                    return dwFileAttribs;
                }
                if (const DWORD dwErrCode = GetLastError(); dwErrCode == 2 || dwErrCode == 3)
                {
                    return FILE_ATTRIBUTE_NOT_FOUND;
                }
                else
                {
                    Wcm::Log->Error("Atrributes of the assocaited file is cannot obtained.", dwErrCode).Sub("File", file);
                    return INVALID_FILE_ATTRIBUTES;
                }
            }
        }
    }
}
