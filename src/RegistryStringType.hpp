// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#pragma once

#include <windows.h>

namespace WCmn
{
    enum class RegistryStringType : DWORD
    {
        ExpandSZ = REG_EXPAND_SZ, //	A null-terminated string that contains unexpanded references to environment variables, for example, %PATH%. It's either a Unicode or an ANSI string, depending on whether you use the Unicode or ANSI functions. To expand the environment variable references, use the ExpandEnvironmentStrings function.
        Link = REG_LINK,          //	A null-terminated Unicode string that contains the target path of a symbolic link that was created by calling the RegCreateKeyEx function with REG_OPTION_CREATE_LINK.
        MultiSZ = REG_MULTI_SZ,   //	A sequence of null-terminated strings, terminated by an empty string (\0). The following is an example: String1\0String2\0String3\0LastString\0\0. The first \0 terminates the first string, the second-from-last \0 terminates the last string, and the final \0 terminates the sequence. Note that the final terminator must be factored into the length of the string.
        SZ = REG_SZ               // A null-terminated string. It's either a Unicode or an ANSI string, depending on whether you use the Unicode or ANSI functions.
    };
}
