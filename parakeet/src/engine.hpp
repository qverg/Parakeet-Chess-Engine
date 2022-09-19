#pragma once

#include "board.hpp"

struct MoveCounter {
    int moves = 0;
    int captures = 0;
    int enPassant = 0; 
    int castles = 0;
    int promotions = 0;
    int checks = 0;

    void operator+=(const MoveCounter& counter) {
        moves += counter.moves;
        captures += counter.captures;
        enPassant += counter.enPassant;
        castles += counter.castles;
        promotions += counter.promotions;
        checks += counter.checks;
    }
};

class Engine {
public:
    Board board;

private:

    std::unordered_map<PieceType, int> pieceValues;

    float evaluate(const Board& board);
    void search(const Board& initialBoard, const int depth);
    void countMoves(Board& board, std::unordered_map<int, MoveCounter>& countersPerDepth, const int depth=1);

public:
    Engine();

    float evaluate();
    void search(const int depth);
    void countMoves(const int depth=1);
};