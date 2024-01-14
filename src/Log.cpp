// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#include <iostream>
#include <filesystem>
#include <comdef.h>

#include "Log.hpp"
#include "System.hpp"
#include "String.hpp"

namespace WinCmn
{
    extern const std::unique_ptr<Impl::Log> Log = std::make_unique<Impl::Log>();

    namespace Impl
    {
        const Log &Log::WriteLine(const wchar_t *mark, const std::wstring &explanation) const
        {
#define WINCMN_LOG_WRITELINE_STREAM_STRING_INSERTERS mark << L' ' << explanation << L'\n';

            if (!OutputFile.empty())
            {
                // Append instead of overwrite.
                fileStream_.open(std::filesystem::path(OutputFile), std::ios_base::app);
#ifndef NDEBUG
                if (!fileStream_.is_open())
                {
                    std::wcout << "File stream cannot be opened.\n";
                    return *this;
                }
#endif
                fileStream_ << WINCMN_LOG_WRITELINE_STREAM_STRING_INSERTERS;
                fileStream_.close();
#ifndef NDEBUG
                if (fileStream_.fail())
                {
                    std::wcout << "An error occurred while closing the file stream.\n";
                    return *this;
                }
#endif
            }

#ifndef NDEBUG
            std::wcout << WINCMN_LOG_WRITELINE_STREAM_STRING_INSERTERS;
#endif
            return *this;
        }

        const Log &Log::Info(const std::wstring &explanation) const
        {
            WriteLine(InfoMark, explanation);
            Sub({{L"At", GetDate()}});
            return *this;
        }

        const Log &Log::Error(const std::wstring &reason, const std::source_location &location) const
        {
            Error(reason, ErrorType::Normal, location);
            return *this;
        }

        const Log &Log::Error(const std::wstring &reason, const HRESULT errorCode, const std::source_location &location) const
        {
            errorCode_ = errorCode;
            Error(reason, ErrorType::WinAPI, location);
            return *this;
        }

        void Log::Sub(std::initializer_list<std::wstring[2]> messages) const
        {
            for (const auto &msg : messages)
            {
                WriteLine(L"    |___", msg[0] + L": " + msg[1]);
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

        void Log::Error(const std::wstring &reason, const ErrorType type, const std::source_location &location) const
        {
            switch (type)
            {
            case ErrorType::Normal:
                WriteLine(ErrorMark, reason);
                break;
            case ErrorType::WinAPI:
                WriteLine(ErrorMark, reason);
                Sub({{L"Error", ToErrorMessage(errorCode_)}});
                break;
            default:
                WriteLine(ErrorMark, L"Invalid error type.");
                return;
            }
            Sub({{L"Line", std::to_wstring(location.line())}, {L"File", ToWString(location.file_name())}, {L"At", GetDate()}});
        }
    }
}
