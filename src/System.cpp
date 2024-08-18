// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#include <strsafe.h>
#include "WinAPICommon.hpp"

BOOL CALLBACK EnumWindowCallback(HWND, LPARAM);
LRESULT CALLBACK CBTCallback(UINT, WPARAM, LPARAM);
HHOOK hCBTHook = nullptr;
HWND foundhWnd = nullptr;
HICON hMsgBoxWndIco = nullptr;
wchar_t const *windowTitleToFind = nullptr;

namespace Wcm
{
    HANDLE OpenProcessFromID(DWORD processID)
    {
        if (!EnablePrivilegeValue(SE_DEBUG_NAME, true))
        {
            Log->Error("Failed to obtain required privileges for openning the process.");
        }

        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, processID);
        if (hProcess == NULL)
        {
            Log->Error("Failed to open process.", GetLastError());
            return NULL;
        }

        return hProcess;
    }

    HWND FindHWND(std::string_view windowTitle)
    {
        return FindHWND(Wcm::ToWStringIf(windowTitle));
    }
    HWND FindHWND(std::wstring_view windowTitle)
    {
        foundhWnd = nullptr;
        windowTitleToFind = windowTitle.data();
        EnumWindows(EnumWindowCallback, 0);
        return foundhWnd;
    }

    DWORD GetCurrentSessionId()
    {
        WTS_SESSION_INFO *pSessionInfo;
        DWORD dwSessions = 0;
        if (!WTSEnumerateSessions(WTS_CURRENT_SERVER, 0, 1, &pSessionInfo, &dwSessions))
        {
            Log->Error("Enumerating sessions failed.", GetLastError());
            return 0;
        }
        DWORD sessionId = 0;
        for (DWORD i = 0; i < dwSessions; ++i)
        {
            if (pSessionInfo[i].State == WTSActive)
            {
                sessionId = pSessionInfo[i].SessionId;
                break;
            }
        }
        WTSFreeMemory(pSessionInfo);
        return sessionId;
    }

    bool IsCurrentProcessElevated(bool &error)
    {
        const int res = GetCurrentProcessElevationInfo(-1);
        if (res == -1)
        {
            error = true;
            return false;
        }
        error = false;
        return res;
    }

    bool BringWindowToTop(HWND hWnd, bool keepTopmost)
    {
        return BringWindowToTop(hWnd, GetCurrentProcessId(), keepTopmost);
    }

    bool BringWindowToTop(HWND hWnd, DWORD dwProcessId, bool keepTopmost)
    {
        ShowWindow(hWnd, SW_SHOW);
        if (!SetFocus(hWnd))
        {
            Log->Error(L"Setting keyboard focus to the specified window is failed.", GetLastError());
        }
        {
            DWORD dwFailCount = 2;
            if (!SetWindowPos(hWnd, (!keepTopmost) ? HWND_TOP : HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSENDCHANGING | SWP_NOSIZE | SWP_SHOWWINDOW))
            {
                Log->Error(L"Setting window position is failed.", GetLastError()).Sub(L"IsKeepTopmostEnabled", keepTopmost ? L"True" : L"False");
                --dwFailCount;
                if (keepTopmost)
                {
                    return false;
                }
            }
            if (!::BringWindowToTop(hWnd))
            {
                Log->Error(L"Bringing the window to the top of the Z order is failed.", GetLastError());
                --dwFailCount;
                if (dwFailCount < 1)
                {
                    return false;
                }
            }
        }
        if (!AllowSetForegroundWindow(dwProcessId))
        {
            Log->Error(L"Enabling the specified process to set the foreground window is failed. This error can be ignored, the process may already be able to set the foreground window.", GetLastError()).Sub("ProcessID", std::to_string(dwProcessId));
        }
        if (!SetForegroundWindow(hWnd))
        {
            Log->Error(L"Window could not be brought to the foreground.", GetLastError());
        }
        return true;
    }

    bool TerminateProcessFromHwnd(HWND hWnd)
    {
        DWORD procId;
        if (!GetWindowThreadProcessId(hWnd, &procId))
        {
            Log->Error(L"Process id cannot obtained.", GetLastError());
            return false;
        }
        HANDLE hProc = NULL;
        if ((hProc = OpenProcess(PROCESS_TERMINATE, TRUE, procId)) == NULL)
        {
            Log->Error(L"Process handle cannot obtained.", GetLastError());
            return false;
        }
        DWORD exitCode;
        if (!GetExitCodeProcess(hProc, &exitCode))
        {
            Log->Error(L"Process exit code cannot obtained.", GetLastError());
            exitCode = 123;
        }
        if (!TerminateProcess(hProc, exitCode))
        {
            Log->Error(L"Process cannot terminated.", GetLastError());
            return false;
        }
        return true;
    }

    int MsgBox(LPCTSTR text, LPCTSTR title, const DWORD styleFlags, HICON hWndIcon, HINSTANCE hIcoResModule, LPCTSTR titleIconRes, const DWORD langID)
    {
        MSGBOXPARAMS mbp = {0};
        mbp.cbSize = sizeof(mbp);
        mbp.hwndOwner = nullptr;
        mbp.lpszText = text;
        mbp.lpszCaption = title;
        mbp.dwStyle = MB_USERICON | MB_SETFOREGROUND | styleFlags;
        mbp.dwLanguageId = langID;

        if (titleIconRes)
        {
            if (!hIcoResModule)
            {
                Log->Error("Handle to the module that contains the title icon resource is null. Title icon will not be able to load.");
            }
            mbp.hInstance = hIcoResModule;
            mbp.lpszIcon = titleIconRes;
        }

        if (hWndIcon)
        {
            hMsgBoxWndIco = hWndIcon;
            hCBTHook = SetWindowsHookEx(WH_CBT, (HOOKPROC)CBTCallback, 0, GetCurrentThreadId());
            if (hCBTHook)
            {
                return MessageBoxIndirect(&mbp);
            }

            Log->Error("Procedure could not added into a hook chain.");
        }

        return MessageBox(nullptr, text, title, styleFlags);
    }

    std::shared_ptr<NOTIFYICONDATA> CreateTrayIcon(HWND hWnd, const UINT iconID, HICON hIcon)
    {
        const auto nid = std::make_shared<NOTIFYICONDATA>();
        if (!CreateTrayIcon(*nid, hWnd, iconID, hIcon))
        {
            return nullptr;
        }
        return nid;
    }

    bool CreateTrayIcon(NOTIFYICONDATA &outNID, HWND hWnd, const UINT iconID, HICON hIcon)
    {
        outNID.cbSize = sizeof(outNID);
        outNID.uID = iconID;
        outNID.hWnd = hWnd;
        outNID.uFlags = NIF_ICON | NIF_MESSAGE | NIF_STATE;
        outNID.dwStateMask = NIS_HIDDEN;
        outNID.hIcon = hIcon;
        outNID.uVersion = NOTIFYICON_VERSION_4;

        if (outNID.hIcon == nullptr)
        {
            Log->Error("The tray icon could not be created due to address of the icon handle is null.");
            return false;
        }
        if (!Shell_NotifyIcon(NIM_ADD, &outNID))
        {
            Log->Error("'Shell_NotifyIcon()' function is failed while creating tray icon.", GetLastError()).Sub("NotifyAction", "Add");
            return false;
        }
        if (!Shell_NotifyIcon(NIM_SETVERSION, &outNID))
        {
            Log->Error("'Shell_NotifyIcon()' function is failed while creating tray icon.", GetLastError()).Sub("NotifyAction", "SetVersion");
            return false;
        }
        return true;
    }

    bool DeleteTrayIcon(NOTIFYICONDATA &trayIconNID)
    {
        if (!Shell_NotifyIcon(NIM_DELETE, &trayIconNID))
        {
            Log->Error("'Shell_NotifyIcon()' function is failed while deleting the tray icon from the status area.", GetLastError()).Sub("NotifyAction", "Delete");
            return false;
        }
        return true;
    }

    bool ShowTrayIcon(NOTIFYICONDATA &trayIconNID)
    {
        trayIconNID.dwState = 0;
        if (!Shell_NotifyIcon(NIM_MODIFY, &trayIconNID))
        {
            Log->Error("'Shell_NotifyIcon()' function is failed while hiding tray icon.", GetLastError()).Sub("NotifyAction", "Modify");
            return false;
        }
        return true;
    }

    bool HideTrayIcon(NOTIFYICONDATA &trayIconNID)
    {
        trayIconNID.dwState = NIS_HIDDEN;
        if (!Shell_NotifyIcon(NIM_MODIFY, &trayIconNID))
        {
            Log->Error("'Shell_NotifyIcon()' function is failed while hiding tray icon.", GetLastError()).Sub("NotifyAction", "Modify");
            return false;
        }
        return true;
    }

    bool Notify(NOTIFYICONDATA &trayIconNID, LPCTSTR text, LPCTSTR title, HICON hBalloonIcon, const bool makeSound)
    {
        {
            int len;
            if (((len = GetStringLength(text)) > NotifyTextMaxLength))
            {
                Log->Error("Lenght of notification text must be less than or equal to " + std::to_string(NotifyTextMaxLength) + '.', GetLastError()).Sub("Length", std::to_string(len));
                return false;
            }
            if (((len = GetStringLength(title)) > NotifyTitleMaxLength))
            {
                Log->Error("Lenght of notification title must be less than or equal to " + std::to_string(NotifyTitleMaxLength) + '.', GetLastError()).Sub("Length", std::to_string(len));
                return false;
            }
        }

        if (StringCopy(trayIconNID.szInfoTitle, title) == StringCopyResult::InvalidParameter)
        {
            return false;
        }
        if (StringCopy(trayIconNID.szInfo, text) == StringCopyResult::InvalidParameter)
        {
            return false;
        }

        trayIconNID.uFlags |= NIF_INFO;
        trayIconNID.dwInfoFlags = NIIF_USER | NIIF_LARGE_ICON;
        trayIconNID.hBalloonIcon = hBalloonIcon;
        if (makeSound)
        {
            trayIconNID.dwInfoFlags |= NIIF_NOSOUND;
        }
        if (!Shell_NotifyIcon(NIM_MODIFY, &trayIconNID))
        {
            Log->Error("'Shell_NotifyIcon()' function is failed while displaying a balloon notification.", GetLastError()).Sub("NotifyAction", "Modify");
            return false;
        }
        return true;
    }

    namespace Impl
    {
        std::shared_ptr<PROCESS_INFORMATION> CreateNewProcess(LPCWSTR app, LPWSTR args, DWORD creationFlags)
        {
            if (const auto procInfo = GetProcessInfo<WCHAR>(); CreateProcessW(app, args, nullptr, nullptr, FALSE, creationFlags, nullptr, nullptr, const_cast<LPSTARTUPINFOW>(&procInfo.second), procInfo.first.get()))
            {
                return procInfo.first;
            }
            Wcm::Log->Error("Creation of process is failed.", GetLastError()).Sub("Process", app);
            return nullptr;
        }

        std::shared_ptr<PROCESS_INFORMATION> CreateNewProcess(LPCSTR app, LPSTR args, DWORD creationFlags)
        {
            if (const auto procInfo = GetProcessInfo<CHAR>(); CreateProcessA(app, args, nullptr, nullptr, FALSE, creationFlags, nullptr, nullptr, const_cast<LPSTARTUPINFOA>(&procInfo.second), procInfo.first.get()))
            {
                return procInfo.first;
            }
            Wcm::Log->Error("Creation of process is failed.", GetLastError()).Sub("Process", app);
            return nullptr;
        }

        std::shared_ptr<PROCESS_INFORMATION> CreateNewProcess(HANDLE hToken, LPCWSTR app, LPWSTR args, std::wstring_view desktop, DWORD creationFlags)
        {
            if (const auto procInfo = GetProcessInfo<WCHAR>(desktop); CreateProcessAsUserW(hToken, app, args, nullptr, nullptr, FALSE, creationFlags, nullptr, nullptr, const_cast<LPSTARTUPINFOW>(&procInfo.second), procInfo.first.get()))
            {
                return procInfo.first;
            }
            Wcm::Log->Error("Creation of process is failed.", GetLastError()).Sub("Process", app);
            return nullptr;
        }

        std::shared_ptr<PROCESS_INFORMATION> CreateNewProcess(HANDLE hToken, LPCSTR app, LPSTR args, std::string_view desktop, DWORD creationFlags)
        {
            if (const auto procInfo = GetProcessInfo<CHAR>(desktop); CreateProcessAsUserA(hToken, app, args, nullptr, nullptr, FALSE, creationFlags, nullptr, nullptr, const_cast<LPSTARTUPINFOA>(&procInfo.second), procInfo.first.get()))
            {
                return procInfo.first;
            }
            Wcm::Log->Error("Creation of process is failed.", GetLastError()).Sub("Process", app);
            return nullptr;
        }

        int GetCurrentProcessElevationInfo(const int errCode)
        {
            int res;
            HANDLE hToken = nullptr;
            if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
            {
                TOKEN_ELEVATION elevation;
                DWORD cbSize = sizeof(TOKEN_ELEVATION);
                if (GetTokenInformation(hToken, TokenElevation, &elevation, sizeof(elevation), &cbSize))
                {
                    res = elevation.TokenIsElevated;
                }
                else
                {
                    Wcm::Log->Error("Could not be retrieved whether the token is elevated.", GetLastError());
                    res = errCode;
                }
            }
            else
            {
                Wcm::Log->Error("Unable to open the access token associated with the current process.", GetLastError());
                res = errCode;
            }
            if (hToken)
            {
                CloseHandle(hToken);
            }
            return res;
        }
    }
}

BOOL CALLBACK EnumWindowCallback(HWND hWnd, [[maybe_unused]] LPARAM lparam)
{
    if (foundhWnd)
    {
        return FALSE;
    }

    int length = GetWindowTextLengthW(hWnd);
    if (length > 0)
    {
        WCHAR buffer[length + 1] = {'\0'};
        GetWindowTextW(hWnd, buffer, length + 1);
        std::wstring windowTitle{buffer};
        // Do not bloat the log messages with this info.
        // Log->Info(std::wstring{L"Name of the current window handle is "} + buffer + L'.');
        if (windowTitle.find(windowTitleToFind) != std::wstring::npos)
        {
            foundhWnd = hWnd;
            Wcm::Log->Info(L"Window handle of the process is found!").Sub("WindowTitle", windowTitle);
            return FALSE;
        }
    }

    return TRUE;
}

LRESULT CALLBACK CBTCallback(UINT nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HCBT_ACTIVATE)
    {
        SendMessage((HWND)wParam, WM_SETICON, ICON_SMALL, (LPARAM)hMsgBoxWndIco);
        UnhookWindowsHookEx(hCBTHook);
    }
    else
    {
        CallNextHookEx(hCBTHook, nCode, wParam, lParam);
    }
    return 0;
}
