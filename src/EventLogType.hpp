// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#pragma once

namespace WinCmn
{
    enum class EventLogType : DWORD
    {
        Error = 1,
        Warning = 2,
        Info = 4,
        Any = Error | Warning | Info
    };
}
