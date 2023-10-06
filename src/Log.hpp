// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#pragma once

#include <string>
#include <fstream>
#include <memory>

namespace WinCmn
{
    namespace Impl
    {
        class Log
        {
            enum class ErrorType;

        public:
            std::wstring OutputFile;
            wchar_t InfoMark[4]{'(', '!', ')', '\0'}, ErrorMark[4]{'[', 'X', ']', '\0'};

            Log();
            void WriteLine(const wchar_t *mark, const std::wstring &explanation);
            void Info(const std::wstring &explanation);
            void Error(const std::wstring &reason);
            void Error(const std::wstring &reason, const HRESULT errorCode);
            [[nodiscard]] std::wstring GetLastErrorMessage() const;
            [[nodiscard]] std::wstring ToErrorMessage(const HRESULT errorCode) const;

        private:
            void Error(const std::wstring &reason, const ErrorType type);

            std::wofstream fileStream_;
            HRESULT errorCode_;

            enum class ErrorType
            {
                Normal,
                WinAPI
            };
        };
    }

    extern const std::unique_ptr<Impl::Log> Log;
}
