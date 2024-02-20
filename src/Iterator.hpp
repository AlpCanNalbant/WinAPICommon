// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#pragma once
#include <ranges>
#include "Concepts.hpp"

namespace Wcm
{
    template <typename T>
    struct IteratorT { };
    template <CharacterPointer T>
    struct IteratorT<T>
    {
        using Type = ToPointer<T>;
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
        using Type = ToConstPointerConst<T>;
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
        using Type = ToPointer<T>;
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
        using Type = ToConstPointerConst<T>;
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
}
