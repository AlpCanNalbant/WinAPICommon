// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

#pragma once

#include <iostream>
#include <type_traits>
#include <memory>
#include <tuple>
#include <array>
#include <string>
#include <string_view>
#include <fstream>
#include <filesystem>
#include <source_location>
#include "Concepts.hpp"
#include "System.hpp"
#include "String.hpp"

namespace Wcm
{
    namespace Impl
    {
        template <typename T>
        concept LoggableMessage = StringViewConvertible<T> || Character<T>;

        class Log
        {
            enum class ErrorType;

        public:
            std::wstring OutputFile{};
            wchar_t InfoMark[4]{'(', '!', ')', '\0'}, ErrorMark[4]{'[', 'X', ']', '\0'};

            template <LoggableMessage... T>
            const Log &Write(const Character auto *mark, const T &...explanations) const;
            template <LoggableMessage... T>
            const Log &WriteLine(const Character auto *mark, const T &...explanations) const;
            template <LoggableMessage T>
            const Log &Info(const T &explanation) const;
            template <LoggableMessage T>
            const Log &Error(const T &reason, const std::source_location &location = std::source_location::current()) const;
            template <LoggableMessage T>
            const Log &Error(const T &reason, const HRESULT errorCode, const std::source_location &location = std::source_location::current()) const;
            template <LoggableMessage... T>
            const Log &Sub(const T &...titledSubMessage) const
                requires IsTypesEqualTo<2, T...>;
            [[nodiscard]] std::wstring GetLastErrorMessage() const;
            [[nodiscard]] std::wstring ToErrorMessage(const HRESULT errorCode) const;

        private:
            template <LoggableMessage T>
            void Error(const T &reason, const ErrorType type, const std::source_location &location) const;

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

#include "Log.inl"
