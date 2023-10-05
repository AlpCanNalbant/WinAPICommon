// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#pragma once

#include <concepts>

#include "Concepts.hpp"

namespace WCmn
{
    template <Character T = wchar_t, bool IsHasCounter = false>
    struct StringLoopTerminator
    {
    };

    template <Character T>
    struct StringLoopTerminator<T, false>
    {
        bool IsMultiString;

        constexpr explicit StringLoopTerminator(const bool isMultiString = false);

        [[nodiscard]] constexpr bool operator()(const T chr) const noexcept;

    private:
        mutable int nullCount_{};
    };

    template <Character T>
    struct StringLoopTerminator<T, true>
    {
        bool IsCountNulls;
        bool IsMultiString;

        constexpr explicit StringLoopTerminator(const bool isCountNulls = false, const bool isMultiString = false);
        [[nodiscard]] constexpr std::size_t GetLength() const noexcept;

        [[nodiscard]] constexpr bool operator()(const T chr) const noexcept;

    private:
        mutable std::size_t count_{};
        mutable int nullCount_{};
    };
}

#include "StringLoopTerminator.inl"
