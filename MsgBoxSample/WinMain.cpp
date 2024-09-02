#include "WinAPICommon.hpp"

#ifdef WCM_UNICODE
int wmain([[maybe_unused]] int argc, [[maybe_unused]] wchar_t *argv[])
#elif
int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[])
#endif
{
    Wcm::Log->OutputFile = Wcm::GetBaseDirectory() / "MsgBox.log";
    Wcm::Log->Info("The Message Box process is has been started by the user.");

    HMODULE hInst = GetModuleHandle(NULL);
    LPCTSTR lpszIcon = MAKEINTRESOURCE(1);
    HICON hIcon = reinterpret_cast<HICON>(LoadImage(hInst, lpszIcon, IMAGE_ICON, 128, 128, LR_DEFAULTSIZE));

    auto createMsgBox = [hInst, hIcon](LPCTSTR text, LPCTSTR title, DWORD style, DWORD iconID = 1)
    {
        Wcm::MsgBox(text, title, style, hIcon, hInst, MAKEINTRESOURCE(iconID), MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US));
    };

    // Just create a message box with the 'OK' button..
    createMsgBox(TEXT("Thank you for choosing WinAPICommon library! I hope you're satisfied with our product."),
                 TEXT("Windows API Common Helper Library"),
                 Wcm::MessageBoxStyle::Ok);
    // Same but, 'DarkMode' enabled.
    createMsgBox(TEXT("Thank you for choosing WinAPICommon library! I hope you're satisfied with our product."),
                 TEXT("WinAPICommon (DarkMode)"),
                 Wcm::MessageBoxStyle::Ok | Wcm::MessageBoxStyle::DarkMode);

    // Create another message box with 'Yes/No' buttons.
    createMsgBox(TEXT("Are you sure you do not want to use the WinAPICommon library permanently? This action is irreversible."),
                 TEXT("Awesome WinAPICommon"),
                 Wcm::MessageBoxStyle::YesNo,
                 2);
    // Same but, 'DarkMode' enabled.
    createMsgBox(TEXT("Are you sure you do not want to use the WinAPICommon library permanently? This action is irreversible."),
                 TEXT("WinAPICommon - Library (DarkMode)"),
                 Wcm::MessageBoxStyle::YesNo | Wcm::MessageBoxStyle::DarkMode,
                 3);

    Wcm::Log->Info("The Message Box process is has been ended.");
    return 0;
}
