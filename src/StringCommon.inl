// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

namespace Wcm
{
    template <CharacterPointer T>
    constexpr size_t GetStringLength(const T str) noexcept
    {
        if constexpr (requires { { std::strlen(str) } -> std::same_as<size_t>; })
        {
            return std::strlen(str);
        }
        else if constexpr (requires { { std::wcslen(str) } -> std::same_as<size_t>; })
        {
            return std::wcslen(str);
        }
        else
        {
            auto i = 0uz;
            while (str[i])
            {
                ++i;
            }
            return i;
        }
    }

    template <Character T, size_t Size>
    constexpr size_t GetStringLength(const T(&)[Size]) noexcept
    {
        return Size - 1;
    }
}
