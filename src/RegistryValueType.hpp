/*
// MIT License - Copyright (c) 2023 Alp Can Nalbant
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.
*/

#pragma once

#include "RegistryStringType.hpp"

namespace WCmn
{
    enum class RegistryValueType : DWORD
    {
        Binary = REG_BINARY,                         //	Binary data in any form.
        DWord = REG_DWORD,                           //	A 32-bit number.
        DWordLittleEndian = REG_DWORD_LITTLE_ENDIAN, //	A 32-bit number in little-endian format. Windows is designed to run on little-endian computer architectures. Therefore, this value is defined as REG_DWORD in the Windows header files.
        DWordBigEndian = REG_DWORD_BIG_ENDIAN,       //	A 32-bit number in big-endian format. Some UNIX systems support big-endian architectures.
        ExpandSZ = static_cast<DWORD>(RegistryStringType::ExpandSZ),
        Link = static_cast<DWORD>(RegistryStringType::Link),
        MultiSZ = static_cast<DWORD>(RegistryStringType::MultiSZ),
        None = REG_NONE,                             //	No defined value type.
        QWord = REG_QWORD,                           //	A 64-bit number.
        QWordLittleEndian = REG_QWORD_LITTLE_ENDIAN, //	A 64-bit number in little-endian format. Windows is designed to run on little-endian computer architectures. Therefore, this value is defined as REG_QWORD in the Windows header files.
        SZ = static_cast<DWORD>(RegistryStringType::SZ)
    };
}
