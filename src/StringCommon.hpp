// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#pragma once
#include <cstring>
#include <concepts>
#include <filesystem>
#include "TypeTraitsCommon.hpp"

namespace Wcm
{
    namespace Impl
    {
        struct GetStringLengthT
        {
            template <CharacterStringAny T>
            [[nodiscard]] constexpr size_t operator()(const T &str) const noexcept;
            [[nodiscard]] size_t operator()(const std::filesystem::path &path) const noexcept;
            template <Character T>
            [[nodiscard]] constexpr size_t operator()(const T *str) const noexcept
                requires (requires { { std::strlen(str) } -> std::same_as<size_t>; } || requires { { std::wcslen(str) } -> std::same_as<size_t>; });
            template <Character T>
            [[nodiscard]] constexpr size_t operator()(const T *const str) const noexcept;
            template <Character T>
            [[nodiscard]] constexpr size_t operator()(const T *const begin, const T *const end) const noexcept;

        private:
            template <Character T>
            [[nodiscard]] constexpr size_t GetLength(const T *const str) const noexcept;
        };
    }

    inline constexpr Impl::GetStringLengthT GetStringLength{};

    template <CharacterPointer T>
    [[nodiscard]] constexpr DWORD GetMultiStringLength(ToConstPointerConst<T> buffer, bool countNullTerminators = false) noexcept;
}

#include "StringCommon.inl"
