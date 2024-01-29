// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

namespace Wcm
{
    template <StringLike T>
    std::shared_ptr<PROCESS_INFORMATION> Execute(const T &app, const T &args)
    {
        typename std::conditional_t<WideCharacter<CharacterOf<T>>, LPWSTR, LPSTR> appStr = nullptr, argsStr;
        if constexpr (const auto command = ToStringView(app), commandLine = ToStringView(args);
                      WideCharacter<CharacterOf<T>>)
        {
            if constexpr (std::same_as<CharacterOf<T>, WCHAR>)
            {
                appStr = const_cast<LPWSTR>(command.data());
                argsStr = const_cast<LPWSTR>(commandLine.data());
            }
            else
            {
                appStr = reinterpret_cast<LPWSTR>(command.data());
                argsStr = reinterpret_cast<LPWSTR>(commandLine.data());
            }
        }
        else
        {
            if constexpr (std::same_as<CharacterOf<T>, CHAR>)
            {
                appStr = const_cast<LPSTR>(command.data());
                argsStr = const_cast<LPSTR>(commandLine.data());
            }
            else
            {
                appStr = reinterpret_cast<LPSTR>(command.data());
                argsStr = reinterpret_cast<LPSTR>(commandLine.data());
            }
        }
        if (!appStr)
        {
            return nullptr;
        }
        return Impl::CreateNewProcess(appStr, argsStr);
    }

    namespace Impl
    {
        template <Character T>
        auto GetProcessInfo()
        {
            const auto pi = std::shared_ptr<PROCESS_INFORMATION>(new PROCESS_INFORMATION, [](auto pi) { CloseHandle(pi->hProcess); CloseHandle(pi->hThread); delete pi; });
            ZeroMemory(pi.get(), sizeof(PROCESS_INFORMATION));
            typename std::conditional_t<WideCharacter<T>, STARTUPINFOW, STARTUPINFOA> si;
            ZeroMemory(&si, sizeof(si));
            si.cb = sizeof(si);
            return std::make_pair(pi, si);
        }
    }
}
