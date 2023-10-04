/*
// MIT License - Copyright (c) 2023 Alp Can Nalbant
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.
*/

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
