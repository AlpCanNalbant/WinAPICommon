// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

namespace WinCmn
{
    template <Character T>
    std::basic_string<T> GetBaseDirectory()
    {
        if constexpr (std::is_same_v<T, wchar_t>)
        {
            WCHAR buffer[MAX_PATH] = {'\0'};
            if (!GetModuleFileNameW(nullptr, buffer, MAX_PATH))
            {
                WinCmn::Log->Error(L"Failed to retrieving the computer name.", GetLastError());
            }
            PathCchRemoveFileSpec(buffer, MAX_PATH);
            return {buffer};
        }
        else
        {
            T buffer[MAX_PATH] = {'\0'};
            if (!GetModuleFileNameA(nullptr, buffer, MAX_PATH))
            {
                WinCmn::Log->Error(L"Failed to retrieving the computer name.", GetLastError());
            }

            std::basic_string<T> path{buffer};
            return path.substr(0, path.find_last_of("\\/"));
        }
    }
}
