#pragma once

#include "lastpositiondata.hpp"

#include <vector>

class PositionDataStack {
private:
    std::vector<LastPositionData> m_positionDataVector;
    int m_highestIndex;
    int m_maxSize;
public:
    PositionDataStack();
    void init(const int maxSize);
    LastPositionData* push();
    LastPositionData* pop();
};