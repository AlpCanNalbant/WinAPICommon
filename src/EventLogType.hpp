// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

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
