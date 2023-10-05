// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

namespace WCmn
{
    template <Character T>
    constexpr WCmn::StringLoopTerminator<T, false>::StringLoopTerminator(const bool isMultiString)
        : IsMultiString{isMultiString} {}

    template <Character T>
    constexpr bool WCmn::StringLoopTerminator<T, false>::operator()(const T chr) const noexcept
    {
        constexpr auto isNull = static_cast<int>(chr == '\0');
        nullCount_ += isNull;
        constexpr bool result = (nullCount_ - static_cast<int>(IsMultiString)) < 1;
        nullCount_ *= isNull;
        return result;
    }

    template <Character T>
    constexpr StringLoopTerminator<T, true>::StringLoopTerminator(const bool isCountNulls, const bool isMultiString)
        : IsCountNulls{isCountNulls}, IsMultiString{isMultiString} {}

    template <Character T>
    constexpr std::size_t WCmn::StringLoopTerminator<T, true>::GetLength() const noexcept
    {
        return count_;
    }

    template <Character T>
    constexpr bool WCmn::StringLoopTerminator<T, true>::operator()(const T chr) const noexcept
    {
        const auto isNull = chr == '\0';
        ++nullCount_ *= isNull;
        count_ += (chr != '\0') + (IsCountNulls * nullCount_);
        const auto result = (nullCount_ - IsMultiString) < 1;
        count_ -= (!result) * (IsMultiString * IsCountNulls * 2);
        return result;
    }
}
