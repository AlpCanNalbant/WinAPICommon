// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#pragma once

#include <utility>
#include <chrono>

#include "IModule.hpp"
#include "Key.hpp"

namespace WCmn::Modules
{
    class KeySender : public IModule
    {
    public:
        Key Key;
        float Delay{};

        KeySender();
        KeySender(const WCmn::Key key, const float delayInSeconds = 0.0f);

        void ResetTiming();
        bool Send();

    private:
        [[nodiscard]] WCmn::Key GetKeyInput() const;
        void SetKeyInput(const WCmn::Key key);
        bool DelayedSendInput();
        bool SendInput();

        INPUT inputs_[2]{};
        std::chrono::time_point<std::chrono::steady_clock> prev_{std::chrono::steady_clock::now()};
        std::chrono::duration<float> elapsed_{};
    };
}
