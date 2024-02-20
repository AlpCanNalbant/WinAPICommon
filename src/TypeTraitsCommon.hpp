// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#pragma once
#include <type_traits>

namespace Wcm
{
    template <typename T, typename U = std::remove_cvref_t<std::remove_pointer_t<std::remove_extent_t<T>>>>
    struct RemoveAllT : RemoveAllT<U> { };
    template <typename T>
    struct RemoveAllT<T, T>
    {
        using Type = T;
    };
    template <typename T, typename U = std::remove_volatile_t<std::remove_reference_t<std::remove_pointer_t<std::remove_extent_t<T>>>>>
    struct IsConstT : IsConstT<U> { };
    template <typename T>
    struct IsConstT<T, T> : std::integral_constant<bool, std::is_const_v<T>> { };

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
}
