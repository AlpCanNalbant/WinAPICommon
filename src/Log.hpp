/*
// MIT License - Copyright (c) 2023 Alp Can Nalbant
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.
*/

#pragma once

#include <string>
#include <memory>
#include <fstream>
#include <windows.h>

namespace WCmn
{
    namespace Impl
    {
        class Log
        {
            enum class ErrorType;

        public:
            std::wstring OutputFile;
            std::string InfoMark{"(!)"}, ErrorMark{"[X]"};

            Log();
            void WriteLine(const std::string &mark, const std::wstring &explanation);
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
