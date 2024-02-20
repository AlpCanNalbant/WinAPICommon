// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#include "StringCommon.hpp"

namespace Wcm::Impl
{
    size_t GetStringLengthT::operator()(const std::filesystem::path &path) const noexcept
    {
        return path.native().length();
    }
}
