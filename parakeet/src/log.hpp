#pragma once

enum class LogLevel {
    ERROR, WARN, INFO, DEBUG
};

#define LOG_LEVEL LogLevel::DEBUG

template<typename T>
void Log(LogLevel level, const T& msg);