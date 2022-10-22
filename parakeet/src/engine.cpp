#include "engine.hpp"
#include "log.hpp"
#include "timer.hpp"

#include <vector>
#include <iostream>

Engine::Engine() {
    board = Board();
    
    m_pieceValues[PieceType::PAWN] = 100;
    m_pieceValues[PieceType::KNIGHT] = 300;
    m_pieceValues[PieceType::BISHOP] = 350;
    m_pieceValues[PieceType::ROOK] = 500;
    m_pieceValues[PieceType::QUEEN] = 900;
    m_pieceValues[PieceType::KING] = 1000000;

    board.setPieceValues(m_pieceValues);
    board.fillKnightAttacksArray();
    board.fillKingMovesArray();
}

int Engine::evaluate() const {
    return evaluate(board);
}


int Engine::evaluate(const Board& board) const{
    if (board.sideToPlay == Side::WHITE)
        return board.materialDifference;

    return -board.materialDifference;
}

int Engine::search(Board& initialBoard, const int depth, int alpha, const int beta) {
    if (depth == 0) return evaluate(initialBoard);
    
    
    std::vector<Move> moves;
    initialBoard.generateAllMoves(moves);

    if (moves.size() == 0) {
        //if (initialBoard.check.at(initialBoard.sideToPlay)) return -infinity;
        Log(LogLevel::DEBUG, "Stalemate found");
        return 0;
    }

    for (const auto& move : moves) {
        Board newBoard = initialBoard;
        newBoard.makeMove(move);

        const int eval = -search(newBoard, depth-1, -beta, -alpha);

        if (eval >= beta) {
            return beta;
        }
        if (eval > alpha) {
            alpha = eval;
        }
    }

    return alpha;
}

void Engine::play() {    
    std::vector<Move> moves;
    board.generateAllMoves(moves);
    int bestEval = -infinity;
    Move bestMove;
    
    Log(LogLevel::DEBUG, "Starting search");
    {
        Timer timer;
        for (const Move& move : moves) {
            Board newBoard = board;
            newBoard.makeMove(move);

            const int eval = -search(newBoard, 3, -infinity, infinity);
            
            if (eval > bestEval) {
                bestEval = eval;
                bestMove = move;
            }
        }
    }
    Log(LogLevel::DEBUG, "Search complete");


    if (bestMove.beforeAndAfterDifferent()) {
        board.makeMove(bestMove);
    } else {
        Log(LogLevel::INFO, "No moves found");
    }
    Log(LogLevel::INFO, board.materialDifference);
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