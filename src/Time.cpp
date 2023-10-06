// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#include <chrono>

#include "Time.hpp"

namespace WinCmn
{
    std::wstring GetDate()
    {
        const auto now = std::chrono::system_clock::now();
        const std::time_t t_c = std::chrono::system_clock::to_time_t(now);
        const std::wstring dateStr{_wctime64(&t_c)};
        return dateStr.substr(0, dateStr.find_last_of(L'\n'));
    }
}
