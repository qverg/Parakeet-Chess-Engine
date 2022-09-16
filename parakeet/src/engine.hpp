#pragma once

#include "board.hpp"

class Engine {
public:
    Board board;

private:
    float evaluate(const Board& board);
    void search(const Board& initialBoard);
    int countMoves(Board& board, const int depth=1) const;

public:
    Engine();

    float evaluate();
    void search();
    int countMoves(const int depth=1);
};