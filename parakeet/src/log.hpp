#pragma once
#include <iostream>

enum class LogLevel {
    ERROR, WARN, INFO, DEBUG
};

extern LogLevel LOG_LEVEL;

template<typename T>
void Log(LogLevel level, const T& msg) {
    if (level <= LOG_LEVEL) {
        switch (level) {
            case (LogLevel::ERROR): {
                std::cerr << "$ERROR: " << msg << std::endl;
            } break;
            case (LogLevel::WARN): {
                std::cout << "$WARNING: " << msg << std::endl;
            } break;
            case (LogLevel::INFO): {
                std::cout << "$INFO: " << msg << std::endl;
            } break;
            case (LogLevel::DEBUG): {
                std::cout << "$DEBUG: " << msg << std::endl;
            } break;
        }        
    }
}