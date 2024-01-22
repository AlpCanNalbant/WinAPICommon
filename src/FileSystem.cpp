// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#include "FileSystem.hpp"

namespace Wcm
{
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
