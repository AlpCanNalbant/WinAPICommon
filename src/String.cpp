// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#include "Log.hpp"
#include "String.hpp"

namespace Wcm
{
    StringCopyResult StringCopy(LPTSTR dest, LPCTSTR src)
    {
        HRESULT hRes;
        if (FAILED(hRes = StringCchCopy(dest, sizeof(dest), src)))
        {
            switch (hRes)
            {
            case STRSAFE_E_INVALID_PARAMETER:
                Log->Error("The string copy operation failed due to destination is either 0 or larger than STRSAFE_MAX_CCH.");
                return StringCopyResult::InvalidParameter;
            case STRSAFE_E_INSUFFICIENT_BUFFER:
                Log->Error("The string copy operation failed due to insufficient buffer space. The destination buffer contains a truncated, null-terminated version of the intended result. In situations where truncation is acceptable, this may not necessarily be seen as a failure condition.");
                return StringCopyResult::InsufficientBuffer;
            }
        }
        return StringCopyResult::Succeeded;
    }
}

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
