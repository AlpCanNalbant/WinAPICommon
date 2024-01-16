// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

namespace Wcm
{
    template <Character T>
    std::filesystem::path GetBaseDirectory()
    {
        if constexpr (std::is_same_v<T, wchar_t>)
        {
            WCHAR buffer[MAX_PATH] = {'\0'};
            if (!GetModuleFileNameW(nullptr, buffer, MAX_PATH))
            {
                Log->Error("Failed to retrieving the computer name.", GetLastError());
            }
            PathCchRemoveFileSpec(buffer, MAX_PATH);
            return {buffer};
        }
        else
        {
            T buffer[MAX_PATH] = {'\0'};
            if (!GetModuleFileNameA(nullptr, buffer, MAX_PATH))
            {
                Log->Error("Failed to retrieving the computer name.", GetLastError());
            }

            std::basic_string<T> path{buffer};
            return path.substr(0, path.find_last_of("\\/"));
        }
    }

    template <Character T>
    bool IsSameFile(const T *const file1, const T *const file2)
    {
#define WCM_FILESYSTEM_ISSAMEFILE_OPEN_ERROR_LOG(file) Log->Error("Equivalence comparison file is cannot opened.").Sub("ComparisonFile", file)
        std::basic_ifstream<T> stream1{file1, std::basic_ifstream<T>::binary | std::basic_ifstream<T>::ate};
        if (!stream1.is_open())
        {
            WCM_FILESYSTEM_ISSAMEFILE_OPEN_ERROR_LOG(file1);
            return false;
        }
        std::basic_ifstream<T> stream2{file2, std::basic_ifstream<T>::binary | std::basic_ifstream<T>::ate};
        if (!stream2.is_open())
        {
            WCM_FILESYSTEM_ISSAMEFILE_OPEN_ERROR_LOG(file2);
            return false;

        }
        if (stream1.tellg() != stream2.tellg())
        {
            return false;
        }
        stream1.seekg(0, std::basic_ifstream<T>::beg); stream2.seekg(0, std::basic_ifstream<T>::beg);
        return std::equal(std::istreambuf_iterator<T>{stream1.rdbuf()}, std::istreambuf_iterator<T>{}, std::istreambuf_iterator<T>{stream2.rdbuf()});
    }

    template <Character T>
    bool IsSameFile(const std::basic_string<T> &file1, const std::basic_string<T> &file2)
    {
        return IsSameFile(file1.c_str(), file2.c_str());
    }
}
