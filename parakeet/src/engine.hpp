#pragma once

//#include <limits>

#include "board.hpp"
#include "types/movecounter.hpp"

class Engine {
public:
    Board board;

private:

    std::unordered_map<PieceType, int> m_pieceValues;

    const int m_depth = 3;

    const int infinity = 1000000;

    int evaluate(const Board& board) const;
    int search(
        Board& initialBoard,
        const int depth,
        int alpha,
        const int beta
    );
    
    void countMoves(
        const Board& board,
        std::unordered_map<int, MoveCounter>& countersPerDepth,
        const int depth=1
    ) const;

public:
    Engine();

    int evaluate() const;
    void play();

    void countMoves(const int depth=1) const;
};