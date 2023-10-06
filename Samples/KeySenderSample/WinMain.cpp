#include "WinAPICommon.hpp"

int wmain([[maybe_unused]] int argc, [[maybe_unused]] wchar_t *argv[])
{
    WinCmn::Log->OutputFile = WinCmn::GetBaseDirectory() + L"\\KeySender.log";
    WinCmn::Log->Info(L"The Key Sender process is has been started by the user.");

    WinCmn::KeySender keySender;

    keySender.Key = WinCmn::Key::BackSpace;

    keySender.Delay = 0.0f;
    if (keySender.Send())
    {
        WinCmn::Log->Info(L"Key is sended.");
    }

    keySender.Delay = 1.5f;
    for (int counter = 0; counter < 5;)
    {
        if (keySender.Send())
        {
            WinCmn::Log->Info(L"Key is sended.");
            ++counter;
        }
    }

    WinCmn::Log->Info(L"The Key Sender process is has been ended.");
    return 0;
}
