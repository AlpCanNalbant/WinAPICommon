#include <bitset>

#include "WinAPICommon.hpp"

int wmain([[maybe_unused]] int argc, [[maybe_unused]] wchar_t *argv[])
{
    WCmn::Log->OutputFile = WCmn::Path->GetRoot() + L"\\Registerer.log";
    WCmn::Log->Info(L"The Registerer process is has been started by the user.");

    WCmn::Log->Info(L"Registering some values to the Windows registry now...");
    const auto key = WCmn::Registry->OpenKey(WCmn::RegistryKeyType::CurrentUser, L"Environment");
    if (!key.IsOpen())
    {
        return 1;
    }

    std::bitset<3> keyValSetFlags;

    const std::wstring key1Name = L"TheRegistererString";
    if (key.SetString(key1Name, WCmn::RegistryStringType::SZ, L"Test data value."))
    {
        keyValSetFlags.set(0);
    }

    const std::wstring key2Name = L"TheRegistererMultiString";
    if (key.SetString(key2Name, WCmn::RegistryStringType::MultiSZ, L"Multi\0string\0data\0value test.\0"))
    {
        keyValSetFlags.set(1);
    }

    const std::wstring key3Name = L"TheRegistererValue";
    const DWORD value = 3;
    if (key.SetValue(key3Name, WCmn::RegistryValueType::DWord, reinterpret_cast<LPCBYTE>(&value), sizeof(value)))
    {
        keyValSetFlags.set(2);
    }

    int retCode;
    if (keyValSetFlags.all())
    {
        WCmn::Log->Info(L"Values successfully registered to the Windows registry.");
        retCode = 0;
    }
    else
    {
        WCmn::Log->Error(L"Values unsuccessfully registered to the Windows registry.");
        retCode = 1;
    }

    WCmn::Log->Info(L"The Registerer process is has been ended.");
    return retCode;
}
