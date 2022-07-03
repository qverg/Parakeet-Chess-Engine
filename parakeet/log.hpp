#pragma once

enum class LogLevel {
    ERROR, WARN, INFO, DEBUG
};

#define LOG_LEVEL LogLevel::DEBUG

void Log(LogLevel level, std::string msg);