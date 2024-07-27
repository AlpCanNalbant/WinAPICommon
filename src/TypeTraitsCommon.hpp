// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#pragma once
#include <type_traits>

namespace Wcm
{
    template <typename T, typename U = std::remove_cvref_t<std::remove_pointer_t<std::remove_extent_t<T>>>>
    struct RemoveAllT : RemoveAllT<U>
    {
    };
    template <typename T>
    struct RemoveAllT<T, T>
    {
        using Type = T;
    };
    template <typename T, typename U = std::remove_volatile_t<std::remove_reference_t<std::remove_pointer_t<std::remove_extent_t<T>>>>>
    struct IsConstT : IsConstT<U>
    {
    };
    template <typename T>
    struct IsConstT<T, T> : std::integral_constant<bool, std::is_const_v<T>>
    {
    };

    template <typename E>
    struct UnderlyingTypeIf
    {
    };
    template <typename E>
    requires std::integral<E>
    struct UnderlyingTypeIf<E>
    {
        using Type = std::remove_reference_t<E>;
    };
    template <typename E>
    requires std::is_enum_v<E>
    struct UnderlyingTypeIf<E>
    {
        using Type = std::remove_reference_t<std::underlying_type_t<E>>;
    };

    template <typename... EnumLikes>
    using UnderlyingCommonType = std::common_type_t<typename UnderlyingTypeIf<EnumLikes>::Type...>;

    template <typename T>
    using RemoveAll = RemoveAllT<T>::Type;

    template <typename T>
    using ToPointer = std::add_pointer_t<RemoveAll<T>>;
    template <typename T>
    using ToReference = std::add_lvalue_reference_t<RemoveAll<T>>;
    template <typename T>
    using ToConstReference = std::add_lvalue_reference_t<std::add_const_t<RemoveAll<T>>>;
    template <typename T>
    using ToConstPointer = std::add_pointer_t<std::add_const_t<RemoveAll<T>>>;
    template <typename T>
    using ToConstPointerConst = std::add_const_t<ToConstPointer<T>>;

    template <typename T>
    inline constexpr bool IsConst = IsConstT<T>::value;

    template <typename E1, typename E2>
    requires std::integral<UnderlyingCommonType<E1, E2>>
    constexpr bool HasFlag(const E1 lhs, const E2 rhs) noexcept;
}

#include "TypeTraitsCommon.inl"
