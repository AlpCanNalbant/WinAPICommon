// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#pragma once

#include <cstddef>
#include <utility>
#include <string_view>
#include <type_traits>
#include <concepts>
#include <filesystem>
#include <tuple>

namespace Wcm
{
    namespace Impl
    {
        template <typename T, typename U = std::remove_cvref_t<std::remove_pointer_t<std::remove_extent_t<T>>>>
        struct RemoveAll : RemoveAll<U> { };
        template <typename T>
        struct RemoveAll<T, T> { typedef T Type; };
    }

    template <size_t Index, typename... Types>
    using TypeAt = typename std::tuple_element_t<Index, std::tuple<Types...>>;
    template <typename... Types>
    using First = TypeAt<0, Types...>;
    template <typename... Types>
    using Last = TypeAt<sizeof...(Types) - 1, Types...>;
    template <typename T>
    using RemoveAll = typename Impl::RemoveAll<T>::Type;

    template <typename... Types>
    inline constexpr bool IsSame = true;
    template <typename First, typename Second, typename ...Rest>
    inline constexpr bool IsSame<First, Second, Rest...> = std::same_as<std::remove_cvref_t<First>, std::remove_cvref_t<Second>> && IsSame<std::remove_cvref_t<First>, std::remove_cvref_t<Rest>...>;

    template <size_t Count, typename... Types>
    inline constexpr bool IsEqual = sizeof...(Types) == Count;
    template <size_t Count, typename... Types>
    inline constexpr bool IsGreater = sizeof...(Types) > Count;
    template <size_t Count, typename... Types>
    inline constexpr bool IsGreaterEqual = sizeof...(Types) >= Count;
    template <size_t Count, typename... Types>
    inline constexpr bool IsLess = sizeof...(Types) < Count;
    template <size_t Count, typename... Types>
    inline constexpr bool IsLessEqual = sizeof...(Types) <= Count;
    template <size_t Min, size_t Max, typename... Types>
    inline constexpr bool IsInRange = sizeof...(Types) >= Min && sizeof...(Types) <= Max;

    template <typename T>
    concept ByteCharacter =
        std::disjunction_v<std::is_same<std::remove_cvref_t<T>, char>, std::is_same<std::remove_cvref_t<T>, char8_t>>;
    template <typename T>
    concept WideCharacter =
        std::disjunction_v<std::is_same<std::remove_cvref_t<T>, wchar_t>, std::is_same<std::remove_cvref_t<T>, char16_t>,
                           std::is_same<std::remove_cvref_t<T>, char32_t>>;
    template <typename T>
    concept Character = ByteCharacter<T> || WideCharacter<T>;

    template <typename T>
    concept ByteCharacterPointer = std::is_pointer_v<std::remove_cvref_t<T>> && ByteCharacter<std::remove_pointer_t<std::remove_cvref_t<T>>>;
    template <typename T>
    concept WideCharacterPointer = std::is_pointer_v<std::remove_cvref_t<T>> && WideCharacter<std::remove_pointer_t<std::remove_cvref_t<T>>>;
    template <typename T>
    concept CharacterPointer = ByteCharacterPointer<T> || WideCharacterPointer<T>;

    template <typename T>
    concept ByteCharacterArray = std::is_array_v<T> && ByteCharacter<std::remove_extent_t<T>>;
    template <typename T>
    concept WideCharacterArray = std::is_array_v<T> && WideCharacter<std::remove_extent_t<T>>;
    template <typename T>
    concept CharacterArray = ByteCharacterArray<T> || WideCharacterArray<T>;

    template <typename T>
    concept ByteCharacterString =
        std::same_as<std::remove_cvref_t<T>, std::basic_string<typename std::remove_cvref_t<T>::value_type>> &&
        requires
        {
            { std::declval<typename std::remove_cvref_t<T>::value_type>() } -> ByteCharacter;
        };
    template <typename T>
    concept WideCharacterString =
        std::same_as<std::remove_cvref_t<T>, std::basic_string<typename std::remove_cvref_t<T>::value_type>> &&
        requires
        {
            { std::declval<typename std::remove_cvref_t<T>::value_type>() } -> WideCharacter;
        };
    template <typename T>
    concept CharacterString = ByteCharacterString<T> || WideCharacterString<T>;

    template <typename T>
    concept ByteCharacterStringView =
        std::same_as<std::remove_cvref_t<T>, std::basic_string_view<typename std::remove_cvref_t<T>::value_type>> &&
        requires
        {
            { std::declval<typename std::remove_cvref_t<T>::value_type>() } -> ByteCharacter;
        };
    template <typename T>
    concept WideCharacterStringView =
        std::same_as<std::remove_cvref_t<T>, std::basic_string_view<typename std::remove_cvref_t<T>::value_type>> &&
        requires
        {
            { std::declval<typename std::remove_cvref_t<T>::value_type>() } -> WideCharacter;
        };
    template <typename T>
    concept CharacterStringView = ByteCharacterStringView<T> || WideCharacterStringView<T>;

    template <typename T>
    concept ByteCharacterStringAny =
        ByteCharacterString<T> || ByteCharacterStringView<T>;
    template <typename T>
    concept WideCharacterStringAny =
        WideCharacterString<T> || WideCharacterStringView<T>;
    template <typename T>
    concept CharacterStringAny = ByteCharacterStringAny<T> || WideCharacterStringAny<T>;

    template <typename T>
    concept ByteCharacterStringLike =
        ByteCharacterArray<T> || ByteCharacterPointer<T> || ByteCharacterString<T> || ByteCharacterStringView<T>;
    template <typename T>
    concept WideCharacterStringLike =
        WideCharacterArray<T> || WideCharacterPointer<T> || WideCharacterString<T> || WideCharacterStringView<T>;
    template <typename T>
    concept StringLike = CharacterArray<T> || std::constructible_from<std::filesystem::path, T>;
}

namespace std
{
    constexpr auto begin(Wcm::CharacterPointer auto string) noexcept;
    constexpr auto cbegin(Wcm::CharacterPointer auto string) noexcept;
    constexpr auto end(Wcm::CharacterPointer auto string) noexcept;
    constexpr auto cend(Wcm::CharacterPointer auto string) noexcept;
}

#include "Concepts.inl"
