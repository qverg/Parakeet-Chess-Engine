#pragma once
#include <chrono>

// this is from cherno's video https://www.youtube.com/watch?v=YG4jexlSAjc
class Timer {
public:
    Timer ();
    ~Timer();
    void stop();
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_startTimepoint;
};