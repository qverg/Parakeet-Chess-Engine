#pragma once

#include "board.hpp"

class Engine {
public:
    Board board;

private:

public:
    Engine();

    float evaluate();
    float evaluate(Board& board);

    void search();
    void search(Board& initialBoard);
};