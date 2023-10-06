// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

namespace WinCmn
{
    template <Character T>
    BasicStringCounter<T>::BasicStringCounter(T *string, const bool isCountNulls, const bool isMultiString)
        : string_{string}, IsCountNulls{isCountNulls}, IsMultiString{isMultiString} {}

    template <Character T>
    inline bool BasicStringCounter<T>::IsCounted() const noexcept
    {
        return isCounted_;
    }

    template <Character T>
    std::size_t BasicStringCounter<T>::GetLength() const noexcept
    {
        return count_;
    }

    template <Character T>
    const T *BasicStringCounter<T>::GetString() const noexcept
    {
        return string_;
    }

    template <Character T>
    void BasicStringCounter<T>::SetString(T *string) noexcept
    {
        string_ = string;
        isCounted_ = false;
    }

    template <Character T>
    std::size_t BasicStringCounter<T>::Count() noexcept
    {
        for (int i = 0; IsNotNullTerminator(string_[i]); ++i)
            ;
        isCounted_ = true;
        return count_;
    }

    template <Character T>
    bool BasicStringCounter<T>::IsNotNullTerminator(const T chr) const noexcept
    {
        const auto isNull = chr == '\0';
        ++nullCount_ *= isNull;
        count_ += (chr != '\0') + (IsCountNulls * nullCount_);
        const auto result = (nullCount_ - IsMultiString) < 1;
        count_ -= (!result) * (IsMultiString * IsCountNulls * 2);
        return result;
    }
}
