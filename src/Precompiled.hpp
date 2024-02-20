// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#pragma once

#include <cstddef>
#include <windows.h>
#include <type_traits>
#include <concepts>
#include <utility>
#include <ranges>
#include <string>
#include "Concepts.hpp"
#include "TypeTraits.hpp"

#if defined(UNICODE) || defined(_UNICODE)
    #define WCM_UNICODE
#endif

static_assert(__cplusplus > 201411L, "Minimum allowed language version is c++20");
#if __cplusplus > 202002L
    #define WCM_CPP23
#endif

#define WCM_FUNC_ALIAS_DEFINITION(...)                                               \
        noexcept(noexcept(__VA_ARGS__(std::forward<decltype(args)>(args)...)))       \
            -> decltype(__VA_ARGS__(std::forward<decltype(args)>(args)...))          \
    {                                                                                \
        return __VA_ARGS__(std::forward<decltype(args)>(args)...);                   \
    }
#define WCM_CFUNC_ALIAS(NEW_NAME, ...)                                               \
    auto NEW_NAME(auto &&... args) const                                             \
        WCM_FUNC_ALIAS_DEFINITION(__VA_ARGS__)
#define WCM_FUNC_ALIAS(NEW_NAME, ...)                                                \
    auto NEW_NAME(auto &&... args)                                                   \
        WCM_FUNC_ALIAS_DEFINITION(__VA_ARGS__)
