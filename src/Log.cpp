// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#include <iostream>
#include <filesystem>
#include <comdef.h>

#include "WinAPICommon.hpp"

namespace WinCmn
{
    extern const std::unique_ptr<Impl::Log> Log = std::make_unique<Impl::Log>();

    namespace Impl
    {
        Log::Log()
            : OutputFile{WinCmn::GetBaseDirectory() + L"\\Log.txt"} {}

        void Log::WriteLine(const wchar_t *mark, const std::wstring &explanation)
        {
            fileStream_.open(std::filesystem::path(OutputFile), std::ios_base::app); // Append instead of overwrite
#ifndef NDEBUG
            if (!fileStream_.is_open())
            {
                std::wcout << "File stream cannot be opened.\n";
                return;
            }
#endif
#define __WINCMN_LOG_WRITELINE_STREAM_STRING_INSERTERS mark << L' ' << explanation << L" ~ Happened at: " << WinCmn::GetDate() << L".\n";
            fileStream_ << __WINCMN_LOG_WRITELINE_STREAM_STRING_INSERTERS;
            fileStream_.close();
#ifndef NDEBUG
            if (fileStream_.fail())
            {
                std::wcout << "An error occurred while closing the file stream.\n";
                return;
            }
            std::wcout << __WINCMN_LOG_WRITELINE_STREAM_STRING_INSERTERS;
#endif
        }

        void Log::Info(const std::wstring &explanation)
        {
            WriteLine(InfoMark, explanation);
        }

        void Log::Error(const std::wstring &reason)
        {
            Error(reason, ErrorType::Normal);
        }

        void Log::Error(const std::wstring &reason, const HRESULT errorCode)
        {
            errorCode_ = errorCode;
            Error(reason, ErrorType::WinAPI);
        }

        void Log::Error(const std::wstring &reason, const ErrorType type)
        {
            switch (type)
            {
            case ErrorType::Normal:
                WriteLine(ErrorMark, reason);
                break;
            case ErrorType::WinAPI:
                WriteLine(ErrorMark, reason + L" ~ Error: " + ToErrorMessage(errorCode_));
                break;
            default:
                break;
            }
        }

        std::wstring Log::GetLastErrorMessage() const
        {
            return ToErrorMessage(GetLastError());
        }

        std::wstring Log::ToErrorMessage(const HRESULT errorCode) const
        {
            _com_error errorHandler{errorCode};
            LPCTSTR errorText = errorHandler.ErrorMessage();
            return {errorText};
        }
    }
}
