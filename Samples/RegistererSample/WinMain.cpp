#include <bitset>

#include "WinAPICommon.hpp"

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[])
{
    Wcm::Log->OutputFile = Wcm::GetBaseDirectory() / "Registerer.log";
    Wcm::Log->Info("The Registerer process is has been started by the user.");

    Wcm::Log->Info("Registering some values to the Windows registry now...");

    const auto key = Wcm::OpenRegistryKey(Wcm::RegistryKeyType::CurrentUser, L"Environment");
    if (!key.IsOpen())
    {
        return 1;
    }

    std::bitset<3> keyValSetFlags;

    const std::wstring key1Name = L"TheRegistererString";
    if (key.SetString(key1Name, Wcm::RegistryStringType::SZ, L"Test data value."))
    {
        keyValSetFlags.set(0);
    }

    const std::wstring key2Name = L"TheRegistererMultiString";
    if (key.SetString(key2Name, Wcm::RegistryStringType::MultiSZ, L"Multi test\0string\0data\0 value test.\0"))
    {
        keyValSetFlags.set(1);
    }

    const std::wstring key3Name = L"TheRegistererValue";
    const DWORD value = 3;
    if (key.SetValue(key3Name, Wcm::RegistryValueType::DWord, reinterpret_cast<LPCBYTE>(&value), sizeof(value)))
    {
        keyValSetFlags.set(2);
    }

    int retCode;
    if (keyValSetFlags.all())
    {
        Wcm::Log->Info("Values successfully registered to the Windows registry.");
        retCode = 0;
    }
    else
    {
        Wcm::Log->Error("Values unsuccessfully registered to the Windows registry.");
        retCode = 1;
    }

    Wcm::Log->Info("The Registerer process is has been ended.");
    return retCode;
}
