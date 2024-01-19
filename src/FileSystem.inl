// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

namespace Wcm
{
    std::filesystem::path ToBaseDirectory(const StringLike auto &path)
    {
        const auto pathString = ToStringView(path);
#ifdef WCM_UNICODE
        auto buffer = const_cast<PWSTR>(pathString.data());
        if (PathCchRemoveFileSpec(buffer, MAX_PATH) == S_FALSE)
        {
            Log->Error("Failed to remove last element of path.", GetLastError()).Sub("Path", pathString);
            return pathString.substr(0, pathString.find_last_of(L"\\/"));
        }
#else
        return pathString.substr(0, pathString.find_last_of("\\/"));
#endif
    }

    bool MakeDirectory(const StringLike auto &dir)
    {
#define WCM_FILESYSTEM_MAKEDIRECTORY_CREATE_ERROR_LOG(dir) Log->Error("Failed to create directory.", GetLastError()).Sub("Directory", dir)
        if constexpr (CharacterPointer<decltype(dir)>)
        {
            if (!CreateDirectory(dir, nullptr) && GetLastError() != ERROR_ALREADY_EXISTS)
            {
                WCM_FILESYSTEM_MAKEDIRECTORY_CREATE_ERROR_LOG(dir);
                return false;
            }
        }
        else
        {
            if (!CreateDirectory(dir.data(), nullptr) && GetLastError() != ERROR_ALREADY_EXISTS)
            {
                WCM_FILESYSTEM_MAKEDIRECTORY_CREATE_ERROR_LOG(dir);
                return false;
            }
        }
        return true;
    }

    bool MakeDirectories(const StringLike auto &dirs)
    {
        std::filesystem::path::string_type current;
        const auto isSeparator = [](const auto c) { return c == '/' || c == '\\'; };
        if constexpr (CharacterPointer<decltype(dirs)>)
        {
            const auto length = GetStringLength(dirs);
            current.reserve(length);
            for (const auto pos = dirs.data(), end = dirs.data() + length, find = nullptr; pos != end; find = std::find_if(std::find_if_not(pos, isSeparator), end, isSeparator))
            {
                current.append(pos, find);
                if (!MakeDirectory(current))
                {
                    return false;
                }
                pos = find;
            }
        }
        else
        {
            current.reserve(GetStringLength(dirs.begin(), dirs.end()));
            for (const auto pos = dirs.begin(), find = nullptr; pos != dirs.cend(); find = std::find_if(std::find_if_not(pos, isSeparator), dirs.end(), isSeparator))
            {
                current.append(pos, find);
                if (!MakeDirectory(current))
                {
                    return false;
                }
                pos = find;
            }
        }
        return true;
    }

    template <StringLike T>
    bool IsSameFile(const T &file1, const T &file2)
    {
#define WCM_FILESYSTEM_ISSAMEFILE_OPEN_ERROR_LOG(file) Log->Error("Equivalence comparison file is cannot opened.").Sub("ComparisonFile", file)
        using Chr = RemoveAll<T>;
        typename std::basic_string_view<Chr>::const_pointer files[] = {ToStringView(file1).data(), ToStringView(file2).data()};
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

    template <Character T>
    bool IsSameFile(const std::basic_string<T> &file1, const std::basic_string<T> &file2)
    {
        return IsSameFile(file1.c_str(), file2.c_str());
    }
}
