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
    namespace Impl
    {
        using ButtonT = std::tuple<HWND, std::array<bool, 3>, std::array<COLORREF, 4>>;

        struct FindHWNDT final
        {
            inline static HWND foundhWnd{nullptr};
            inline static wchar_t const *windowTitleToFind{nullptr};

            [[nodiscard]] HWND operator()(std::wstring_view windowTitle) const noexcept;
            [[nodiscard]] HWND operator()(std::string_view windowTitle) const;
        };

        struct MsgBoxT final
        {
            inline static std::vector<std::tuple<std::shared_ptr<TCHAR>, HWND, HHOOK, bool, HICON, WNDPROC, std::vector<ButtonT>>> instances;

            int operator()(LPCTSTR text = nullptr, LPCTSTR title = nullptr, DWORD styleFlags = static_cast<DWORD>(MessageBoxStyle::Ok), HICON hWndIcon = nullptr, HINSTANCE hIcoResModule = nullptr, LPCTSTR titleIconRes = nullptr, const DWORD langID = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US)) const;
        };
    }

    inline constexpr int NotifyTitleMaxLength = 64;
    inline constexpr int NotifyTextMaxLength = 256;

    inline constexpr Impl::FindHWNDT FindHWND{};
    inline constexpr Impl::MsgBoxT MsgBox{};

    template <Character T>
    bool EnablePrivilegeValue(const T *const lpszPrivilege, bool bEnablePrivilege);
    [[nodiscard]] HANDLE OpenProcessFromID(DWORD processID);
    std::shared_ptr<PROCESS_INFORMATION> Execute(const StringLike auto &app, const StringLike auto &args = {}, DWORD creationFlags = ProcessCreationFlags::CreateUnicodeEnvironment)
        requires((WideCharacter<CharacterOf<decltype(app)>> && WideCharacter<CharacterOf<decltype(args)>>) ||
                 (ByteCharacter<CharacterOf<decltype(app)>> && ByteCharacter<CharacterOf<decltype(args)>>));
    std::shared_ptr<PROCESS_INFORMATION> Execute(const StringLike auto &app, DWORD sessionId, const StringLike auto &args, bool isInteractive = true, DWORD creationFlags = Wcm::ProcessCreationFlags::CreateUnicodeEnvironment | Wcm::ProcessCreationFlags::NormalPriorityClass)
        requires((WideCharacter<CharacterOf<decltype(app)>> && WideCharacter<CharacterOf<decltype(args)>>) ||
                 (ByteCharacter<CharacterOf<decltype(app)>> && ByteCharacter<CharacterOf<decltype(args)>>));
    template <StringLike T>
    std::shared_ptr<void> RunCommand(const T &command, HWND hWnd, bool runAsAdmin = false);
    template <StringLike T>
    std::shared_ptr<void> RunCommand(const T &command, bool runAsAdmin = false);
    [[nodiscard]] std::optional<std::pair<int, UINT>> FindMenuItem(HMENU hMenu, LPCTSTR text);
    [[nodiscard]] DWORD GetCurrentSessionId();
    [[nodiscard]] bool IsCurrentProcessElevated();
    [[nodiscard]] bool IsCurrentProcessElevated(bool &error);
    bool BringWindowToTop(HWND hWnd, bool keepTopmost = false);
    bool BringWindowToTop(HWND hWnd, DWORD dwProcessId, bool keepTopmost = false);
    bool TerminateProcessFromHwnd(HWND hWnd);
    std::shared_ptr<NOTIFYICONDATA> CreateTrayIcon(HWND hWnd = nullptr, const UINT iconID = 0, HICON hIcon = nullptr);
    bool CreateTrayIcon(NOTIFYICONDATA &outNID, HWND hWnd = nullptr, const UINT iconID = 0, HICON hIcon = nullptr);
    bool DeleteTrayIcon(NOTIFYICONDATA &trayIconNID);
    bool ShowTrayIcon(NOTIFYICONDATA &trayIconNID);
    bool HideTrayIcon(NOTIFYICONDATA &trayIconNID);
    bool Notify(NOTIFYICONDATA &trayIconNID, LPCTSTR text = nullptr, LPCTSTR title = nullptr, const bool makeSound = true);
    bool Notify(NOTIFYICONDATA &trayIconNID, LPCTSTR text = nullptr, LPCTSTR title = nullptr, HICON hBalloonIcon = nullptr, const bool makeSound = true);
    [[nodiscard]] std::vector<std::wstring> GetCommandLineArgvW();
    [[nodiscard]] std::vector<std::string> GetCommandLineArgvA();

    namespace Impl
    {
        template <Character T>
        [[nodiscard]] auto GetProcessInfo();
        template <Character T>
        [[nodiscard]] auto GetProcessInfo(std::basic_string_view<T> desktop);
        void CastString(CharacterPointer auto &destStr, const CharacterStringView auto &sourceStrView);

        std::shared_ptr<PROCESS_INFORMATION> CreateNewProcess(LPCWSTR app, LPWSTR args, DWORD creationFlags);
        std::shared_ptr<PROCESS_INFORMATION> CreateNewProcess(LPCSTR app, LPSTR args, DWORD creationFlags);

        std::shared_ptr<PROCESS_INFORMATION> CreateNewProcess(HANDLE hToken, LPCWSTR app, LPWSTR args, std::wstring_view desktop, DWORD creationFlags);
        std::shared_ptr<PROCESS_INFORMATION> CreateNewProcess(HANDLE hToken, LPCSTR app, LPSTR args, std::string_view desktop, DWORD creationFlags);

        [[nodiscard]] int GetCurrentProcessElevationInfo(const int errCode);

        template <Wcm::Character T>
        [[nodiscard]] std::vector<std::basic_string<T>> GetCommandLineArgv();
    }
}

#include "System.inl"
