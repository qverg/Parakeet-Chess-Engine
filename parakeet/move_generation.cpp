#include "parakeet_main.hpp"
#include <functional>

#define WITHIN_BOUNDS(x) x[0] >= 0 && x[0] < 8 && x[1] >= 0 && x[1] < 8
#define COORD_TO_SQUARE(x) x[1] * 8 + x[0]

using Coordinate = std::array<int, 2>;

static void generateMovesInDirection(const Coordinate coord, const Board& board, const std::function<Coordinate(Coordinate)>& directionFunc,
                                std::vector<Move>& moves, const Side playingAs, const Side opponent) {
    unsigned short nextSquare;
    Coordinate nextCoord;
    Coordinate lastCoord = coord;

    for (int i = 1; i < 8; i++) {
        nextCoord = directionFunc(lastCoord);
        //LOG(nextCoord[0] << ", " << nextCoord[1]);

        nextSquare = COORD_TO_SQUARE(nextCoord);

        if (WITHIN_BOUNDS(nextCoord)) {
            if (board.position[nextSquare].type == PieceType::EMPTY) {
                moves.emplace_back(COORD_TO_SQUARE(coord), nextSquare, 0, 0, 0, 0);
                //LOG("Empty " << nextSquare);
                lastCoord = nextCoord;
            } else if (board.position[nextSquare].side == opponent) { // capture
                moves.emplace_back(COORD_TO_SQUARE(coord), nextSquare, 0, 1, 0, 0);
                //LOG("Captured " << nextSquare);
                break;
            } else break;

        } else break;
    }
}

namespace dirs {
    static Coordinate north(Coordinate c)   { return {c[0], c[1]+1}; }
    static Coordinate south(Coordinate c)   { return {c[0], c[1]+1}; }
    static Coordinate east(Coordinate c)    { return {c[0], c[1]+1}; }
    static Coordinate west(Coordinate c)    { return {c[0], c[1]+1}; }
    static Coordinate northeast(Coordinate c) { return {c[0], c[1]+1}; }
    static Coordinate southeast(Coordinate c) { return {c[0], c[1]+1}; }
    static Coordinate northwest(Coordinate c) { return {c[0], c[1]+1}; }
    static Coordinate southwest(Coordinate c) { return {c[0], c[1]+1}; }
}

std::vector<Move> generateMoves(const unsigned short square, Board& board) {
    std::vector<Move> moves;
    Piece piece = board.position[square];
    
    /*  // maybe use macros instead of functions for performance?
        // doesn't really matter because for bishop/rook/queen I'm using lambdas of these anyway
    #define NORTH(x) {x[0], x[1]+1}
    #define SOUTH(x) {x[0], x[1]-1}
    #define EAST(x) {x[0]+1, x[1]}
    #define WEST(x) {x[0]-1, x[1]}
    #define NORTHEAST(x) {x[0]+1, x[1]+1}
    #define SOUTHEAST(x) {x[0]+1, x[1]-1}
    #define NORTHWEST(x) {x[0]-1, x[1]+1}
    #define SOUTHWEST(x) {x[0]-1, x[1]-1}
    */

    Coordinate c = {square % 8, square / 8};

    switch(piece.type) {
        case PieceType::EMPTY:
            break;
        case PieceType::KING: {
            // king moves
            std::array<Coordinate, 8> candidates = { {
                dirs::north(c), dirs::south(c), dirs::east(c), dirs::west(c),
                dirs::northeast(c), dirs::southeast(c), dirs::northwest(c), dirs::southwest(c)
            } };

            for (Coordinate& candidate : candidates) {
                unsigned short newSquare = COORD_TO_SQUARE(candidate);
                if (WITHIN_BOUNDS(candidate)) {
                    if (board.position[newSquare].type == PieceType::EMPTY) {
                        moves.emplace_back(square, newSquare);
                    } else if (board.position[newSquare].side == Side::BLACK) {
                        moves.emplace_back(square, newSquare, 1);
                    }
                }
            }

            if (board.castlingRights[piece.side]) {
                if (board.position[square+1].type == PieceType::EMPTY && board.position[square+2].type == PieceType::EMPTY) {
                    moves.emplace_back(square, square+2, 0, 0, 1, 0); // king-side castle
                }
                if (board.position[square-1].type == PieceType::EMPTY && board.position[square-2].type == PieceType::EMPTY 
                    && board.position[square-3].type == PieceType::EMPTY) {
                    moves.emplace_back(square, square-2, 0, 0, 1, 1); // queen-side castle
                }
            }
        } break;

        case PieceType::QUEEN: {
            // queen moves
            // write function for casting ray in certain direction
            Side opponent = (piece.side == Side::WHITE) ? Side::BLACK : Side::WHITE;
            generateMovesInDirection(c, board, dirs::north, moves, piece.side, opponent);
            generateMovesInDirection(c, board, dirs::south, moves, piece.side, opponent);
            generateMovesInDirection(c, board, dirs::east, moves, piece.side, opponent);
            generateMovesInDirection(c, board, dirs::west, moves, piece.side, opponent);

            generateMovesInDirection(c, board, dirs::northeast, moves, piece.side, opponent);
            generateMovesInDirection(c, board, dirs::southeast, moves, piece.side, opponent);
            generateMovesInDirection(c, board, dirs::northwest, moves, piece.side, opponent);
            generateMovesInDirection(c, board, dirs::southwest, moves, piece.side, opponent);
        } break;
        case PieceType::BISHOP: {
            // bishop moves
            Side opponent = (piece.side == Side::WHITE) ? Side::BLACK : Side::WHITE;
            generateMovesInDirection(c, board, dirs::northeast, moves, piece.side, opponent);
            generateMovesInDirection(c, board, dirs::southeast, moves, piece.side, opponent);
            generateMovesInDirection(c, board, dirs::northwest, moves, piece.side, opponent);
            generateMovesInDirection(c, board, dirs::southwest, moves, piece.side, opponent);
        } break;
        case PieceType::KNIGHT: {
            // knight moves

        } break;
        case PieceType::ROOK: {
            // rook moves
            Side opponent = (piece.side == Side::WHITE) ? Side::BLACK : Side::WHITE;
            generateMovesInDirection(c, board, dirs::north, moves, piece.side, opponent);
            generateMovesInDirection(c, board, dirs::south, moves, piece.side, opponent);
            generateMovesInDirection(c, board, dirs::east, moves, piece.side, opponent);
            generateMovesInDirection(c, board, dirs::west, moves, piece.side, opponent);
        } break;
        case PieceType::PAWN: {
            // pawn moves
            // remember to mark promotions and double pawn pushes!

        } break;
    }

    return moves;
}

