// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

namespace std
{
    constexpr auto begin(Wcm::CharacterPointer auto string) noexcept
    {
        return string;
    }
    constexpr auto cbegin(Wcm::CharacterPointer auto string) noexcept
    {
        return string;
    }
    constexpr auto end(Wcm::CharacterPointer auto string) noexcept
    {
        if constexpr (Wcm::WideCharacterPointer<decltype(string)>)
        {
            return string + wcslen(string);
        }
        else
        {
            return string + strlen(string);
        }
    }
    constexpr auto cend(Wcm::CharacterPointer auto string) noexcept
    {
        return end(string);
    }
}
