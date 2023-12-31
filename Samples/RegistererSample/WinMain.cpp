#include <bitset>

#include "WinAPICommon.hpp"

int wmain([[maybe_unused]] int argc, [[maybe_unused]] wchar_t *argv[])
{
    WinCmn::Log->OutputFile = WinCmn::GetBaseDirectory() + L"\\Registerer.log";
    WinCmn::Log->Info(L"The Registerer process is has been started by the user.");

    WinCmn::Log->Info(L"Registering some values to the Windows registry now...");

    const auto key = WinCmn::OpenRegistryKey(WinCmn::RegistryKeyType::CurrentUser, L"Environment");
    if (!key.IsOpen())
    {
        return 1;
    }

    std::bitset<3> keyValSetFlags;

    const std::wstring key1Name = L"TheRegistererString";
    if (key.SetString(key1Name, WinCmn::RegistryStringType::SZ, L"Test data value."))
    {
        keyValSetFlags.set(0);
    }

    const std::wstring key2Name = L"TheRegistererMultiString";
    if (key.SetString(key2Name, WinCmn::RegistryStringType::MultiSZ, L"Multi test\0string\0data\0 value test.\0"))
    {
        keyValSetFlags.set(1);
    }

    const std::wstring key3Name = L"TheRegistererValue";
    const DWORD value = 3;
    if (key.SetValue(key3Name, WinCmn::RegistryValueType::DWord, reinterpret_cast<LPCBYTE>(&value), sizeof(value)))
    {
        keyValSetFlags.set(2);
    }

    int retCode;
    if (keyValSetFlags.all())
    {
        WinCmn::Log->Info(L"Values successfully registered to the Windows registry.");
        retCode = 0;
    }
    else
    {
        WinCmn::Log->Error(L"Values unsuccessfully registered to the Windows registry.");
        retCode = 1;
    }

    WinCmn::Log->Info(L"The Registerer process is has been ended.");
    return retCode;
}
