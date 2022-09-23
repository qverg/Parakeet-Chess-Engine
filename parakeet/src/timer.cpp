#include "timer.hpp"

#include <iostream>

Timer::Timer() {
    m_startTimepoint = std::chrono::high_resolution_clock::now();
}

Timer::~Timer() {
    stop();
}

void Timer::stop() {
    const auto endTimepoint = std::chrono::high_resolution_clock::now();
    
    const auto start = std::chrono::time_point_cast<std::chrono::microseconds>(m_startTimepoint).time_since_epoch().count();
    const auto end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

    const auto duration = end - start;
    const double ms = duration * 0.001;

    std::cout << "$Duration: " << ms << " ms" << std::endl;
}