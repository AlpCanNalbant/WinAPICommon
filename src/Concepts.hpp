// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#pragma once
#include <concepts>
#include <type_traits>
#include <string_view>
#include <filesystem>

namespace Wcm
{
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
    concept Array = std::is_bounded_array_v<std::remove_cvref_t<T>>;

    template <typename T>
    concept ByteCharacterArray = Array<T> && ByteCharacter<std::remove_extent_t<std::remove_cvref_t<T>>>;
    template <typename T>
    concept WideCharacterArray = Array<T> && WideCharacter<std::remove_extent_t<std::remove_cvref_t<T>>>;
    template <typename T>
    concept CharacterArray = ByteCharacterArray<T> || WideCharacterArray<T>;

    template <typename T>
    concept ByteCharacterPointer = std::is_pointer_v<std::remove_cvref_t<T>> && ByteCharacter<std::remove_pointer_t<std::remove_cvref_t<T>>>;
    template <typename T>
    concept WideCharacterPointer = std::is_pointer_v<std::remove_cvref_t<T>> && WideCharacter<std::remove_pointer_t<std::remove_cvref_t<T>>>;
    template <typename T>
    concept CharacterPointer = ByteCharacterPointer<T> || WideCharacterPointer<T>;

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
    concept ByteCharacterRawString =
        ByteCharacterArray<T> || ByteCharacterPointer<T>;
    template <typename T>
    concept WideCharacterRawString =
        WideCharacterArray<T> || WideCharacterPointer<T>;
    template <typename T>
    concept CharacterRawString = ByteCharacterRawString<T> || WideCharacterRawString<T>;

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
        ByteCharacterRawString<T> || ByteCharacterStringAny<T>;
    template <typename T>
    concept WideCharacterStringLike =
        WideCharacterRawString<T> || WideCharacterStringAny<T>;
    template <typename T>
    concept CharacterStringLike =
        ByteCharacterStringLike<T> || WideCharacterStringLike<T>;
    template <typename T>
    concept StringLike = CharacterArray<T> || std::constructible_from<std::filesystem::path, T>;
}
