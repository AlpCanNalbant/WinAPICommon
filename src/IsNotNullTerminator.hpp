// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#pragma once

namespace WinCmn
{
    namespace Impl
    {
        template <bool IsMultiString>
        struct IsNotNullTerminator
        {
            template <Character T>
            [[nodiscard]] constexpr bool operator()(const T chr) const noexcept;

        private:
            mutable int nullCount_{};
        };
    }
}

#include "IsNotNullTerminator.inl"
