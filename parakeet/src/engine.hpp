#pragma once

#include <limits>

#include "board.hpp"
#include "types/movecounter.hpp"

class Engine {
public:
    Board board;

private:

    std::unordered_map<PieceType, int> pieceValues;

    const int infinity = 1000000;

    int evaluate(const Board& board) const;
    int search(
        const Board& initialBoard,
        Move& bestMove,
        const int depth,
        int alpha,
        const int beta
    ) const;
    
    void countMoves(
        const Board& board,
        std::unordered_map<int, MoveCounter>& countersPerDepth,
        const int depth=1
    ) const;

public:
    Engine();

    int evaluate() const;
    int search(Move& bestMove, const int depth) const;
    void play();

    void countMoves(const int depth=1) const;
};