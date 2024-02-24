// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#pragma once

namespace Wcm
{
    namespace Impl
    {
        template <bool IsMultiString>
        struct IsNotNullTerminator final
        {
            template <Character T>
            [[nodiscard]] constexpr bool operator()(const T chr) const noexcept;

        private:
            mutable int nullCount_{};
        };
    }

    inline constexpr Impl::template IsNotNullTerminator<false> IsNotNullTerminator{};
    inline constexpr Impl::template IsNotNullTerminator<true> IsNotNullTerminatorMulti{};
}

#include "IsNotNullTerminator.inl"
