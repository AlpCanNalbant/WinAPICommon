// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#include "FileSystem.hpp"

namespace Wcm
{
    std::filesystem::path CutPath(const std::filesystem::path &path, const std::filesystem::path &itemName, bool inReverse)
    {
        if (!inReverse)
        {
            std::filesystem::path resPath;
            for (auto begin = path.begin(); begin != path.end() && itemName != *begin; ++begin)
            {
                resPath /= *begin;
            }
            return resPath / itemName;
        }
        else
        {
            typename std::filesystem::path::string_type resPath = path.filename().native();
            for (auto begin = --path.begin(), end = ----path.end(); end != begin && itemName != *end; --end)
            {
                resPath = (*end).native() + std::filesystem::path::preferred_separator + resPath;
            }
            return itemName.native() + std::filesystem::path::preferred_separator + resPath;
        }
    }

    std::filesystem::path GetBaseDirectory()
    {
#define WCM_FILESYSTEM_GETBASEDIRECTORY_MODULE_ERROR_LOG Log->Error("Failed to retrieving the module file name.", GetLastError())
#ifdef WCM_UNICODE
        WCHAR buffer[MAX_PATH] = {'\0'};
        if (!GetModuleFileNameW(nullptr, buffer, MAX_PATH))
        {
            WCM_FILESYSTEM_GETBASEDIRECTORY_MODULE_ERROR_LOG;
            return std::filesystem::current_path();
        }
        return ToBaseDirectory(buffer);
#else
        CHAR buffer[MAX_PATH] = {'\0'};
        if (!GetModuleFileNameA(nullptr, buffer, MAX_PATH))
        {
            WCM_FILESYSTEM_GETBASEDIRECTORY_MODULE_ERROR_LOG;
            return std::filesystem::current_path();
        }
        return ToBaseDirectory(buffer);
#endif
    }

    std::filesystem::path GetSourceDirectory(const std::source_location &location)
    {
        return std::filesystem::path{location.file_name()}.parent_path();
    }
}
