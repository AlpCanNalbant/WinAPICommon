// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

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
