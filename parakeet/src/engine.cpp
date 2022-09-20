#include "engine.hpp"
#include "log.hpp"

#include <vector>
#include <iostream>
//#include <algorithm>    // std::max

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

int Engine::evaluate() const {
    return evaluate(board);
}


int Engine::evaluate(const Board& board) const{
    if (board.sideToPlay == Side::WHITE)
        return board.materialDifference;

    return -board.materialDifference;
}

int Engine::search(Move& bestMove, const int depth) const {
    return search(board, bestMove, depth, -infinity, infinity);
}

int Engine::search(const Board& initialBoard, Move& bestMove, const int depth, int alpha, const int beta) const {
    if (depth == 0) return evaluate(initialBoard);
    
    
    std::vector<Move> moves;
    board.generateAllMoves(moves);

    if (moves.size() == 0) {
        //if (initialBoard.check.at(initialBoard.sideToPlay)) return -infinity;
        Log(LogLevel::DEBUG, "Stalemate found");
        return 0;
    }

    for (const auto& move : moves) {
        Board newBoard = initialBoard;
        newBoard.makeMove(move);

        Move tempMove;
        const int eval = -search(newBoard, tempMove, depth-1, -beta, -alpha);

        if (eval >= beta) {
            return beta;
        }
        if (eval > alpha) {
            alpha = eval;
            bestMove = move;
        }
    }

    return alpha;
}

void Engine::play() {

    Move bestMove;

    Log(LogLevel::DEBUG, "Starting search");

    if (board.sideToPlay == Side::WHITE)
        const int eval = search(bestMove, 5);
    else const int eval = -search(bestMove, 5);

    Log(LogLevel::DEBUG, "Search complete");

    if (bestMove.beforeAndAfterDifferent()) {
        board.makeMove(bestMove);
    } else {
        Log(LogLevel::INFO, "No moves found");
    }
}


//==========================================================================
// Perft stuff
void Engine::countMoves(const int depth) const {
    std::unordered_map<int, MoveCounter> countersPerDepth;
    for (int i = depth; i > 0; i--) {
        countersPerDepth[i] = MoveCounter();
    }
    countMoves(board, countersPerDepth, depth);

    for (int i = depth; i > 0; i--) {
        countersPerDepth.at(i).print(depth-i+1);
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