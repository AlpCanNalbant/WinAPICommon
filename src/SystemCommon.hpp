// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#pragma once
#include <string>

namespace Wcm
{
    [[nodiscard]] std::wstring GetDate();
    [[nodiscard]] std::wstring GetDesktopName();
    [[nodiscard]] std::wstring GetPCName();
    [[nodiscard]] HWND GetParentOr(HWND hWndOwner, HWND hWndDefault = nullptr);
    void CloseWindow(HWND hWnd);

    namespace ProcessCreationFlags
    {
        enum : DWORD
        {
            None,
            DebugProcess = 0x00000001,
            DebugOnlyThisProcess = 0x00000002,
            CreateSuspended = 0x00000004,
            DetachedProcess = 0x00000008,
            CreateNewConsole = 0x00000010,
            NormalPriorityClass = 0x00000020,
            IdlePriorityClass = 0x00000040,
            HighPriorityClass = 0x00000080,
            RealtimePriorityClass = 0x00000100,
            CreateNewProcessGroup = 0x00000200,
            CreateUnicodeEnvironment = 0x00000400,
            CreateSeparateWowVdm = 0x00000800,
            CreateSharedWowVdm = 0x00001000,
            CreateForceDos = 0x00002000,
            BelowNormalPriorityClass = 0x00004000,
            AboveNormalPriorityClass = 0x00008000,
            InheritParentAffinity = 0x00010000,
            InheritCallerPriority = 0x00020000,
            CreateProtectedProcess = 0x00040000,
            ExtendedStartupInfoPresent = 0x00080000,
            ProcessModeBackgroundBegin = 0x00100000,
            ProcessModeBackgroundEnd = 0x00200000,
            CreateBreakAwayFromJob = 0x01000000,
            CreatePreserveCodeAuthzLevel = 0x02000000,
            CreateDefaultErrorMode = 0x04000000,
            CreateNoWindow = 0x08000000,
            ProfileUser = 0x10000000,
            ProfileKernel = 0x20000000,
            ProfileServer = 0x40000000,
            CreateIgnoreSystemDefault = 0x80000000
        };
    }

    namespace MessageBoxStyle
    {
        enum : DWORD
        {
            Ok = 0x00000000,
            OkCancel = 0x00000001,
            AbortRetryIgnore = 0x00000002,
            YesNoCancel = 0x00000003,
            YesNo = 0x00000004,
            RetryCancel = 0x00000005,
            CancelTryContinue = 0x00000006,
            Help = 0x00004000,
            IconStop = 0x00000010,
            IconError = 0x00000010,
            IconHand = 0x00000010,
            IconQuestion = 0x00000020,
            IconWarning = 0x00000030,
            IconExclamation = 0x00000030,
            IconInformation = 0x00000040,
            IconAsterisk = 0x00000040,
            DarkMode = 0x00400000,
            DefaultButton = 0x00000000,
            DefaultButton2 = 0x00000100,
            DefaultButton3 = 0x00000200,
            DefaultButton4 = 0x00000300,
            ApplicationModal = 0x00000000,
            SystemModal = 0x00001000,
            TaskModal = 0x00002000,
            DefaultDesktopOnly = 0x00020000,
            Right = 0x00080000,
            RightToLeftReading = 0x00100000,
            SetForeground = 0x00010000,
            TopMost = 0x00040000,
            ServiceNotification = 0x00200000
        };
    }
}
