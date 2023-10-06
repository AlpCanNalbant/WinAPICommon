// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

namespace WinCmn
{
    namespace Impl
    {
        template <bool IsMultiString>
        template <Character T>
        constexpr bool IsNotNullTerminator<IsMultiString>::operator()(const T chr) const noexcept
        {
            constexpr auto isNull = chr == '\0';
            nullCount_ += isNull;

            if constexpr (IsMultiString)
            {
                constexpr bool result = (nullCount_ - 1) < 1;
                nullCount_ *= isNull;

                return result;
            }
            else
            {
                constexpr bool result = nullCount_ < 1;
                nullCount_ *= isNull;

                return result;
            }
        }
    }

    inline constexpr Impl::template IsNotNullTerminator<false> IsNotNullTerminator{};
    inline constexpr Impl::template IsNotNullTerminator<true> IsNotNullTerminatorMulti{};
}
