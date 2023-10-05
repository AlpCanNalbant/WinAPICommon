// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

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
        return SendInput();
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
            const auto isSended = SendInput();
            elapsed_ = {};
            return isSended;
        }

        return false;
    }

    bool KeySender::SendInput()
    {
        SetKeyInput(this->Key);
        const UINT sentResult = ::SendInput(ARRAYSIZE(inputs_), inputs_, sizeof(INPUT));
        if (sentResult != ARRAYSIZE(inputs_))
        {
            WCmn::Log->Error(L"SendInput failed to send.", GetLastError());
            return false;
        }
        return true;
    }
}
