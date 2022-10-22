#include "positiondatastack.hpp"

#include <cassert>

PositionDataStack::PositionDataStack() {}

void PositionDataStack::init(const int maxSize) {
    for (int i = 0; i < maxSize; i++) {
        m_positionDataVector.emplace_back();
    }
    m_highestIndex = -1;
}

LastPositionData* PositionDataStack::push() {
    assert(m_highestIndex + 1 < m_maxSize);
    m_highestIndex += 1;
    return &m_positionDataVector[m_highestIndex];
}

LastPositionData* PositionDataStack::pop() {
    assert(m_highestIndex > -1);
    m_highestIndex -= 1;
    return &m_positionDataVector[m_highestIndex+1];
}