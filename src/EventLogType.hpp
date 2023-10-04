/*
// MIT License - Copyright (c) 2023 Alp Can Nalbant
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.
*/

#pragma once

#include <windows.h>

namespace WCmn
{
    enum class EventLogType : DWORD
    {
        Error = 1,
        Warning = 2,
        Info = 4,
        Any = Error | Warning | Info
    };
}
