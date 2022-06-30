#include "parakeet_main.hpp"

std::vector<Move> generateMove(const unsigned char square, const Board& board) {
    std::vector<Move> moves;
    moves.emplace_back(1, 2, 0, 0, 0, 0);
    Piece piece = board.position[square];

    switch(piece.type) {
        case EMPTY:
            break;
        case KING:
            // king moves

            break;
        case QUEEN:
            // queen moves

            break;
        case BISHOP:
            // bishop moves

            break;
        case KNIGHT:
            // knight moves

            break;
        case PAWN:
            // pawn moves

            break;
    }

    return moves;
}