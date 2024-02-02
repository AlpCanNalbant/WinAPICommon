// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#pragma once
#include <cstring>
#include <functional>
#include "StringCommon.hpp"

namespace Wcm
{
    template <typename T>
    struct IteratorT { };
    template <CharacterPointer T>
    struct IteratorT<T>
    {
        using Type = std::remove_reference_t<T>;
    };
    template <typename T>
    requires requires { typename std::ranges::iterator_t<T>; }
    struct IteratorT<T>
    {
        using Type = std::ranges::iterator_t<T>;
    };

    template <typename T>
    struct ConstIteratorT { };
    template <CharacterPointer T>
    struct ConstIteratorT<T>
    {
        using Type = ToConstPointer<std::remove_reference_t<T>>;
    };
    template <typename T>
#ifdef WCM_CPP23
    requires requires { typename std::ranges::const_iterator_t<T>; }
#else
    requires requires { typename std::ranges::iterator_t<T>; }
#endif
    struct ConstIteratorT<T>
    {
#ifdef WCM_CPP23
        using Type = std::ranges::const_iterator_t<T>;
#else
        using Type = std::ranges::iterator_t<T>;
#endif
    };

    template <typename T>
    struct SentinelT { };
    template <CharacterPointer T>
    struct SentinelT<T>
    {
        using Type = std::remove_reference_t<T>;
    };
    template <typename T>
    requires requires { typename std::ranges::sentinel_t<T>; }
    struct SentinelT<T>
    {
        using Type = std::ranges::sentinel_t<T>;
    };

    template <typename T>
    struct ConstSentinelT { };
    template <CharacterPointer T>
    struct ConstSentinelT<T>
    {
        using Type = ToConstPointer<std::remove_reference_t<T>>;
    };
    template <typename T>
#ifdef WCM_CPP23
    requires requires { typename std::ranges::const_sentinel_t<T>; }
#else
    requires requires { typename std::ranges::sentinel_t<T>; }
#endif
    struct ConstSentinelT<T>
    {
#ifdef WCM_CPP23
        using Type = std::ranges::const_sentinel_t<T>;
#else
        using Type = std::ranges::sentinel_t<T>;
#endif
    };

    template <typename T>
        requires requires { typename std::reverse_iterator<T>::iterator_type; }
    struct ReverseIteratorT
    {
        using Type = std::reverse_iterator<T>;
    };

    template <typename T>
    concept Range = CharacterPointer<T> || std::ranges::range<T>;

    template <Range T>
    using Iterator = IteratorT<T>::Type;
    template <Range T>
    using ConstIterator = ConstIteratorT<T>::Type;
    template <Range T>
    using Sentinel = SentinelT<T>::Type;
    template <Range T>
    using ConstSentinel = ConstSentinelT<T>::Type;
    template <typename T>
    using ReverseIterator = ReverseIteratorT<T>::Type;

    struct BeginT
    {
    public:
        template <Range T>
        [[nodiscard]] constexpr Iterator<T> operator()(T &&t) const noexcept(IsNoexcept<T>());
    private:
        template <Range T>
        [[nodiscard]] static constexpr bool IsNoexcept() noexcept;
    };
    struct CBeginT
    {
    public:
        template <Range T>
        [[nodiscard]] constexpr ConstIterator<T> operator()(T &&t) const noexcept(IsNoexcept<T>());
    private:
        template <Range T>
        [[nodiscard]] static constexpr bool IsNoexcept() noexcept;
    };
    struct EndT
    {
    public:
        template <Range T>
        [[nodiscard]] constexpr Sentinel<T> operator()(T &&t) const noexcept(IsNoexcept<T>());
    private:
        template <Range T>
        [[nodiscard]] static constexpr bool IsNoexcept() noexcept;
    };
    struct CEndT
    {
    public:
        template <Range T>
        [[nodiscard]] constexpr ConstSentinel<T> operator()(T &&t) const noexcept(IsNoexcept<T>());
    private:
        template <Range T>
        [[nodiscard]] static constexpr bool IsNoexcept() noexcept;
    };

    struct RBeginT
    {
    public:
        template <Range T>
        [[nodiscard]] constexpr ReverseIterator<Iterator<T>> operator()(T &&t) const noexcept(IsNoexcept<T>());
    private:
        template <Range T>
        [[nodiscard]] static constexpr bool IsNoexcept() noexcept;
    };
    struct CRBeginT
    {
    public:
        template <Range T>
        [[nodiscard]] constexpr ReverseIterator<ConstIterator<T>> operator()(T &&t) const noexcept(IsNoexcept<T>());
    private:
        template <Range T>
        [[nodiscard]] static constexpr bool IsNoexcept() noexcept;
    };
    struct REndT
    {
    public:
        template <Range T>
        [[nodiscard]] constexpr ReverseIterator<Sentinel<T>> operator()(T &&t) const noexcept(IsNoexcept<T>());
    private:
        template <Range T>
        [[nodiscard]] static constexpr bool IsNoexcept() noexcept;
    };
    struct CREndT
    {
    public:
        template <Range T>
        [[nodiscard]] constexpr ReverseIterator<ConstSentinel<T>> operator()(T &&t) const noexcept(IsNoexcept<T>());
    private:
        template <Range T>
        [[nodiscard]] static constexpr bool IsNoexcept() noexcept;
    };

    inline constexpr BeginT Begin{};
    inline constexpr CBeginT CBegin{};
    inline constexpr EndT End{};
    inline constexpr CEndT CEnd{};

    inline constexpr RBeginT RBegin{};
    inline constexpr CRBeginT CRBegin{};
    inline constexpr REndT REnd{};
    inline constexpr CREndT CREnd{};
}

#include "Ranges.inl"
