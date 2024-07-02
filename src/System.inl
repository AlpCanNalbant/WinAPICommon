// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

namespace Wcm
{
    std::shared_ptr<PROCESS_INFORMATION> Execute(const StringLike auto &app, const StringLike auto &args, DWORD creationFlags)
        requires((WideCharacter<CharacterOf<decltype(app)>> && WideCharacter<CharacterOf<decltype(args)>>) ||
                 (ByteCharacter<CharacterOf<decltype(app)>> && ByteCharacter<CharacterOf<decltype(args)>>))
    {
        const auto command = ToStringView(app);
        const auto commandLine = ToStringView(args);

        typename std::conditional_t<WideCharacter<CharacterOf<decltype(command)>>, LPWSTR, LPSTR> appStr = nullptr;
        typename std::conditional_t<WideCharacter<CharacterOf<decltype(commandLine)>>, LPWSTR, LPSTR> argsStr = nullptr;

        constexpr auto castString = [](CharacterPointer auto &destStr, const CharacterStringView auto &sourceStrView)
        {
            if constexpr (WideCharacter<CharacterOf<decltype(sourceStrView)>>)
            {
                if constexpr (std::same_as<CharacterOf<decltype(sourceStrView)>, WCHAR>)
                {
                    destStr = const_cast<LPWSTR>(sourceStrView.data());
                }
                else
                {
                    destStr = reinterpret_cast<LPWSTR>(sourceStrView.data());
                }
            }
            else
            {
                if constexpr (std::same_as<CharacterOf<decltype(sourceStrView)>, CHAR>)
                {
                    destStr = const_cast<LPSTR>(sourceStrView.data());
                }
                else
                {
                    destStr = reinterpret_cast<LPSTR>(sourceStrView.data());
                }
            }
        };
        castString(appStr, command);
        castString(argsStr, commandLine);

        return Impl::CreateNewProcess(appStr, (!commandLine.empty()) ? argsStr : NULL, creationFlags);
    }

    std::shared_ptr<PROCESS_INFORMATION> Execute(const StringLike auto &app, DWORD sessionId, const StringLike auto &args, DWORD creationFlags)
        requires((WideCharacter<CharacterOf<decltype(app)>> && WideCharacter<CharacterOf<decltype(args)>>) ||
                 (ByteCharacter<CharacterOf<decltype(app)>> && ByteCharacter<CharacterOf<decltype(args)>>))
    {
        HANDLE hToken;
        if (!WTSQueryUserToken(sessionId, &hToken))
        {
            Wcm::Log->Error("Obtaining the primary access token of the logged-on user specified by the session ID is failed.", GetLastError());
            return nullptr;
        }

        LPVOID lpEnvironment = NULL;
        if (!CreateEnvironmentBlock(&lpEnvironment, hToken, TRUE))
        {
            Wcm::Log->Error("Retrieving the environment variables for the specified user is failed.", GetLastError());
            CloseHandle(hToken);
            return nullptr;
        }

        TOKEN_LINKED_TOKEN lto;
        DWORD nbytes;

        WINBOOL res = GetTokenInformation(hToken, TokenLinkedToken, &lto, sizeof(lto), &nbytes);
        CloseHandle(hToken);

        if (res)
        {
            hToken = lto.LinkedToken;
        }
        else
        {
            Wcm::Log->Error("Access token information is cannot obtained.", GetLastError());
            return nullptr;
        }

        const auto command = ToStringView(app);
        const auto commandLine = ToStringView(args);

        typename std::conditional_t<WideCharacter<CharacterOf<decltype(command)>>, LPWSTR, LPSTR> appStr = nullptr;
        typename std::conditional_t<WideCharacter<CharacterOf<decltype(commandLine)>>, LPWSTR, LPSTR> argsStr = nullptr;

        constexpr auto castString = [](CharacterPointer auto &destStr, const CharacterStringView auto &sourceStrView)
        {
            if constexpr (WideCharacter<CharacterOf<decltype(sourceStrView)>>)
            {
                if constexpr (std::same_as<CharacterOf<decltype(sourceStrView)>, WCHAR>)
                {
                    destStr = const_cast<LPWSTR>(sourceStrView.data());
                }
                else
                {
                    destStr = reinterpret_cast<LPWSTR>(sourceStrView.data());
                }
            }
            else
            {
                if constexpr (std::same_as<CharacterOf<decltype(sourceStrView)>, CHAR>)
                {
                    destStr = const_cast<LPSTR>(sourceStrView.data());
                }
                else
                {
                    destStr = reinterpret_cast<LPSTR>(sourceStrView.data());
                }
            }
        };

        castString(appStr, command);
        castString(argsStr, commandLine);

        auto procInfo = Impl::CreateNewProcess(hToken, appStr, (!commandLine.empty()) ? argsStr : NULL, creationFlags);

        DestroyEnvironmentBlock(lpEnvironment);
        CloseHandle(hToken);

        if (!procInfo)
        {
            Wcm::Log->Error(std::wstring{L"Creating the new process is failed. "} + L"Application: " + appStr + L" Paramaters: " + argsStr, GetLastError()); // .Sub("Application", appName).Sub("Paramaters", commandLine);
            return nullptr;
        }

        return procInfo.first;
    }

    template <StringLike T>
    std::shared_ptr<void> RunCommand(const T &command, HWND hWnd, bool runAsAdmin)
    {
        typename std::conditional_t<WideCharacter<CharacterOf<T>>, LPCWSTR, LPCSTR> lpParams;
        const auto commandView = ToStringView(command);

        if constexpr (WideCharacter<CharacterOf<T>> && (!std::same_as<CharacterOf<T>, WCHAR>))
        {
            lpParams = reinterpret_cast<LPCWSTR>(commandView.data());
        }
        else if constexpr (ByteCharacter<CharacterOf<T>> && (!std::same_as<CharacterOf<T>, CHAR>))
        {
            lpParams = reinterpret_cast<LPCSTR>(commandView.data());
        }
        else
        {
            lpParams = commandView.data();
        }

        typename std::conditional_t<WideCharacter<CharacterOf<T>>, SHELLEXECUTEINFOW, SHELLEXECUTEINFOA> shExInfo;
        ZeroMemory(&shExInfo, sizeof(shExInfo));
        shExInfo.cbSize = sizeof(shExInfo);
        shExInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
        shExInfo.hwnd = hWnd;
        shExInfo.lpVerb = !runAsAdmin ? TEXT("open") : TEXT("runas");
        shExInfo.lpFile = TEXT("cmd.exe");
        shExInfo.lpParameters = lpParams;
        shExInfo.lpDirectory = NULL;
        shExInfo.nShow = SW_SHOW;
        shExInfo.hInstApp = NULL;

        auto hProcess = std::shared_ptr<void>(shExInfo.hProcess, [](auto hProcessRawPtr)
                                              { CloseHandle(hProcessRawPtr); });
        if (!ShellExecuteEx(&shExInfo))
        {
            Log->Error("Running of command is failed.", GetLastError()).Sub("Command", lpParams);
            return nullptr;
        }
        return hProcess;
    }
    template <StringLike T>
    std::shared_ptr<void> RunCommand(const T &command, bool runAsAdmin)
    {
        return RunCommand(command, NULL, runAsAdmin);
    }

    namespace Impl
    {
        template <Character T>
        auto GetProcessInfo()
        {
            const auto pi = std::shared_ptr<PROCESS_INFORMATION>(new PROCESS_INFORMATION, [](auto p)
                                                                 { CloseHandle(p->hProcess); CloseHandle(p->hThread); delete p; });
            ZeroMemory(pi.get(), sizeof(PROCESS_INFORMATION));
            typename std::conditional_t<WideCharacter<T>, STARTUPINFOW, STARTUPINFOA> si;
            ZeroMemory(&si, sizeof(si));
            si.cb = sizeof(si);
            return std::make_pair(pi, si);
        }
    }
}
