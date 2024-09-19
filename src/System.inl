// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

namespace Wcm
{
    template <Character T>
    bool EnablePrivilegeValue(const T *const lpszPrivilege, bool bEnablePrivilege)
    {
        HANDLE hToken;
        LUID luid;
        TOKEN_PRIVILEGES tp;
        bool isSuccess = true;

        if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
        {
            Log->Error("OpenProcessToken error.", GetLastError());
            isSuccess = false;
        }

        if (!LookupPrivilegeValue(NULL, lpszPrivilege, &luid)) // SE_DEBUG_NAME
        {
            Log->Error("LookupPrivilegeValue error.", GetLastError());
            isSuccess = false;
        }

        tp.PrivilegeCount = 1;
        tp.Privileges[0].Luid = luid;
        tp.Privileges[0].Attributes = bEnablePrivilege ? SE_PRIVILEGE_ENABLED : 0;

        if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES)NULL, (PDWORD)NULL))
        {
            Log->Error("AdjustTokenPrivileges error.", GetLastError());
            isSuccess = false;
        }

        if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)
        {
            Log->Error("The token does not have the specified privilege.", ERROR_NOT_ALL_ASSIGNED);
            isSuccess = false;
        }
        if (hToken)
        {
            CloseHandle(hToken);
        }

        return isSuccess;
    }

    std::shared_ptr<PROCESS_INFORMATION> Execute(const StringLike auto &app, const StringLike auto &args, DWORD creationFlags)
        requires((WideCharacter<CharacterOf<decltype(app)>> && WideCharacter<CharacterOf<decltype(args)>>) ||
                 (ByteCharacter<CharacterOf<decltype(app)>> && ByteCharacter<CharacterOf<decltype(args)>>))
    {
        const auto command = ToStringView(app);
        const auto commandLine = ToStringView(args);

        typename std::conditional_t<WideCharacter<CharacterOf<decltype(command)>>, LPWSTR, LPSTR> appStr = nullptr;
        typename std::conditional_t<WideCharacter<CharacterOf<decltype(commandLine)>>, LPWSTR, LPSTR> argsStr = nullptr;

        Impl::CastString(appStr, command);
        Impl::CastString(argsStr, commandLine);

        return Impl::CreateNewProcess((!command.empty()) ? appStr : NULL, (!commandLine.empty()) ? argsStr : NULL, creationFlags);
    }

    std::shared_ptr<PROCESS_INFORMATION> Execute(const StringLike auto &app, DWORD sessionId, const StringLike auto &args, bool isInteractive, DWORD creationFlags)
        requires((WideCharacter<CharacterOf<decltype(app)>> && WideCharacter<CharacterOf<decltype(args)>>) ||
                 (ByteCharacter<CharacterOf<decltype(app)>> && ByteCharacter<CharacterOf<decltype(args)>>))
    {
        HANDLE hToken;
        if (!WTSQueryUserToken(sessionId, &hToken))
        {
            Log->Error("Obtaining the primary access token of the logged-on user specified by the session ID is failed.", GetLastError());
            return nullptr;
        }

        LPVOID lpEnvironment = NULL;
        if (!CreateEnvironmentBlock(&lpEnvironment, hToken, TRUE))
        {
            Log->Error("Retrieving the environment variables for the specified user is failed.", GetLastError());
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
            Log->Error("Access token information is cannot obtained.", GetLastError());
            return nullptr;
        }

        const auto command = ToStringView(app);
        const auto commandLine = ToStringView(args);

        typename std::conditional_t<WideCharacter<CharacterOf<decltype(command)>>, LPWSTR, LPSTR> appStr = nullptr;
        typename std::conditional_t<WideCharacter<CharacterOf<decltype(commandLine)>>, LPWSTR, LPSTR> argsStr = nullptr;

        Impl::CastString(appStr, command);
        Impl::CastString(argsStr, commandLine);

        auto procInfo = Impl::CreateNewProcess(hToken, (!command.empty()) ? appStr : NULL, (!commandLine.empty()) ? argsStr : NULL, isInteractive ? TEXT("winsta0\\default") : TEXT(""), creationFlags);

        DestroyEnvironmentBlock(lpEnvironment);
        CloseHandle(hToken);

        if (!procInfo)
        {
            Log->Error(L"Creating the new process is failed.", GetLastError()).Sub(L"Application", appStr).Sub("Paramaters", argsStr);
            return nullptr;
        }

        return procInfo;
    }

    template <StringLike T>
    std::shared_ptr<void> RunCommand(const T &command, HWND hWnd, const bool runAsAdmin, const ULONG mask, const int showFlags)
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
        shExInfo.fMask = mask;
        shExInfo.hwnd = hWnd;
        shExInfo.lpVerb = !runAsAdmin ? TEXT("open") : TEXT("runas");
        shExInfo.lpFile = TEXT("cmd.exe");
        shExInfo.lpParameters = lpParams;
        shExInfo.nShow = showFlags;

        if (!ShellExecuteEx(&shExInfo))
        {
            Log->Error("Running of command is failed.", GetLastError()).Sub("Command", lpParams);
            return nullptr;
        }
        return std::shared_ptr<void>(shExInfo.hProcess, [](auto hProcessRawPtr)
                                     { CloseHandle(hProcessRawPtr); });
    }
    template <StringLike T>
    std::shared_ptr<void> RunCommand(const T &command, const bool runAsAdmin)
    {
        return RunCommand(command, nullptr, runAsAdmin);
    }

    inline bool IsCurrentProcessElevated()
    {
        return Impl::GetCurrentProcessElevationInfo(0);
    }

    inline bool Notify(NOTIFYICONDATA &trayIconNID, LPCTSTR text, LPCTSTR title, const bool makeSound)
    {
        return Notify(trayIconNID, text, title, nullptr, makeSound);
    }

    inline std::vector<std::wstring> GetCommandLineArgvW()
    {
        return Impl::GetCommandLineArgv<WCHAR>();
    }

    inline std::vector<std::string> GetCommandLineArgvA()
    {
        return Impl::GetCommandLineArgv<CHAR>();
    }

    namespace Impl
    {
        template <Character T>
        auto GetProcessInfo()
        {
            return GetProcessInfo(std::basic_string_view<T>{});
        }
        template <Character T>
        auto GetProcessInfo(std::basic_string_view<T> desktop)
        {
            const auto pi = std::shared_ptr<PROCESS_INFORMATION>(new PROCESS_INFORMATION, [](auto p)
                                                                 { CloseHandle(p->hProcess); CloseHandle(p->hThread); delete p; });
            ZeroMemory(pi.get(), sizeof(PROCESS_INFORMATION));
            typename std::conditional_t<WideCharacter<T>, STARTUPINFOW, STARTUPINFOA> si;
            ZeroMemory(&si, sizeof(si));
            si.cb = sizeof(si);
            if (!desktop.empty())
            {
                si.lpDesktop = const_cast<decltype(si.lpDesktop)>(desktop.data());
            }
            return std::make_pair(pi, si);
        }

        void CastString(CharacterPointer auto &destStr, const CharacterStringView auto &sourceStrView)
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
        }

        template <Wcm::Character T>
        std::vector<std::basic_string<T>> GetCommandLineArgv()
        {
            int numArgs;
            auto cmdLines = CommandLineToArgvW(GetCommandLineW(), &numArgs);
            if (cmdLines == NULL)
            {
                Wcm::Log->Error("Failed to get array of command line arguments.", GetLastError());
                return {};
            }
            std::vector<std::basic_string<T>> args;
            for (int i = 0; i < numArgs; ++i)
            {
                if constexpr (Wcm::WideCharacter<T>)
                {
                    args.emplace_back(cmdLines[i]);
                }
                else
                {
                    args.emplace_back(Wcm::ToString(cmdLines[i]));
                }
            }
            if (LocalFree(cmdLines))
            {
                Wcm::Log->Error("String array buffer is could not be freed.", GetLastError());
            }
            return args;
        }
    }
}
