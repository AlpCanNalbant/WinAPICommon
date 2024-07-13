// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

namespace Wcm
{
    template <Character T>
    constexpr DWORD GetMultiStringLength(const T *const buffer, bool countNullTerminators) noexcept
    {
        DWORD len = countNullTerminators;
        for (int i = 0; buffer[i] || buffer[i + 1]; ++i)
        {
            len += static_cast<bool>(buffer[i]) + (!buffer[i] * countNullTerminators);
        }
        return len;
    }

    namespace Impl
    {
        template <CharacterStringAny T>
        constexpr size_t GetStringLengthT::operator()(const T &str) const noexcept
        {
            return str.length();
        }

        template <Character T>
        constexpr size_t GetStringLengthT::operator()(const T *str) const noexcept
            requires(requires { { std::strlen(str) } -> std::same_as<size_t>; } || requires { { std::wcslen(str) } -> std::same_as<size_t>; })
        {
            if (!std::is_constant_evaluated())
            {
                if constexpr (requires { { std::strlen(str) } -> std::same_as<size_t>; })
                {
                    return std::strlen(str);
                }
                else if constexpr (requires { { std::wcslen(str) } -> std::same_as<size_t>; })
                {
                    return std::wcslen(str);
                }
            }
            return GetLength(str);
        }

        template <Character T>
        constexpr size_t GetStringLengthT::operator()(const T *const str) const noexcept
        {
            return GetLength(str);
        }

        template <Character T>
        constexpr size_t GetStringLengthT::operator()(const T *const begin, const T *const end) const noexcept
        {
            return end - begin;
        }

        template <Character T>
        constexpr size_t GetStringLengthT::GetLength(const T *const str) const noexcept
        {
            auto i = 0uz;
            while (str[i])
            {
                ++i;
            }
            return i;
        }
    }
}
