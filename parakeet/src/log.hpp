#pragma once

enum class LogLevel {
    ERROR, WARN, INFO, DEBUG
};

#define LOG_LEVEL LogLevel::WARN

void Log(LogLevel level, std::string msg);