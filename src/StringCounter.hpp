// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#pragma once

#include "Concepts.hpp"

namespace WinCmn
{
    template <Character T>
    struct BasicStringCounter
    {
        bool IsCountNulls;
        bool IsMultiString;

        explicit BasicStringCounter(T *string = nullptr, const bool isCountNulls = false, const bool isMultiString = false);
        [[nodiscard]] bool IsCounted() const noexcept;
        [[nodiscard]] std::size_t GetLength() const noexcept;
        [[nodiscard]] const T *GetString() const noexcept;
        void SetString(T *string) noexcept;
        std::size_t Count() noexcept;

    private:
        [[nodiscard]] bool IsNotNullTerminator(const T chr) const noexcept;

        T *string_;
        mutable std::size_t count_{};
        mutable int nullCount_{};
        mutable bool isCounted_{false};
    };

    using StringCounter = BasicStringCounter<char>;
    using WStringCounter = BasicStringCounter<wchar_t>;
}

#include "StringCounter.inl"
