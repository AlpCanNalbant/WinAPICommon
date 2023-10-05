// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#pragma once

#include <memory>
#include <concepts>

#include "IModule.hpp"

namespace WCmn::Modules
{
    template <std::derived_from<Modules::IModule> T>
    struct Module
    {
        T *operator->() const;
    };
}

#include "Module.inl"
