#pragma once

#include "board.hpp"

class Engine {
public:
    Engine();

    float evaluate(Board& board);
    void  search(Board& initialBoard);
};