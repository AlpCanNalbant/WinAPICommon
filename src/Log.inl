// Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

namespace Wcm::Impl
{
    template <LoggableMessage... T>
    const Log &Log::Write(const Character auto *mark, const T &...explanations) const
    {
        static const auto makeStreamThread = [&]
        {
            (streamThread_ = std::make_unique<std::jthread>([&]
            {
                for (auto i = 0uz; i < streamThreads_.size(); ++i)
                {
                    static const auto addStreamThread = [&]
                    {
                        streamThreads_.emplace_back(std::make_unique<std::jthread>([&]
                        {
                            if (!OutputFile.empty())
                            {
                                std::wofstream stream{std::filesystem::path(OutputFile), std::ios_base::app}; // Append instead of overwrite.
#ifndef NDEBUG
                                if (!stream.is_open())
                                {
                                    std::wcout << "File stream cannot be opened.\n";
                                    return;
                                }
#endif
                                stream << mark << ' ';
                                const auto streamExpl = [&](const auto &expl)
                                {
                                    stream << Wcm::ToWStringIf(expl);
                                };
                                (streamExpl(explanations), ...);
                                stream.close();
#ifndef NDEBUG
                                if (stream.fail())
                                {
                                    std::wcout << "An error occurred while closing the file stream.\n";
                                    return;
                                }
#endif
                            }
#ifndef NDEBUG
                            std::wcout << mark << ' ';
                            const auto outputExpl = [](const auto &expl)
                            {
                                std::wcout << Wcm::ToWStringIf(expl);
                            };
                            (outputExpl(explanations), ...);
#endif
                            return;
                        }))->detach();
                    };
                    if (!streamThreads_[i]->joinable())
                    {
                        streamThreads_.erase(streamThreads_.cbegin() + i);
                    }
                    if (writeRequested_)
                    {
                        addStreamThread();
                        writeRequested_ = false;
                    }
                }
            }))->detach();
        };
        if (streamThread_->joinable())
        {
            writeRequested_ = true;
        }
        else
        {
            makeStreamThread();
        }
        return *this;
    }

    template <LoggableMessage... T>
    const Log &Log::WriteLine(const Character auto *mark, const T &...explanations) const
    {
        Write(mark, explanations..., '\n');
        return *this;
    }

    template <LoggableMessage T>
    const Log &Log::Info(const T &explanation) const
    {
        WriteLine(InfoMark, explanation);
        Sub("At", GetDate());
        return *this;
    }

    template <LoggableMessage T>
    const Log &Log::Error(const T &reason, const std::source_location &location) const
    {
        Error(reason, ErrorType::Normal, location);
        return *this;
    }

    template <LoggableMessage T>
    const Log &Log::Error(const T &reason, const HRESULT errorCode, const std::source_location &location) const
    {
        errorCode_ = errorCode;
        Error(reason, ErrorType::WinAPI, location);
        return *this;
    }

    template <LoggableMessage T>
    void Log::Error(const T &reason, const ErrorType type, const std::source_location &location) const
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

    template <LoggableMessage... T>
    inline const Log &Log::Sub(const T &...titledSubMessage) const
        requires IsEqual<2, T...>
    {
        auto &&t = std::forward_as_tuple(titledSubMessage...);
        Write("    |___", std::get<0>(t), ": ", std::get<1>(t), '\n');
        return *this;
    }
}
