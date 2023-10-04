/*
// MIT License - Copyright (c) 2023 Alp Can Nalbant
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.
*/

#pragma once

#include <pathcch.h>

#include "IModule.hpp"
#include "Concepts.hpp"

namespace WCmn::Modules
{
    class Path : public IModule
    {
    public:
        template <Character TChr = wchar_t, template <typename> typename TStr = std::basic_string>
            requires std::derived_from<TStr<TChr>, std::basic_string<TChr>>
        [[nodiscard]] TStr<TChr> GetRoot() const;
    };
}

#include "Path.inl"
