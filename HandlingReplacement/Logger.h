#pragma once
#include <filesystem>
#include <format>
#include <mutex>
#include <string>

#define LOG(level, fmt, ...) \
    gLogger.Write(level, fmt, __VA_ARGS__)

enum ELogLevel {
    Debug,
    Info,
    Warning,
    Error,
    Fatal,
};

class CLogger {
public:
    void SetPath(const std::filesystem::path& logFilePath);
    void SetLogLevel(ELogLevel level);
    void Clear();
    bool IsInError() const;
    void ClearError();

    template <typename... Args>
    void Write(ELogLevel level, std::string_view fmt, Args&&... args) {
        try {
            WriteImpl(level, std::vformat(fmt, std::make_format_args(args...)));
        }
        catch (const std::exception& ex) {
            WriteImpl(Error, std::format("Failed to format: [{}], {}", fmt, ex.what()));
        }
    }

private:
    void WriteImpl(ELogLevel, const std::string& txt);

    std::filesystem::path mLogFilePath;
    ELogLevel mLogLevel = Info;

    mutable bool mError = false;
    mutable std::mutex mMutex;
};

extern CLogger gLogger;
