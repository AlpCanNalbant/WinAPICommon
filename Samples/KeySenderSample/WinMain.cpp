#include "WinAPICommon.hpp"

int wmain([[maybe_unused]] int argc, [[maybe_unused]] wchar_t *argv[])
{
    Wcm::Log->OutputFile = Wcm::GetBaseDirectory() / "KeySender.log";
    Wcm::Log->Info("The Key Sender process is has been started by the user.");

    Wcm::KeySender keySender;

    keySender.Key = Wcm::Key::BackSpace;

    keySender.Delay = 0.0f;
    if (keySender.Send())
    {
        Wcm::Log->Info("The key is sent.").Sub("KeyCode", std::to_string(static_cast<WORD>(keySender.Key)));
    }

    keySender.Delay = 1.5f;
    for (int counter = 0; counter < 5;)
    {
        if (keySender.Send())
        {
            Wcm::Log->Info("The key is sent.");
            ++counter;
        }
    }

    Wcm::Log->Info("The Key Sender process is has been ended.");
    return 0;
}
