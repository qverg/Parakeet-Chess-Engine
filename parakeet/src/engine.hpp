#pragma once

#include "board.hpp"
#include "types/movecounter.hpp"

class Engine {
public:
    Board board;

private:

    std::unordered_map<PieceType, int> pieceValues;

    float evaluate(const Board& board);
    void search(const Board& initialBoard, const int depth);
    
    void countMoves(const Board& board, std::unordered_map<int, MoveCounter>& countersPerDepth, const int depth=1) const;

public:
    Engine();

    float evaluate();
    void search(const int depth);

    void countMoves(const int depth=1) const;
};