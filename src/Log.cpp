// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#include <comdef.h>
#include "Log.hpp"
#include "System.hpp"

namespace Wcm
{
    extern const std::unique_ptr<Impl::Log> Log = std::make_unique<Impl::Log>();

    namespace Impl
    {
        std::basic_string<TCHAR> Log::GetLastErrorMessage() const
        {
            return ToErrorMessage(GetLastError());
        }

        std::basic_string<TCHAR> Log::ToErrorMessage(const HRESULT errorCode) const
        {
            return _com_error{errorCode}.ErrorMessage();
        }
    }
}
