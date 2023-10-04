/*
// MIT License - Copyright (c) 2023 Alp Can Nalbant
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.
*/

#include <chrono>

#include "Time.hpp"

namespace WCmn::Modules
{
    std::wstring Time::GetDate() const
    {
        const auto now = std::chrono::system_clock::now();
        const std::time_t t_c = std::chrono::system_clock::to_time_t(now);
        const std::wstring dateStr{_wctime64(&t_c)};
        return dateStr.substr(0, dateStr.find_last_of(L'\n'));
    }
}
