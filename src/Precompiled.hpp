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
