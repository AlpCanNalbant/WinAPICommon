#include "FileSystem.hpp"

namespace Wcm
{
    std::filesystem::path GetSourceDirectory(const std::source_location &location)
    {
        return std::filesystem::path(location.file_name()).parent_path();
    }
}
