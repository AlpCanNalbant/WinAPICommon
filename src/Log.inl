// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

namespace Wcm::Impl
{
    template <LoggableMessage... Msgs>
    Log &Log::Write(const Character auto *mark, const Msgs &...explanations)
    {
        if (!OutputFile.empty())
        {
            fileStream_.open(OutputFile.c_str(), std::ios_base::app); // Append instead of overwrite.
#ifndef NDEBUG
            if (!fileStream_.is_open())
            {
                std::wcerr << ErrorMark << ' ' << "File stream cannot be opened.\n";
                return *this;
            }
#endif
            fileStream_ << mark << ' ';
            auto streamExpl = [this] (const auto &expl)
            {
                fileStream_ << ToWStringIf(expl);
            };
            (streamExpl(explanations), ...);
            fileStream_.close();
#ifndef NDEBUG
            if (fileStream_.fail())
            {
                std::wcerr << ErrorMark << ' ' << "An error occurred while closing the file stream.\n";
                return *this;
            }
#endif
        }
#ifndef NDEBUG
        const auto isError = IsSameString(ToStringView(ToWStringIf(ErrorMark)), ToStringView(ToWStringIf(mark)));
        if (isError)
        {
            std::wcerr << mark << ' ';
        }
        else
        {
            std::wcout << mark << ' ';
        }
        const auto outputExpl = [isError] (const auto &expl)
        {
            if (isError)
            {
                std::wcerr << ToWStringIf(expl);
            }
            else
            {
                std::wcout << ToWStringIf(expl);
            }
        };
        (outputExpl(explanations), ...);
#endif
        return *this;
    }

    template <LoggableMessage... Msgs>
    Log &Log::WriteLine(const Character auto *mark, const Msgs &...explanations)
    {
        Write(mark, explanations..., '\n');
        return *this;
    }

    template <LoggableMessage T>
    Log &Log::Info(const T &explanation)
    {
        WriteLine(InfoMark, explanation);
        Sub("At", GetDate());
        return *this;
    }

    template <LoggableMessage T>
    Log &Log::Error(const T &reason, const std::source_location &location)
    {
        Error(reason, ErrorType::Normal, location);
        return *this;
    }

    template <LoggableMessage T>
    Log &Log::Error(const T &reason, const HRESULT errorCode, const std::source_location &location)
    {
        errorCode_ = errorCode;
        Error(reason, ErrorType::WinAPI, location);
        return *this;
    }

    template <LoggableMessage T>
    void Log::Error(const T &reason, const ErrorType type, const std::source_location &location)
    {
        switch (type)
        {
        case ErrorType::Normal:
            WriteLine(ErrorMark, reason);
            break;
        case ErrorType::WinAPI:
            WriteLine(ErrorMark, reason);
            Sub("Error", ToErrorMessage(errorCode_));
            break;
        default:
            WriteLine(ErrorMark, "Invalid error type.");
            return;
        }
        Sub("Line", std::to_string(location.line())).Sub("File", location.file_name()).Sub("At", GetDate());
    }

    template <LoggableMessage... Msgs>
    Log &Log::Sub(const Msgs &...titledSubMessage)
        requires IsEqual<2, Msgs...>
    {
        auto &&t = std::forward_as_tuple(titledSubMessage...);
        Write("    |___", std::get<0>(t), ": ", std::get<1>(t), '\n');
        return *this;
    }
}
