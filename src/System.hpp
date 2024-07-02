// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#pragma once
#include <concepts>
#include <userenv.h>
#include <wtsapi32.h>
#include "SystemCommon.hpp"
#include "Concepts.hpp"
#include "Log.hpp"

namespace Wcm
{
    bool EnablePrivilegeValue([[maybe_unused]] LPCWSTR lpszPrivilege, bool bEnablePrivilege);
    [[nodiscard]] HANDLE OpenProcessFromID(DWORD processID);
    std::shared_ptr<PROCESS_INFORMATION> Execute(const StringLike auto &app, const StringLike auto &args = {}, DWORD creationFlags = ProcessCreationFlags::CreateUnicodeEnvironment)
        requires((WideCharacter<CharacterOf<decltype(app)>> && WideCharacter<CharacterOf<decltype(args)>>) ||
                 (ByteCharacter<CharacterOf<decltype(app)>> && ByteCharacter<CharacterOf<decltype(args)>>));
    std::shared_ptr<PROCESS_INFORMATION> Execute(const StringLike auto &app, DWORD sessionId, const StringLike auto &args, DWORD creationFlags = Wcm::ProcessCreationFlags::CreateUnicodeEnvironment | Wcm::ProcessCreationFlags::NormalPriorityClass)
        requires((WideCharacter<CharacterOf<decltype(app)>> && WideCharacter<CharacterOf<decltype(args)>>) ||
                 (ByteCharacter<CharacterOf<decltype(app)>> && ByteCharacter<CharacterOf<decltype(args)>>));
    template <StringLike T>
    std::shared_ptr<void> RunCommand(const T &command, HWND hWnd, bool runAsAdmin = false);
    template <StringLike T>
    std::shared_ptr<void> RunCommand(const T &command, bool runAsAdmin = false);
    [[nodiscard]] HWND FindHWND(std::string_view windowTitle);
    [[nodiscard]] HWND FindHWND(std::wstring_view windowTitle);
    [[nodiscard]] DWORD GetCurrentSessionId();

    namespace Impl
    {
        template <Character T>
        [[nodiscard]] auto GetProcessInfo();

        std::shared_ptr<PROCESS_INFORMATION> CreateNewProcess(LPCWSTR app, LPWSTR args, DWORD creationFlags);
        std::shared_ptr<PROCESS_INFORMATION> CreateNewProcess(LPCSTR app, LPSTR args, DWORD creationFlags);

        std::shared_ptr<PROCESS_INFORMATION> CreateNewProcess(HANDLE hToken, LPCWSTR app, LPWSTR args, DWORD creationFlags);
        std::shared_ptr<PROCESS_INFORMATION> CreateNewProcess(HANDLE hToken, LPCSTR app, LPSTR args, DWORD creationFlags);
    }
}

#include "System.inl"
