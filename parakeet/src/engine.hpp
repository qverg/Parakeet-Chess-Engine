#pragma once

#include "board.hpp"

class Engine {
public:
    Board board;

private:

public:
    Engine();

    float evaluate(Board& board);
    void  search(Board& initialBoard);
};