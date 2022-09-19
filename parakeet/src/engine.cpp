#include "engine.hpp"

#include <vector>
#include <iostream>

Engine::Engine() {
    board = Board();

    pieceValues[PieceType::PAWN] = 100;
    pieceValues[PieceType::KNIGHT] = 300;
    pieceValues[PieceType::BISHOP] = 350;
    pieceValues[PieceType::ROOK] = 500;
    pieceValues[PieceType::QUEEN] = 900;
    pieceValues[PieceType::KING] = 1000000;

    board.setPieceValues(pieceValues);
}

float Engine::evaluate() {
    evaluate(board);
}


float Engine::evaluate(const Board& board) {
    return board.materialDifference;
}

void Engine::search(const int depth) {
    search(board, depth);
}

void Engine::search(const Board& initialBoard, const int depth) {
    for (int square = 0; square < 64; square++) {
        if (board.position[square].side == board.sideToPlay) {
            std::vector<Move> moves;
            board.generateMoves(square, moves);

            for (const auto& move : moves) {
                // do some evaluating
                // add mat diff attribute to move!
            }
        }
    }
}


//==========================================================================
// Perft stuff
void printMoveCounter(const MoveCounter& counter, const int depth) {
    std::cout << "Depth " << depth << std::endl;
    std::cout << "Moves:     " << counter.moves << std::endl;
    std::cout << "Captures:  " << counter.captures << std::endl;
    std::cout << "En passant:" << counter.enPassant << std::endl;
    std::cout << "Castles:   " << counter.castles << std::endl;
    std::cout << "Promotions:" << counter.promotions << std::endl;
    std::cout << "Checks:    " << counter.checks << std::endl;
    std::cout << std::endl;
}

void Engine::countMoves(const int depth) const {
    std::unordered_map<int, MoveCounter> countersPerDepth;
    for (int i = depth; i > 0; i--) {
        countersPerDepth[i] = MoveCounter();
    }
    countMoves(board, countersPerDepth, depth);

    for (int i = depth; i > 0; i--) {
        printMoveCounter(countersPerDepth.at(i), depth-i+1);
    }
}



void Engine::countMoves(const Board& board, std::unordered_map<int, MoveCounter>& countersPerDepth, const int depth) const {
    MoveCounter& counter = countersPerDepth.at(depth);
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
                if (move.willBeCheck) counter.checks++;

                if (depth > 1) {
                    Board newBoard = board;
                    newBoard.makeMove(move);
                    countMoves(newBoard, countersPerDepth, depth-1);
                }
            }


        }
    }
}