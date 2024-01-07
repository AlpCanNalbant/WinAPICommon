// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#include <locale>
#include <codecvt>
#include <sstream>
#include <ios>

#include "String.hpp"

using StringConverter = std::wstring_convert<std::conditional_t<sizeof(wchar_t) == 4, std::codecvt_utf8<wchar_t>, std::codecvt_utf8_utf16<wchar_t>>>;

namespace WinCmn
{
    std::string ToNarrow(const std::wstring &wide)
    {
        return (StringConverter{}).to_bytes(wide);
    }

    std::string ToNarrow(const wchar_t *wide)
    {
        return (StringConverter{}).to_bytes(wide);
    }

    std::wstring ToWide(const std::string &narrow)
    {
        return (StringConverter{}).from_bytes(narrow);
    }

    std::wstring ToWide(const char *narrow)
    {
        return (StringConverter{}).from_bytes(narrow);
    }
}
