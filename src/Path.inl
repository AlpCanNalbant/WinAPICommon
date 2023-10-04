/*
// MIT License - Copyright (c) 2023 Alp Can Nalbant
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.
*/

namespace WCmn::Modules
{
    template <Character TChr, template <typename> typename TStr>
        requires std::derived_from<TStr<TChr>, std::basic_string<TChr>>
    TStr<TChr> Path::GetRoot() const
    {
        if constexpr (std::same_as<TChr, wchar_t>)
        {
            WCHAR buffer[MAX_PATH] = {'\0'};
            if (!GetModuleFileNameW(nullptr, buffer, MAX_PATH))
            {
                WCmn::Log->Error(L"Failed to retrieving the computer name.", GetLastError());
            }
            PathCchRemoveFileSpec(buffer, MAX_PATH);
            return {buffer};
        }
        else
        {
            TChr buffer[MAX_PATH] = {'\0'};
            if (!GetModuleFileNameA(nullptr, buffer, MAX_PATH))
            {
                WCmn::Log->Error(L"Failed to retrieving the computer name.", GetLastError());
            }

            TStr<TChr> path{buffer};
            return path.substr(0, path.find_last_of("\\/"));
        }
    }
}
