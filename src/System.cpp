// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#include <strsafe.h>
#include <uxtheme.h>
#include <dwmapi.h>
#include <wingdi.h>
#include "WinAPICommon.hpp"

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

    std::optional<std::pair<int, UINT>> FindMenuItem(HMENU hMenu, LPCTSTR text)
    {
        MENUITEMINFO item;
        const int itemCount = GetMenuItemCount(hMenu);
        if (itemCount == -1)
        {
            Log->Error("Menu item count not found.", GetLastError());
            return std::nullopt;
        }

        for (int i = 0; i < itemCount; ++i)
        {
            std::memset(&item, 0, sizeof(item));
            item.cbSize = sizeof(item);
            item.fMask = MIIM_STRING | MIIM_ID | MIIM_SUBMENU;
            if (!GetMenuItemInfo(hMenu, i, TRUE, &item))
            {
                Log->Error("Could not get menu item information.", GetLastError()).Sub("Position", std::to_string(i));
                continue;
            }

            if (item.cch > 0)
            {
                item.cch++;
                LPTSTR buf = static_cast<LPTSTR>(std::malloc(item.cch * sizeof(TCHAR)));
                if (buf != nullptr)
                {
                    if (!GetMenuString(hMenu, i, buf, item.cch, MF_BYPOSITION))
                    {
                        std::free(buf);
                        Log->Error("Could not get menu item string data.", GetLastError()).Sub("Position", std::to_string(i)).Sub("ID", std::to_string(item.wID));
                        continue;
                    }

                    if (lstrcmpi(text, buf) == 0)
                    {
                        std::free(buf);
                        return std::make_optional<std::pair<int, UINT>>(i, item.wID);
                    }

                    std::free(buf);
                }

                if (item.hSubMenu != nullptr)
                {
                    if (const auto res = FindMenuItem(item.hSubMenu, text))
                    {
                        return res;
                    }
                }
            }
        }

        return std::make_optional<std::pair<int, UINT>>(-1, 0);
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
        const int res = Impl::GetCurrentProcessElevationInfo(-1);
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

        static BOOL CALLBACK EnumWndsCallbackFindHWND(HWND, LPARAM);

        HWND FindHWNDT::operator()(std::wstring_view windowTitle) const noexcept
        {
            foundhWnd = nullptr;
            windowTitleToFind = windowTitle.data();
            EnumWindows(EnumWndsCallbackFindHWND, 0);
            return foundhWnd;
        }

        HWND FindHWNDT::operator()(std::string_view windowTitle) const
        {
            return this->operator()(ToWString(windowTitle));
        }

        static BOOL CALLBACK EnumWndsCallbackFindHWND(HWND hWnd, [[maybe_unused]] LPARAM lparam)
        {
            if (FindHWNDT::foundhWnd)
            {
                return FALSE;
            }

            const int length = GetWindowTextLengthW(hWnd);
            if (length > 0)
            {
                WCHAR buffer[length + 1];
                GetWindowTextW(hWnd, buffer, length + 1);
                std::wstring windowTitle{buffer};
                if (windowTitle.find(FindHWNDT::windowTitleToFind) != std::wstring::npos)
                {
                    FindHWNDT::foundhWnd = hWnd;
                    Wcm::Log->Info(L"Window handle of the process is found!").Sub("WindowTitle", windowTitle);
                    return FALSE;
                }
            }

            return TRUE;
        }

        static LRESULT CALLBACK CBTCallback(INT, WPARAM, LPARAM);
        static LRESULT CALLBACK MsgBoxWndProc(HWND, UINT, WPARAM, LPARAM);
        static BOOL CALLBACK EnumControlWndsCallback(HWND, LPARAM);
        static LRESULT CALLBACK ButtonProc(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);

        int MsgBoxT::operator()(LPCTSTR text, LPCTSTR title, DWORD styleFlags, HICON hWndIcon, HINSTANCE hIcoResModule, LPCTSTR titleIconRes, const DWORD langID) const
        {
            const auto insert = [title](const int seed) -> std::pair<decltype(MsgBoxT::instances)::iterator, bool>
            {
                // According to the docs, if this value is null, the default title 'Error' will be used.
                // HWND is cannot stored yet. Max displayable window title length is 256. String comparisons in the 'HCBT_CREATEWND' hook, will be cheap enough.
                for (auto it = MsgBoxT::instances.begin(); it != MsgBoxT::instances.cend(); ++it)
                {
                    if (lstrcmpi(std::get<0>(*it).get(), title) == 0)
                    {
                        return {MsgBoxT::instances.end(), false};
                    }
                }
                return {MsgBoxT::instances.emplace(
                            MsgBoxT::instances.cend(),
                            Wcm::StringAppend((title != nullptr) ? title : TEXT("Error"), ToString<TCHAR>(MsgBoxT::instances.size() + seed).get()), HWND{nullptr}, HHOOK{nullptr}, false, HICON{nullptr}, WNDPROC{nullptr}, std::vector<ButtonT>{}),
                        true};
            };
            constexpr auto rand = []
            {
                return std::rand() % (99999 - 10000 + 1) + 10000;
            };

            std::srand(std::time(nullptr));
            auto insertRes = insert(rand());
            while (!(insertRes.second))
            {
                insertRes = insert(rand());
            }
            auto it = insertRes.first;

            if (Wcm::HasFlag(styleFlags, MessageBoxStyle::DarkMode))
            {
                styleFlags &= ~(static_cast<DWORD>(MessageBoxStyle::DarkMode));
                std::get<3>(*it) = true;
            }

            MSGBOXPARAMS mbp{};
            mbp.cbSize = sizeof(mbp);
            mbp.hwndOwner = nullptr;
            mbp.lpszText = text;
            mbp.lpszCaption = std::get<0>(*it).get(); // title;
            mbp.dwStyle = MB_USERICON | MB_SETFOREGROUND | styleFlags;
            mbp.dwLanguageId = langID;

            if (titleIconRes)
            {
                if (!hIcoResModule)
                {
                    Wcm::Log->Error("Handle to the module that contains the title icon resource is null. Title icon will not be able to load.");
                }
                mbp.hInstance = hIcoResModule;
                mbp.lpszIcon = titleIconRes;
            }

            std::get<2>(*it) = SetWindowsHookEx(WH_CBT, &CBTCallback, 0, GetCurrentThreadId());
            std::get<4>(*it) = hWndIcon;
            if (std::get<2>(*it))
            {
                if (hWndIcon)
                {
                    return MessageBoxIndirect(&mbp);
                }
            }
            else
            {
                Wcm::Log->Error("Procedure could not added into a hook chain.", GetLastError()).Sub("ProcName", "CBTCallback");
            }

            return MessageBox(nullptr, text, title, styleFlags);
        }

        static LRESULT CALLBACK CBTCallback(INT nCode, WPARAM wParam, LPARAM lParam)
        {
            if (nCode < 0)
            {
                return CallNextHookEx(nullptr, nCode, wParam, lParam);
            }

            /* ~ The 'lparam' value for the 'HCBT_DESTROYWND' hook is undefined. */
            if (nCode == HCBT_CREATEWND)
            {
                LPCBT_CREATEWND lpCrateWnd = reinterpret_cast<LPCBT_CREATEWND>(lParam);
                HWND hWnd = reinterpret_cast<HWND>(wParam);

                for (auto it = MsgBoxT::instances.begin(); it != MsgBoxT::instances.cend(); ++it)
                {
                    if (lstrcmpi(lpCrateWnd->lpcs->lpszName, std::get<0>(*it).get()) == 0)
                    {
                        std::get<0>(*it).get()[lstrlen(lpCrateWnd->lpcs->lpszName) - 5 /*5 is length of the window's unique ID*/] = TEXT('\0');
                        std::get<0>(*it) = Wcm::ToBuffer<wchar_t>(std::get<0>(*it).get());

                        std::get<1>(*it) = hWnd;
                        if (std::get<4>(*it) != nullptr)
                        {
                            SendMessage(hWnd, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(std::get<4>(*it)));
                        }
                        break;
                    }
                }
            }
            else if (nCode == HCBT_ACTIVATE)
            {
                LPCBTACTIVATESTRUCT lpActivate = reinterpret_cast<LPCBTACTIVATESTRUCT>(lParam);
                for (auto it = MsgBoxT::instances.begin(); it != MsgBoxT::instances.cend(); ++it)
                {
                    if (lpActivate->hWndActive == std::get<1>(*it))
                    {
                        SendMessage(lpActivate->hWndActive, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(std::get<0>(*it).get()));
                        UnhookWindowsHookEx(std::get<2>(*it));

                        if (std::get<3>(*it) == true)
                        {
                            std::get<2>(*it) = nullptr; // Assing nullptr to unhooked proc.

                            auto style = GetWindowLongPtr(lpActivate->hWndActive, GWL_STYLE);
                            auto exStyle = GetWindowLongPtr(lpActivate->hWndActive, GWL_EXSTYLE);
                            style |= WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
                            exStyle |= WS_EX_COMPOSITED;
                            SetWindowLongPtr(lpActivate->hWndActive, GWL_STYLE, 2261255109);
                            SetWindowLongPtr(lpActivate->hWndActive, GWL_EXSTYLE, 33620225);

                            HRESULT res;
                            if ((res = SetWindowTheme(lpActivate->hWndActive, TEXT("DarkMode_Explorer::Button"), NULL)) != S_OK)
                            {
                                Wcm::Log->Error("Setting the window theme failed.", res);
                            }

                            BOOL bPolicyDisallow;
                            int nPolicyVal;
                            nPolicyVal = 2;
                            DwmSetWindowAttribute(lpActivate->hWndActive, DWMWA_NCRENDERING_POLICY, &nPolicyVal, sizeof(nPolicyVal));
                            DwmSetWindowAttribute(lpActivate->hWndActive, DWMWA_SYSTEMBACKDROP_TYPE, &nPolicyVal, sizeof(nPolicyVal));
                            nPolicyVal = 0;
                            DwmSetWindowAttribute(lpActivate->hWndActive, DWMWA_ALLOW_NCPAINT, &nPolicyVal, sizeof(nPolicyVal));
                            nPolicyVal = 0x2b2b2b;
                            DwmSetWindowAttribute(lpActivate->hWndActive, DWMWA_CAPTION_COLOR, &nPolicyVal, sizeof(nPolicyVal));
                            bPolicyDisallow = TRUE;
                            DwmSetWindowAttribute(lpActivate->hWndActive, DWMWA_HAS_ICONIC_BITMAP, &bPolicyDisallow, sizeof(bPolicyDisallow));
                            DwmSetWindowAttribute(lpActivate->hWndActive, DWMWA_USE_HOSTBACKDROPBRUSH, &bPolicyDisallow, sizeof(bPolicyDisallow));
                            DwmSetWindowAttribute(lpActivate->hWndActive, DWMWA_USE_IMMERSIVE_DARK_MODE, &bPolicyDisallow, sizeof(bPolicyDisallow));

                            EnumChildWindows(lpActivate->hWndActive, EnumControlWndsCallback, 0);

                            std::get<5>(*it) = reinterpret_cast<WNDPROC>(SetWindowLongPtr(lpActivate->hWndActive, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(MsgBoxWndProc)));
                        }
                        else
                        {
                            MsgBoxT::instances.erase(it);
                            MsgBoxT::instances.shrink_to_fit();
                        }

                        break;
                    }
                }
            }

            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        }

        static LRESULT CALLBACK MsgBoxWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
        {
            static std::unique_ptr<std::remove_pointer_t<HBRUSH>, WINBOOL (*)(HGDIOBJ)> hBrush1{CreateSolidBrush(0x202020), DeleteObject};
            static std::unique_ptr<std::remove_pointer_t<HBRUSH>, WINBOOL (*)(HGDIOBJ)> hBrush2{CreateSolidBrush(0x2b2b2b), DeleteObject};

            static constexpr COLORREF btnColor = RGB(51, 51, 51);
            static constexpr COLORREF btnHoveredColor = RGB(69, 69, 69);
            static constexpr COLORREF btnCheckedColor = RGB(102, 102, 102);

            static constexpr COLORREF btnBorderColor = RGB(255, 255, 255);
            static constexpr COLORREF btnBorderHoveredColor = RGB(155, 155, 155);
            static constexpr COLORREF btnBorderCheckedColor = RGB(155, 155, 155);
            static constexpr COLORREF btnBorderInactiveColor = RGB(155, 155, 155);

            static bool paintInit = false;

            switch (message)
            {
            case WM_ACTIVATE:
            {
                /* if (wParam == WA_ACTIVE || wParam == WA_CLICKACTIVE)
                {
                    // Got Focus
                }
                else if (wParam == WA_INACTIVE)
                {
                    // Lost Focus
                } */
                if (!paintInit)
                {
                    BufferedPaintInit();
                    paintInit = true;
                }
                break;
            }
            case WM_NCDESTROY:
            {
                BufferedPaintStopAllAnimations(hWnd);
                for (auto it = MsgBoxT::instances.begin(); it != MsgBoxT::instances.cend(); ++it)
                {
                    if (std::get<1>(*it) == hWnd)
                    {
                        MsgBoxT::instances.erase(it);
                        MsgBoxT::instances.shrink_to_fit();
                        break;
                    }
                }
                if (MsgBoxT::instances.empty() && paintInit)
                {
                    BufferedPaintUnInit();
                    paintInit = false;
                }
                break;
            }
            case WM_SIZE:
            {
                BufferedPaintStopAllAnimations(hWnd);
                break;
            }
            case WM_CTLCOLORSTATIC:
            {
                HDC hDC = reinterpret_cast<HDC>(wParam);

                SelectObject(hDC, hBrush2.get());
                SetBkMode(hDC, 0);
                SetTextColor(hDC, 0xFFFFFF);
                SetBkColor(hDC, 0x2b2b2b);

                RECT rcW{}, rcC{}, rcBtn{};
                GetWindowRect(hWnd, &rcW);
                GetClientRect(hWnd, &rcC);

                for (auto it = MsgBoxT::instances.begin(); it != MsgBoxT::instances.cend(); ++it)
                {
                    if (hWnd == std::get<1>(*it))
                    {
                        GetWindowRect(std::get<0>(std::get<6>(*it).back()), &rcBtn);
                    }
                }

                POINT pt{};
                pt.y = rcW.bottom - rcBtn.bottom;
                ScreenToClient(hWnd, &pt);

                HDC hDCwindow = GetWindowDC(hWnd);
                rcC.top = rcBtn.top + pt.y - 2;
                rcC.bottom *= 2;
                rcC.right *= 2;

                SetBkMode(hDCwindow, 0);
                FillRect(hDCwindow, &rcC, hBrush1.get());
                ReleaseDC(hWnd, hDCwindow);

                return reinterpret_cast<LRESULT>(hBrush2.get());
            }
            case WM_CTLCOLORDLG:
            case WM_CTLCOLOREDIT:
            {
                HDC hDC = reinterpret_cast<HDC>(wParam);

                SelectObject(hDC, hBrush2.get());
                SetBkMode(hDC, 0);
                SetTextColor(hDC, 0xFFFFFF);
                SetBkColor(hDC, 0x2b2b2b);

                return reinterpret_cast<LRESULT>(hBrush2.get());
            }
            case WM_DRAWITEM:
            {
                LPDRAWITEMSTRUCT lpDIS = (LPDRAWITEMSTRUCT)lParam;

                for (auto it = MsgBoxT::instances.begin(); it != MsgBoxT::instances.cend(); ++it)
                {
                    if (std::get<1>(*it) == hWnd)
                    {
                        for (auto itBtn = std::get<6>(*it).begin(); itBtn != std::get<6>(*it).cend(); ++itBtn)
                        {
                            if (lpDIS->hwndItem != std::get<0>(*itBtn))
                            {
                                continue;
                            }

                            HDC hDC = GetDC(std::get<0>(*itBtn));

                            if (!BufferedPaintRenderAnimation(std::get<0>(*itBtn), hDC))
                            {
                                static const auto drawButton = [](HWND hWnd, HDC hDC, RECT btnRect, COLORREF btnCol, COLORREF btnBorderCol)
                                {
                                    HBRUSH hBrush1 = CreateSolidBrush(btnCol);
                                    HBRUSH hBrush2 = CreateSolidBrush(btnBorderCol);

                                    // Draw border of the button.
                                    FillRect(hDC, &btnRect, hBrush2);

                                    // Draw button itself.
                                    btnRect.left += 1;
                                    btnRect.right -= 1;
                                    btnRect.top += 1;
                                    btnRect.bottom -= 1;
                                    FillRect(hDC, &btnRect, hBrush1);

                                    // Draw button text.
                                    const int len = GetWindowTextLength(hWnd);
                                    if (len > 0)
                                    {
                                        TCHAR lpBuf[len + 1];
                                        GetWindowText(hWnd, lpBuf, len + 1);

                                        // Get the current font data for its name.
                                        HFONT hCurFont = reinterpret_cast<HFONT>(SendMessage(hWnd, WM_GETFONT, 0, 0));
                                        LOGFONT lf;
                                        GetObject(hCurFont, sizeof(lf), &lf);
                                        // Create the font with the same name but with higher quality.
                                        constexpr auto fontSize = 18;
                                        HFONT hFont =
                                            CreateFont(fontSize,            // Height Of Font '-4', etc... /*-MulDiv(fontSize, GetDeviceCaps(hDC, LOGPIXELSY), 72)*/
                                                       0,                   // Width Of Font
                                                       0,                   // Angle Of Escapement
                                                       0,                   // Orientation Angle
                                                       FW_DONTCARE,         // Font Weight 'FW_BOLD', 'FW_DONTCARE', etc...
                                                       FALSE,               // Italic 'WINFONT_ITALIC'
                                                       FALSE,               // Underline 'WINFONT_UNDERLINE'
                                                       FALSE,               // Strikeout 'WINFONT_STRIKEOUT'
                                                       DEFAULT_CHARSET,     // Character Set Identifier
                                                       OUT_OUTLINE_PRECIS,  // Output Precision 'OUT_TT_PRECIS', etc...
                                                       CLIP_DEFAULT_PRECIS, // Clipping Precision
                                                       CLEARTYPE_QUALITY,   // Output Quality 'PROOF_QUALITY', 'ANTIALIASED_QUALITY', etc...
                                                       VARIABLE_PITCH,      // Family And Pitch 'FF_DONTCARE | DEFAULT_PITCH', etc..
                                                       lf.lfFaceName);      // Font Name 'Segoe UI Light' , etc..

                                        // Store prevs, for restoring their olds.
                                        HFONT hFontPrev = reinterpret_cast<HFONT>(SelectObject(hDC, hFont));
                                        UINT uAlignPrev = SetTextAlign(hDC, GetTextAlign(hDC) | TA_CENTER);

                                        SetBkMode(hDC, TRANSPARENT);
                                        SetTextColor(hDC, RGB(255, 255, 255));

                                        ExtTextOut(hDC, btnRect.right / 2, (btnRect.bottom / 2) - (fontSize / 2), ETO_CLIPPED, &btnRect, lpBuf, len, nullptr);

                                        SelectObject(hDC, hFontPrev);
                                        SetTextAlign(hDC, uAlignPrev);
                                        DeleteObject(hFont);
                                    }

                                    DeleteObject(hBrush1);
                                    DeleteObject(hBrush2);
                                };

                                BP_ANIMATIONPARAMS animParams{};
                                animParams.cbSize = sizeof(animParams);
                                animParams.style = BPAS_LINEAR;

                                if (std::get<1>(*itBtn)[0]) // Button is pressed.
                                {
                                    animParams.dwDuration = 150;

                                    std::get<2>(*itBtn)[0] = btnCheckedColor;       // Assign the pressed color to the target button color.
                                    std::get<2>(*itBtn)[2] = btnBorderCheckedColor; // Assign the pressed color to the target button border color.

                                    std::get<2>(*itBtn)[1] = btnHoveredColor;       // Assign the hovered color to the previous button border color.
                                    std::get<2>(*itBtn)[3] = btnBorderHoveredColor; // Assign the hovered color to the previous button border color.

                                    std::get<1>(*itBtn)[2] = true;
                                }
                                else if (std::get<1>(*itBtn)[1]) // Button is hovered.
                                {
                                    animParams.dwDuration = 250;

                                    std::get<2>(*itBtn)[0] = btnHoveredColor;
                                    std::get<2>(*itBtn)[2] = btnBorderHoveredColor;

                                    std::get<2>(*itBtn)[1] = btnColor;
                                    std::get<2>(*itBtn)[3] = (GetActiveWindow() == hWnd) ? btnBorderColor : btnBorderInactiveColor;

                                    std::get<1>(*itBtn)[2] = true;
                                }
                                else if (std::get<1>(*itBtn)[2])
                                {
                                    animParams.dwDuration = 250;

                                    // Swap the button colors.
                                    std::get<2>(*itBtn)[1] = std::get<2>(*itBtn)[0];
                                    std::get<2>(*itBtn)[3] = std::get<2>(*itBtn)[2];

                                    std::get<2>(*itBtn)[0] = btnColor;
                                    std::get<2>(*itBtn)[2] = (GetActiveWindow() == hWnd) ? btnBorderColor : btnBorderInactiveColor;

                                    std::get<1>(*itBtn)[2] = false;
                                }
                                else
                                {
                                    std::get<2>(*itBtn)[0] = btnColor;
                                    std::get<2>(*itBtn)[2] = (GetActiveWindow() == hWnd) ? btnBorderColor : btnBorderInactiveColor;
                                    animParams.dwDuration = 0;
                                }

                                RECT rcBtn;
                                GetClientRect(std::get<0>(*itBtn), &rcBtn);

                                HDC hDCFrom, hDCTo;
                                HANIMATIONBUFFER hBPAnimation =
                                    BeginBufferedAnimation(std::get<0>(*itBtn), hDC, &rcBtn,
                                                           BPBF_COMPATIBLEBITMAP, NULL, &animParams,
                                                           &hDCFrom, &hDCTo);
                                if (hBPAnimation)
                                {
                                    if (hDCFrom)
                                    {
                                        drawButton(std::get<0>(*itBtn), hDCFrom, rcBtn, std::get<2>(*itBtn)[1], std::get<2>(*itBtn)[3]);
                                    }
                                    if (hDCTo)
                                    {
                                        drawButton(std::get<0>(*itBtn), hDCTo, rcBtn, std::get<2>(*itBtn)[0], std::get<2>(*itBtn)[2]);
                                    }

                                    EndBufferedAnimation(hBPAnimation, TRUE);
                                }
                                else
                                {
                                    drawButton(std::get<0>(*itBtn), hDC, rcBtn, std::get<2>(*itBtn)[0], std::get<2>(*itBtn)[2]);
                                }
                            }

                            ReleaseDC(std::get<0>(*itBtn), hDC);
                        }

                        break;
                    }
                }

                return TRUE;
            }
            case WM_DESTROY:
            {
                PostQuitMessage(0);
                break;
            }
            default:
            {
                for (auto it = MsgBoxT::instances.begin(); it != MsgBoxT::instances.cend(); ++it)
                {
                    if (std::get<1>(*it) == hWnd)
                    {
                        return reinterpret_cast<LRESULT>(CallWindowProc(std::get<5>(*it), hWnd, message, wParam, lParam));
                    }
                }
                break;
            }
            }

            return 0;
        }

        static BOOL CALLBACK EnumControlWndsCallback(HWND hWnd, [[maybe_unused]] LPARAM lParam)
        {
            // The maximum length for class name is 256.
            TCHAR lpszClassName[256];
            if (!GetClassName(hWnd, lpszClassName, 256))
            {
                return TRUE;
            }

            HRESULT res;
            if ((res = SetWindowTheme(hWnd, TEXT("DarkMode_Explorer::Button"), NULL)) != S_OK)
            {
                Wcm::Log->Error("Setting the window theme failed.", res);
            }

            // Frist argument is, a pointer to the null-terminated string to search.
            // Second argument is, a pointer to the substring to search for.
            if (!StrStr(lpszClassName, TEXT("B")))
            {
                return TRUE;
            }

            // If this control is a button.
            if (HWND hWndParent = Wcm::GetParentOr(hWnd))
            {
                for (auto it = MsgBoxT::instances.begin(); it != MsgBoxT::instances.cend(); ++it)
                {
                    if (hWndParent == std::get<1>(*it))
                    {
                        SetWindowLongPtr(hWnd, GWL_STYLE, WS_CHILD | WS_VISIBLE | BS_OWNERDRAW);
                        SetWindowSubclass(hWnd, ButtonProc, 0, 0);
                        std::get<6>(*it).emplace_back(hWnd, std::array<bool, 3>{{false, false, false}}, std::array<COLORREF, 4>{{RGB(0, 0, 0), RGB(0, 0, 0), RGB(0, 0, 0), RGB(0, 0, 0)}});
                        break;
                    }
                }
            }

            return TRUE;
        }

        static LRESULT CALLBACK ButtonProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, [[maybe_unused]] UINT_PTR uIdSubclass, [[maybe_unused]] DWORD_PTR dwRefData)
        {
            switch (message)
            {
            case WM_MOUSEMOVE:
            {
                for (auto itWnd = MsgBoxT::instances.begin(); itWnd != MsgBoxT::instances.cend(); ++itWnd)
                {
                    for (auto itBtn = std::get<6>(*itWnd).begin(); itBtn != std::get<6>(*itWnd).cend(); ++itBtn)
                    {
                        if (std::get<0>(*itBtn) == hWnd)
                        {
                            if (GetCapture() != hWnd)
                            {
                                std::get<1>(*itBtn)[1] = true;
                                SetCapture(hWnd);
                                InvalidateRect(hWnd, NULL, TRUE);
                            }
                            else
                            {
                                RECT rect;
                                POINT pt = {LOWORD(lParam), HIWORD(lParam)};

                                GetWindowRect(hWnd, &rect);
                                ClientToScreen(hWnd, &pt);

                                if (!PtInRect(&rect, pt))
                                {
                                    std::get<1>(*itBtn) = {false, false};
                                    ReleaseCapture();
                                    InvalidateRect(hWnd, NULL, TRUE);
                                }
                            }

                            return DefSubclassProc(hWnd, message, wParam, lParam);
                        }
                    }
                }
            }
            break;
            case WM_LBUTTONDOWN:
            {
                for (auto itWnd = MsgBoxT::instances.begin(); itWnd != MsgBoxT::instances.cend(); ++itWnd)
                {
                    for (auto itBtn = std::get<6>(*itWnd).begin(); itBtn != std::get<6>(*itWnd).cend(); ++itBtn)
                    {
                        if (std::get<0>(*itBtn) == hWnd)
                        {
                            std::get<1>(*itBtn)[0] = true;

                            return DefSubclassProc(hWnd, message, wParam, lParam);
                        }
                    }
                }
            }
            break;
            }

            return DefSubclassProc(hWnd, message, wParam, lParam);
        }
    }
}
