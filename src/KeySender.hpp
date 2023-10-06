// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#pragma once

#include <chrono>

#include "Key.hpp"

namespace WinCmn
{
    class KeySender
    {
    public:
        Key Key;
        float Delay;

        KeySender();
        KeySender(const WinCmn::Key key, const float delayInSeconds = 0.0f);

        void ResetTiming() const noexcept;
        bool Send() const noexcept;

    private:
        bool DelayedSendInput() const noexcept;
        bool SendInput() const noexcept;

        mutable INPUT inputs_[2]{};
        mutable std::chrono::time_point<std::chrono::steady_clock> prev_{std::chrono::steady_clock::now()};
        mutable std::chrono::duration<float> elapsed_{};
    };
}
