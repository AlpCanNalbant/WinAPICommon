// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#pragma once

#include <string>

#include "IModule.hpp"

namespace WCmn::Modules
{
    class Time : public IModule
    {
    public:
        [[nodiscard]] std::wstring GetDate() const;
    };
}
