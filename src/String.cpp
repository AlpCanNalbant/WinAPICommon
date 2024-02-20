// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#include "String.hpp"

namespace Wcm::Impl
{
    bool UnorderedContainsT::operator()(const std::filesystem::path &lhs, const std::filesystem::path &rhs) const noexcept
    {
#ifdef WCM_CPP23
        if (lhs.native().size() >= rhs.native().size())
        {
            return lhs.native().contains(rhs.native());
        }
        else
        {
            return rhs.native().contains(lhs.native());
        }
#else
        if (lhs.native().size() >= rhs.native().size())
        {
            return lhs.native().find(rhs.native()) != std::string::npos;
        }
        else
        {
            return rhs.native().find(lhs.native()) != std::string::npos;
        }
#endif
    }

    bool ContainsT::operator()(const std::filesystem::path &lhs, const std::filesystem::path &rhs) const noexcept
    {
#ifdef WCM_CPP23
        return lhs.native().contains(rhs.native());
#else
        return lhs.native().find(rhs.native()) != std::string::npos;
#endif
    }
}
