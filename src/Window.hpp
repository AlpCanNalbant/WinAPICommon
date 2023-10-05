// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#pragma once

#include <windows.h>

#include "IModule.hpp"

namespace WCmn::Modules
{
    class Window : public IModule
    {
    public:
        void Close(HWND hWnd) const;
    };
}
