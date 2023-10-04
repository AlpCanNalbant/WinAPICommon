/*
// MIT License - Copyright (c) 2023 Alp Can Nalbant
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.
*/

#include "KeySender.hpp"
#include "Log.hpp"

namespace WCmn::Modules
{
    KeySender::KeySender()
        : KeySender({}, {}) {}

    KeySender::KeySender(const WCmn::Key key, const float delayInSeconds)
        : Key{key}, Delay{delayInSeconds}
    {
        ZeroMemory(inputs_, sizeof(inputs_));
        inputs_[0].type = INPUT_KEYBOARD;
        inputs_[1].type = INPUT_KEYBOARD;
        inputs_[1].ki.dwFlags = KEYEVENTF_KEYUP;
    }

    void KeySender::ResetTiming()
    {
        prev_ = std::chrono::steady_clock::now();
        elapsed_ = {};
    }

    bool KeySender::Send()
    {
        if (Delay != 0.0f)
        {
            return DelayedSendInput();
        }
        return MSendInput();
    }

    Key KeySender::GetKeyInput() const
    {
        return static_cast<WCmn::Key>(inputs_[0].ki.wVk);
    }

    void KeySender::SetKeyInput(const WCmn::Key key)
    {
        inputs_[0].ki.wVk = inputs_[1].ki.wVk = static_cast<WORD>(key);
    }

    bool KeySender::DelayedSendInput()
    {
        const auto now = std::chrono::steady_clock::now();
        elapsed_ += (now - prev_);
        prev_ = now;

        if (elapsed_.count() > Delay)
        {
            const auto isSended = MSendInput();
            elapsed_ = {};
            return isSended;
        }

        return false;
    }

    bool KeySender::MSendInput()
    {
        SetKeyInput(this->Key);
        const UINT sentResult = SendInput(ARRAYSIZE(inputs_), inputs_, sizeof(INPUT));
        if (sentResult != ARRAYSIZE(inputs_))
        {
            WCmn::Log->Error(L"SendInput failed to send.", GetLastError());
            return false;
        }
        return true;
    }
}
