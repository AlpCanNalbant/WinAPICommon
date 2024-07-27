// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

namespace Wcm
{
    template <typename E1, typename E2>
    requires std::integral<UnderlyingCommonType<E1, E2>>
    constexpr bool HasFlag(const E1 lhs, const E2 rhs) noexcept
    {
        return static_cast<UnderlyingCommonType<E1, E2>>(lhs) & static_cast<UnderlyingCommonType<E1, E2>>(rhs);
    }
}
