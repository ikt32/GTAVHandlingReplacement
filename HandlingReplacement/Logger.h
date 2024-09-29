#pragma once
#include <string>

enum LogLevel {
    DEBUG,
    INFO,
    WARN,
    ERROR_,
    FATAL,
};

class Logger {
public:
    static void SetFile(const std::string& fileName);
    static void SetMinLevel(LogLevel level);
    static void Clear();
    static void Write(LogLevel level, const std::string& text);
    static void Write(LogLevel level, const char* fmt, ...);
};
