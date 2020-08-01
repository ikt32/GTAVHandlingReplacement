#include "Logger.h"

#include <Windows.h>

#include <iomanip>
#include <fstream>
#include <mutex>
#include <vector>

namespace {
    const std::vector<std::string> levelStrings{
        " DEBUG ",
        " INFO  ",
        "WARNING",
        " ERROR ",
        " FATAL ",
    };

    std::string file;
    LogLevel minLevel = LogLevel::DEBUG;
    std::mutex mutex;

    std::string levelText(LogLevel level) {
        return levelStrings[level];
    }
}

void Logger::SetFile(const std::string &fileName) {
    file = fileName;
}

void Logger::SetMinLevel(LogLevel level) {
    minLevel = level;
}

void Logger::Clear() {
    std::lock_guard lock(mutex);
    std::ofstream logFile(file, std::ofstream::out | std::ofstream::trunc);
}

void Logger::Write(LogLevel level, const std::string& text) {
#ifndef _DEBUG
    if (level < minLevel) return;
#endif
    std::lock_guard lock(mutex);
    std::ofstream logFile(file, std::ios_base::out | std::ios_base::app);
    SYSTEMTIME currTimeLog;
    GetLocalTime(&currTimeLog);
    logFile << "[" <<
        std::setw(2) << std::setfill('0') << currTimeLog.wHour << ":" <<
        std::setw(2) << std::setfill('0') << currTimeLog.wMinute << ":" <<
        std::setw(2) << std::setfill('0') << currTimeLog.wSecond << "." <<
        std::setw(3) << std::setfill('0') << currTimeLog.wMilliseconds << "] " <<
        "[" << levelText(level) << "] " <<
        text << "\n";
}

void Logger::Write(LogLevel level, const char *fmt, ...) {
    const int size = 1024;
    char buff[size];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buff, size, fmt, args);
    va_end(args);
    Write(level, std::string(buff));
}
