// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#pragma once

#include <string>
#include <string_view>
#include <fstream>
#include <memory>
#include <source_location>

namespace Wcm
{
    namespace Impl
    {
        class Log
        {
            enum class ErrorType;

        public:
            std::wstring OutputFile{};
            wchar_t InfoMark[4]{'(', '!', ')', '\0'}, ErrorMark[4]{'[', 'X', ']', '\0'};

            const Log &WriteLine(const wchar_t *mark, std::wstring_view explanation) const;
            const Log &Info(std::wstring_view explanation) const;
            const Log &Error(std::wstring_view reason, const std::source_location &location = std::source_location::current()) const;
            const Log &Error(std::wstring_view reason, const HRESULT errorCode, const std::source_location &location = std::source_location::current()) const;
            void Sub(std::initializer_list<std::wstring_view[2]> messages) const;
            [[nodiscard]] std::wstring GetLastErrorMessage() const;
            [[nodiscard]] std::wstring ToErrorMessage(const HRESULT errorCode) const;

        private:
            void Error(std::wstring_view reason, const ErrorType type, const std::source_location &location) const;

            mutable std::wofstream fileStream_;
            mutable HRESULT errorCode_;

            enum class ErrorType
            {
                Normal,
                WinAPI
            };
        };
    }

    extern const std::unique_ptr<Impl::Log> Log;
}
