/*
// MIT License - Copyright (c) 2023 Alp Can Nalbant
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.
*/

#pragma once

#include <windows.h>
#include <string>

#include "IModule.hpp"
#include "Concepts.hpp"
#include "StringLoopTerminator.hpp"

namespace WCmn::Modules
{
    class String : public IModule
    {
    public:
        template <Character T>
        [[nodiscard]] T *ToBuffer(const std::basic_string<T> &str) const;
        template <Character T>
        [[nodiscard]] DWORD GetLength(const T *string, bool isMulti = false) const;
        template <Character T>
        [[nodiscard]] DWORD GetLengthCountNulls(const T *string, bool isMulti = false) const;

    private:
        template <Character T>
        [[nodiscard]] DWORD GetLength(const T *string, bool isCountNulls, bool isMulti) const;

        WCmn::StringLoopTerminator<wchar_t, true> slt_;
    };
}

#include "String.inl"
