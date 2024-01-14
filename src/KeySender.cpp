// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#include "KeySender.hpp"
#include "Log.hpp"

namespace WinCmn
{
    KeySender::KeySender()
        : KeySender({}, {}) {}

    KeySender::KeySender(const WinCmn::Key key, const float delayInSeconds)
        : Key{key}, Delay{delayInSeconds}
    {
        ZeroMemory(inputs_, sizeof(inputs_));
        inputs_[0].type = INPUT_KEYBOARD;
        inputs_[1].type = INPUT_KEYBOARD;
        inputs_[1].ki.dwFlags = KEYEVENTF_KEYUP;
    }

    void KeySender::ResetTiming() const noexcept
    {
        prev_ = std::chrono::steady_clock::now();
        elapsed_ = {};
    }

    bool KeySender::Send() const noexcept
    {
        if (Delay != 0.0f)
        {
            return DelayedSendInput();
        }
        return SendInput();
    }

    bool KeySender::DelayedSendInput() const noexcept
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

    bool KeySender::SendInput() const noexcept
    {
        inputs_[0].ki.wVk = inputs_[1].ki.wVk = static_cast<WORD>(Key);
        const UINT sentResult = ::SendInput(ARRAYSIZE(inputs_), const_cast<LPINPUT>(inputs_), sizeof(INPUT));
        if (sentResult != ARRAYSIZE(inputs_))
        {
            Log->Error(L"SendInput failed to send.", GetLastError());
            return false;
        }
        return true;
    }
}
