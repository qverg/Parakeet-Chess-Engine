#include "engine.hpp"
#include <vector>

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

int Engine::countMoves(const int depth) {
    return countMoves(board, depth);
}

int Engine::countMoves(Board& board, const int depth) const {
    int count = 0;
    for (int square = 0; square < 64; square++) {
        if (board.position[square].side == board.sideToPlay) {
            std::vector<Move> moves;
            board.generateMoves(square, moves);

            count += moves.size();

            if (depth > 1) {
                for (const auto& move : moves) {
                    Board newBoard = board;
                    newBoard.makeMove(move);
                    count += countMoves(newBoard, depth-1);
                }
            }
        }
    }

    return count;
}