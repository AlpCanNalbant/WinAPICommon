#include "WinAPICommon.hpp"

int wmain([[maybe_unused]] int argc, [[maybe_unused]] wchar_t *argv[])
{
    WCmn::Log->OutputFile = WCmn::Path->GetRoot() + L"\\KeyboardSender.log";
    WCmn::Log->Info(L"The Key Sender process is has been started by the user.");

    WCmn::KeySender->Key = WCmn::Key::BackSpace;

    WCmn::KeySender->Delay = 0.0f;
    if (WCmn::KeySender->Send())
    {
        WCmn::Log->Info(L"Key is sended.");
    }

    WCmn::KeySender->Delay = 1.5f;
    for (int counter = 0; counter < 5;)
    {
        if (WCmn::KeySender->Send())
        {
            WCmn::Log->Info(L"Key is sended.");
            ++counter;
        }
    }

    WCmn::Log->Info(L"The Key Sender process is has been ended.");
    return 0;
}
