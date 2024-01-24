// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

namespace Wcm
{
    std::filesystem::path ToBaseDirectory(const StringLike auto &path)
    {
#ifdef WCM_UNICODE
        auto pathString = ToWStringIf(ToStringView(path));
        auto buffer = const_cast<PWSTR>(pathString.data());
        std::replace(Wcm::RBegin(buffer), Wcm::REnd(buffer), L'/', L'\\');
        if (PathCchRemoveFileSpec(buffer, pathString.length()) != S_OK)
        {
            pathString = buffer; // The data pointed to may have changed, so update it before any operation with it.
            Log->Error("Failed to remove last element of path.", GetLastError()).Sub("Path", pathString);
            return pathString.substr(0, pathString.find_last_of(L'\\'));
        }
#else
        auto pathString = ToStringIf(ToStringView(path));
        auto buffer = const_cast<LPSTR>(pathString.data());
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
    bool IsFileExists(const T &file)
    {
        DWORD dwFileAttribs = 0;
        if constexpr (std::is_same_v<std::remove_cvref_t<CharacterOf<T>>, CHAR>)
        {
            if ((dwFileAttribs = GetFileAttributesA(ToStringView(file).data())) == INVALID_FILE_ATTRIBUTES)
            {
                Log->Error("Atrributes of the assocaited file is cannot obtained.", GetLastError()).Sub("File", file);
                return false;
            }
        }
        else if constexpr (std::is_same_v<std::remove_cvref_t<CharacterOf<T>>, CHAR>)
        {
            if ((dwFileAttribs = GetFileAttributesW(ToStringView(file).data())) == INVALID_FILE_ATTRIBUTES)
            {
                Log->Error("Atrributes of the assocaited file is cannot obtained.", GetLastError()).Sub("File", file);
                return false;
            }
        }
        else
        {
            return std::filesystem::is_regular_file({file});
        }
        dwFileAttribs &= FILE_ATTRIBUTE_DIRECTORY;
        return dwFileAttribs == 0;
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
        if (!IsFileExists(srcFile))
        {
            Log->Error("Source file is not found.").Sub("SourceFile", srcFile);
            return false;
        }
        // Also compare their contents if the last write time of the destination file is older.
        if (IsFileExists(destFile) && (!IsFileNewer(srcFile, destFile) || IsSameFile(srcFile, destFile)))
        {
            return false;
        }
        COPYFILE2_EXTENDED_PARAMETERS params{};
        params.dwSize = sizeof(params);
        if (!SUCCEEDED(CopyFile2(ToStringView(ToWStringIf(srcFile)).data(), ToStringView(ToWStringIf(destFile)).data(), &params)))
        {
            Log->Error("Cannot overwrite the destination file with the source file.", GetLastError()).Sub("SourceFile", srcFile).Sub("DestinationFile", destFile);
            return false;
        }
        return true;
    }

    template <StringLike T>
    bool MakeDirectory(const T &dir)
    {
#define WCM_FILESYSTEM_MAKEDIRECTORY_CREATE_ERROR_LOG(dir) Log->Error("Failed to create directory.", GetLastError()).Sub("Directory", dir)
        if constexpr (WideCharacterPointer<T>)
        {
            if (!CreateDirectoryW(dir, nullptr) && GetLastError() != ERROR_ALREADY_EXISTS)
            {
                WCM_FILESYSTEM_MAKEDIRECTORY_CREATE_ERROR_LOG(dir);
                return false;
            }
        }
        else if constexpr (ByteCharacterPointer<T>)
        {
            if (!CreateDirectoryA(dir, nullptr) && GetLastError() != ERROR_ALREADY_EXISTS)
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
        const auto isSeparator = [] (auto c) -> bool { return c == '/' || c == '\\'; };
        const auto getFind = [&isSeparator] (auto begin, auto end) { return std::find_if(std::find_if_not(begin, end, isSeparator), end, isSeparator); };
        auto pos = Wcm::Begin(dirsView);
        const auto end = Wcm::End(dirsView);
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
        requires std::same_as<CharacterOf<T>, CharacterOf<T>>
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
        stream1.seekg(0, std::basic_ifstream<Chr>::beg); stream2.seekg(0, std::basic_ifstream<Chr>::beg);
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
            if ((hSrcFile = CreateFileW(reinterpret_cast<LPCWSTR>(ToStringView(ToWStringIf(srcFile)).data()),
                                        FILE_READ_ATTRIBUTES, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr)) == INVALID_HANDLE_VALUE)
            {
                Wcm::Log->Error("Handle of the source file is cannot be obtained.", GetLastError()).Sub("SourceFile", srcFile);
                return false;
            }
            if ((hDestFile = CreateFileW(reinterpret_cast<LPCWSTR>(ToStringView(ToWStringIf(destFile)).data()),
                FILE_READ_ATTRIBUTES, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr)) == INVALID_HANDLE_VALUE)
            {
                Wcm::Log->Error("Handle of the destination file is cannot be obtained.", GetLastError()).Sub("DestinationFile", destFile);
                return false;
            }
            return true;
        }
    }
}
