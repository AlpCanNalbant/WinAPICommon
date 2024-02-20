// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#pragma once
#include <string>
#include <functional>
#include "StringCommon.hpp"
#include "StringLiteral.hpp"

namespace Wcm
{
    namespace Impl
    {
        struct BeginT
        {
            template <Range T>
            [[nodiscard]] constexpr Iterator<T> operator()(T &&t) const noexcept(IsNoexcept<T>());
        private:
            template <Range T>
            [[nodiscard]] static constexpr bool IsNoexcept() noexcept;
        };
        struct CBeginT
        {
            template <Range T>
            [[nodiscard]] constexpr ConstIterator<T> operator()(T &&t) const noexcept(IsNoexcept<T>());
        private:
            template <Range T>
            [[nodiscard]] static constexpr bool IsNoexcept() noexcept;
        };
        struct EndT
        {
            template <Range T>
            [[nodiscard]] constexpr Sentinel<T> operator()(T &&t) const noexcept(IsNoexcept<T>());
        private:
            template <Range T>
            [[nodiscard]] static constexpr bool IsNoexcept() noexcept;
        };
        struct CEndT
        {
            template <Range T>
            [[nodiscard]] constexpr ConstSentinel<T> operator()(T &&t) const noexcept(IsNoexcept<T>());
        private:
            template <Range T>
            [[nodiscard]] static constexpr bool IsNoexcept() noexcept;
        };

        struct RBeginT
        {
            template <Range T>
            [[nodiscard]] constexpr ReverseIterator<Iterator<T>> operator()(T &&t) const noexcept(IsNoexcept<T>());
        private:
            template <Range T>
            [[nodiscard]] static constexpr bool IsNoexcept() noexcept;
        };
        struct CRBeginT
        {
            template <Range T>
            [[nodiscard]] constexpr ReverseIterator<ConstIterator<T>> operator()(T &&t) const noexcept(IsNoexcept<T>());
        private:
            template <Range T>
            [[nodiscard]] static constexpr bool IsNoexcept() noexcept;
        };
        struct REndT
        {
            template <Range T>
            [[nodiscard]] constexpr ReverseIterator<Sentinel<T>> operator()(T &&t) const noexcept(IsNoexcept<T>());
        private:
            template <Range T>
            [[nodiscard]] static constexpr bool IsNoexcept() noexcept;
        };
        struct CREndT
        {
            template <Range T>
            [[nodiscard]] constexpr ReverseIterator<ConstSentinel<T>> operator()(T &&t) const noexcept(IsNoexcept<T>());
        private:
            template <Range T>
            [[nodiscard]] static constexpr bool IsNoexcept() noexcept;
        };
    }

    inline constexpr Impl::BeginT Begin{};
    inline constexpr Impl::CBeginT CBegin{};
    inline constexpr Impl::EndT End{};
    inline constexpr Impl::CEndT CEnd{};

    inline constexpr Impl::RBeginT RBegin{};
    inline constexpr Impl::CRBeginT CRBegin{};
    inline constexpr Impl::REndT REnd{};
    inline constexpr Impl::CREndT CREnd{};

    template <typename T, typename U>
        requires std::swappable_with<T, U>
    void Swap(T &&lhs, U &&rhs) noexcept(noexcept(std::ranges::swap(std::forward<T>(lhs), std::forward<U>(rhs))));

    template <typename T>
    [[nodiscard]] constexpr auto Empty(T &&t) noexcept(noexcept(std::empty(std::forward<T>(t))))
        -> decltype(std::empty(std::forward<T>(t)))
        requires requires { std::empty(std::forward<T>(t)); };
    template <typename T>
        requires std::invocable<decltype(std::ranges::empty), T>
    [[nodiscard]] constexpr auto Empty(T &&t) noexcept(noexcept(std::ranges::empty(std::forward<T>(t))))
        -> decltype(std::ranges::empty(std::forward<T>(t)));

    template <typename T>
        requires std::invocable<decltype(std::ranges::size), T>
    [[nodiscard]] constexpr auto Length(T &&t) noexcept(noexcept(std::ranges::size(std::forward<T>(t))))
        -> decltype(std::ranges::size(std::forward<T>(t)));
}

namespace std
{
    template <Wcm::Character T>
    struct iterator_traits<const T *const>
    {
        using difference_type = std::iterator_traits<T *>::difference_type;
        using value_type = std::remove_volatile_t<T>;
        using pointer = const T *const;
        using reference = const T &;
        using iterator_category = std::iterator_traits<T *>::iterator_category;
        using iterator_concept = std::iterator_traits<T *>::iterator_concept;
    };
}

#include "Ranges.inl"
