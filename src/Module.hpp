/*
// MIT License - Copyright (c) 2023 Alp Can Nalbant
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.
*/

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
