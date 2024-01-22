// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#pragma once
#include <tuple>

namespace Wcm
{
    template <typename T, typename U = std::remove_cvref_t<std::remove_pointer_t<std::remove_extent_t<T>>>>
    struct RemoveAllT : RemoveAllT<U> { };
    template <typename T>
    struct RemoveAllT<T, T>
    {
        using Type = T;
    };

    template <typename T>
    struct ToConstPointerT { };
    template <typename T>
        requires requires { typename std::add_pointer_t<std::add_const_t<std::remove_const_t<std::remove_pointer_t<T>>>>; }
    struct ToConstPointerT<T>
    {
        using Type = std::add_pointer_t<std::add_const_t<std::remove_pointer_t<T>>>;
    };

    template <typename... Types>
    struct IsAllSameT : std::true_type { };
    template <typename First, typename Second, typename ...Rest>
        requires std::same_as<std::remove_cvref_t<First>, std::remove_cvref_t<Second>> && IsAllSameT<std::remove_cvref_t<First>, std::remove_cvref_t<Rest>...>::value
    struct IsAllSameT<First, Second, Rest...> : std::true_type { };
    template <typename First, typename Second, typename ...Rest>
        requires (!(std::same_as<std::remove_cvref_t<First>, std::remove_cvref_t<Second>> && IsAllSameT<std::remove_cvref_t<First>, std::remove_cvref_t<Rest>...>::value))
    struct IsAllSameT<First, Second, Rest...> : std::false_type { };

    template <size_t Count, typename... Types>
    struct IsEqualT : std::false_type { };
    template <size_t Count, typename... Types>
        requires (sizeof...(Types) == Count)
    struct IsEqualT<Count, Types...> : std::true_type { };

    template <size_t Count, typename... Types>
    struct IsGreaterT : std::false_type { };
    template <size_t Count, typename... Types>
        requires (sizeof...(Types) > Count)
    struct IsGreaterT<Count, Types...> : std::true_type { };

    template <size_t Count, typename... Types>
    struct IsGreaterEqualT : std::false_type { };
    template <size_t Count, typename... Types>
        requires (sizeof...(Types) >= Count)
    struct IsGreaterEqualT<Count, Types...> : std::true_type { };

    template <size_t Count, typename... Types>
    struct IsLessT : std::false_type { };
    template <size_t Count, typename... Types>
        requires (sizeof...(Types) < Count)
    struct IsLessT<Count, Types...> : std::true_type { };

    template <size_t Count, typename... Types>
    struct IsLessEqualT : std::false_type { };
    template <size_t Count, typename... Types>
        requires (sizeof...(Types) <= Count)
    struct IsLessEqualT<Count, Types...> : std::true_type { };

    template <size_t Min, size_t Max, typename... Types>
    struct IsInRangeT : std::false_type { };
    template <size_t Min, size_t Max, typename... Types>
        requires (sizeof...(Types) >= Min && sizeof...(Types) <= Max)
    struct IsInRangeT<Min, Max, Types...> : std::true_type { };

    template <typename T>
    struct CharacterOfT { };
    template <typename T>
        requires requires { typename std::remove_cvref_t<T>::value_type; }
    struct CharacterOfT<T>
    {
        using Type = typename std::remove_cvref_t<T>::value_type;
    };
    template <typename T>
        requires CharacterPointer<T> || CharacterArray<T>
    struct CharacterOfT<T>
    {
        using Type = typename RemoveAllT<T>::Type;
    };

    template <size_t Index, typename... Types>
    using At = std::tuple_element_t<Index, std::tuple<Types...>>;
    template <typename... Types>
    using First = At<0, Types...>;
    template <typename... Types>
    using Last = At<sizeof...(Types) - 1, Types...>;
    template <typename T>
    using RemoveAll = RemoveAllT<T>::Type;
    template <typename T>
    using ToConstPointer = ToConstPointerT<T>::Type;
    template <typename T>
    using CharacterOf = CharacterOfT<T>::Type;

    template <typename... Types>
    inline constexpr bool IsAllSame = IsAllSameT<Types...>::value;
    template <size_t Count, typename... Types>
    inline constexpr bool IsEqual = IsEqualT<Count, Types...>::value;
    template <size_t Count, typename... Types>
    inline constexpr bool IsGreater = IsGreaterT<Count, Types...>::value;
    template <size_t Count, typename... Types>
    inline constexpr bool IsGreaterEqual = IsGreaterEqualT<Count, Types...>::value;
    template <size_t Count, typename... Types>
    inline constexpr bool IsLess = IsLessT<Count, Types...>::value;
    template <size_t Count, typename... Types>
    inline constexpr bool IsLessEqual = IsLessEqualT<Count, Types...>::value;
    template <size_t Min, size_t Max, typename... Types>
    inline constexpr bool IsInRange = IsInRangeT<Min, Max, Types...>::value;
}
