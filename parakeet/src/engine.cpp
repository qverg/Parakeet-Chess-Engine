#include "engine.hpp"

#include <vector>
#include <iostream>

Engine::Engine() {
    board = Board();
}

float Engine::evaluate() {
    evaluate(board);
}


float Engine::evaluate(const Board& board) {
    return 0.0f;
}

void Engine::search() {
    search(board);
}

void Engine::search(const Board& initialBoard) {
    
}

void printMoveCounter(const MoveCounter& counter, const int depth) {
    std::cout << "Depth " << depth << std::endl;
    std::cout << "Moves:     " << counter.moves << std::endl;
    std::cout << "Captures:  " << counter.captures << std::endl;
    std::cout << "En passant:" << counter.enPassant << std::endl;
    std::cout << "Castles:   " << counter.castles << std::endl;
    std::cout << "Promotions:" << counter.promotions << std::endl;
    std::cout << std::endl;
}

void Engine::countMoves(int depth) {
    std::unordered_map<int, MoveCounter> countersPerDepth;
    for (int i = depth; i > 0; i--) {
        countersPerDepth[i] = MoveCounter();
    }
    countMoves(board, countersPerDepth, depth);

    for (; depth > 0; depth--) {
        printMoveCounter(countersPerDepth[depth], depth);
    }
}



void Engine::countMoves(Board& board, std::unordered_map<int, MoveCounter>& countersPerDepth, const int depth) {
    MoveCounter& counter = countersPerDepth[depth];
    for (int square = 0; square < 64; square++) {
        if (board.position[square].side == board.sideToPlay) {
            std::vector<Move> moves;
            board.generateMoves(square, moves);

            for (const auto& move : moves) {
                counter.moves++;
                if (move.capture) counter.captures++;
                if (move.isEnPassant()) counter.enPassant++;
                if (move.isCastle()) counter.castles++;
                if (move.promotion) counter.promotions++;

                if (depth > 1) {
                    Board newBoard = board;
                    newBoard.makeMove(move);
                    countMoves(newBoard, countersPerDepth, depth-1);
                }
            }


        }
    }
}