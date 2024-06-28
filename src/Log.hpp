// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#pragma once
#include <iostream>
#include <memory>
#include <tuple>
#include <array>
#include <string_view>
#include <fstream>
#include <filesystem>
#include <source_location>
#include <thread>
#include <vector>
#include "SystemCommon.hpp"
#include "String.hpp"

namespace Wcm
{
    namespace Impl
    {
        template <typename T>
        concept LoggableMessage = std::constructible_from<std::filesystem::path, T> || Character<T>;

        class Log
        {
            enum class ErrorType;

        public:
            std::wstring OutputFile{};
            wchar_t InfoMark[4]{'(', '!', ')', '\0'}, ErrorMark[4]{'[', 'X', ']', '\0'};

            template <LoggableMessage... Msgs>
            Log &Write(const Character auto *mark, const Msgs &...explanations);
            template <LoggableMessage... Msgs>
            Log &WriteLine(const Character auto *mark, const Msgs &...explanations);
            template <LoggableMessage T>
            Log &Info(const T &explanation);
            template <LoggableMessage T>
            Log &Error(const T &reason, const std::source_location &location = std::source_location::current());
            template <LoggableMessage T>
            Log &Error(const T &reason, const HRESULT errorCode, const std::source_location &location = std::source_location::current());
            template <LoggableMessage... Msgs>
            Log &Sub(const Msgs &...titledSubMessage)
                requires IsEqual<2, Msgs...>;

            [[nodiscard]] std::basic_string<TCHAR> GetLastErrorMessage() const;
            [[nodiscard]] std::basic_string<TCHAR> ToErrorMessage(const HRESULT errorCode) const;

        private:
            template <LoggableMessage T>
            Log &Error(const T &reason, const ErrorType type, const std::source_location &location);
            template <LoggableMessage... Msgs>
            Log &WriteSubLine(const Character auto *mark, const Msgs &...titledSubMessage)
                requires IsEqual<2, Msgs...>;

            const char mySubMark_[8] = {"    |__"};
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

#include "Log.inl"
