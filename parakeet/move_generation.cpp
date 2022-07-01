#include "parakeet_main.hpp"

std::vector<Move> generateMoves(const unsigned short square, const Board& board) {
    std::vector<Move> moves;
    Piece piece = board.position[square];
    
    #define NORTH(x) {x[0], x[1]+1}
    #define SOUTH(x) {x[0], x[1]-1}
    #define EAST(x) {x[0]+1, x[1]}
    #define WEST(x) {x[0]-1, x[1]}
    #define NORTHEAST(x) {x[0]+1, x[1]+1}
    #define SOUTHEAST(x) {x[0]+1, x[1]-1}
    #define NORTHWEST(x) {x[0]-1, x[1]+1}
    #define SOUTHWEST(x) {x[0]-1, x[1]-1}
    
    #define WITHIN_BOUNDS(x) x[0] >= 0 && x[0] < 8 && x[1] >= 0 && x[1] < 8
    #define COORD_TO_SQUARE(x) x[1] * 8 + x[0]

    std::array<int, 2> c = {square % 8, square / 8};

    switch(piece.type) {
        case EMPTY:
            break;
        case KING: {
            // king moves
            std::array<std::array<int, 2>, 8> candidates = { {
                NORTH(c), SOUTH(c), EAST(c), WEST(c),
                NORTHEAST(c), SOUTHEAST(c), NORTHWEST(c), SOUTHWEST(c)
            } };

            for (std::array<int, 2>& candidate : candidates) {
                unsigned short newSquare = COORD_TO_SQUARE(candidate);
                if (WITHIN_BOUNDS(candidate)) {
                    if (board.position[newSquare].type == EMPTY) {
                        moves.emplace_back(square, newSquare);
                    } else if (board.position[newSquare].side == BLACK) {
                        moves.emplace_back(square, newSquare, 1);
                    }
                }
            }

            if (board.castlingRights[piece.side]) {
                if (board.position[square+1].type == EMPTY && board.position[square+2].type == EMPTY) {
                    moves.emplace_back(square, square+2, 0, 0, 1, 0); // king-side castle
                }
                if (board.position[square-1].type == EMPTY && board.position[square-2].type == EMPTY && board.position[square-3].type == EMPTY) {
                    moves.emplace_back(square, square-2, 0, 0, 1, 1); // queen-side castle
                }
            }
        }
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
            // remember to mark promotions and double pawn pushes!

            break;
    }

    return moves;
}