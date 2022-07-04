#include "board.hpp"

#define WITHIN_BOUNDS(c)    c.x >= 0 && c.x < 8 && c.y >= 0 && c.y < 8
#define COORD_TO_SQUARE(c)  c.y * 8 + c.x

Board::Board() {
    castlingRightsKingSide[Side::WHITE]  = true;    castlingRightsKingSide[Side::BLACK]  = true;
    castlingRightsQueenSide[Side::WHITE] = true;    castlingRightsQueenSide[Side::BLACK] = true;
    enPassantPossible = false;
    sideToPlay = Side::WHITE;
    lastDoublePawnPush = 64;
}

Board::Board(std::array<Piece, 64>& position, Side sideToPlay,
    bool whiteCanCastleKingSide, bool whiteCanCastleQueenSide, bool blackCanCastleKingSide, bool blackCanCastleQueenSide,
    bool enPassantPossible, unsigned short lastDoublePawnPush)
    : position(position), sideToPlay(sideToPlay), enPassantPossible(enPassantPossible), lastDoublePawnPush(lastDoublePawnPush)
{
    castlingRightsKingSide[Side::WHITE] = whiteCanCastleKingSide;
    castlingRightsKingSide[Side::BLACK] = blackCanCastleKingSide;
    castlingRightsQueenSide[Side::WHITE] = whiteCanCastleQueenSide;
    castlingRightsQueenSide[Side::BLACK] = blackCanCastleQueenSide;
}

void Board::makeMove(const Move& move) {
    Piece piece = position[move.before];

    position[move.after] = piece;
    position[move.before] = EMPTY_SQUARE;

    if (enPassantPossible) enPassantPossible = false;

    if (move.promotion) {
        if (move.special1 && move.special0) { // queen-promotion
            position[move.after].type = PieceType::QUEEN;
        } else if (move.special1 && !move.special0) { // rook-promotion
            position[move.after].type = PieceType::ROOK;
        } else if (!move.special1 && move.special0) { // bishop-promotion
            position[move.after].type = PieceType::BISHOP;
        } else { // knight-promotion
            position[move.after].type = PieceType::KNIGHT;
        }
    } else if (move.capture) {
        if (move.special0) { // en passant
            if (piece.side == Side::WHITE) {
                position[move.after-8] = EMPTY_SQUARE;
            } else {
                position[move.after+8] = EMPTY_SQUARE;
            }
        }
    } else {
        if (!move.special1 && move.special0) { // double pawn push
            enPassantPossible = true;
            lastDoublePawnPush = move.after;
        } else if (move.special1 && !move.special0) { // king-side castle
            position[move.after-1] = {PieceType::ROOK, piece.side};
            position[move.after+1] = EMPTY_SQUARE;
        } else if (move.special1 && move.special0) { // queen-side castle
            position[move.after+1] = {PieceType::ROOK, piece.side};
            position[move.after-2] = EMPTY_SQUARE;
        }
    }

    // castling rights
    if (piece.type == PieceType::KING) {
        castlingRightsKingSide[piece.side] = false;
        castlingRightsQueenSide[piece.side] = false;
    }

    if (piece.side == Side::WHITE) {
        if (move.before == 0) castlingRightsQueenSide[Side::WHITE] == false;
        else if (move.before == 0) castlingRightsKingSide[Side::WHITE] == false;

        if (move.after == 56) castlingRightsQueenSide[Side::BLACK] == false;
        else if (move.after == 63) castlingRightsKingSide[Side::BLACK] == false;

    } else {
        if (move.before == 56) castlingRightsQueenSide[Side::BLACK] == false;
        else if (move.before == 63) castlingRightsKingSide[Side::BLACK] == false;

        if (move.after == 0) castlingRightsQueenSide[Side::WHITE] == false;
        else if (move.after == 0) castlingRightsKingSide[Side::WHITE] == false;
    }

}

void Board::reset() {

    for (int i = 0; i < 64; i++) {
        position[i] = EMPTY_SQUARE;
    }

    position[0] = {PieceType::ROOK, Side::WHITE};    position[7] = {PieceType::ROOK, Side::WHITE};
    position[1] = {PieceType::KNIGHT, Side::WHITE};  position[6] = {PieceType::KNIGHT, Side::WHITE};
    position[2] = {PieceType::BISHOP, Side::WHITE};  position[5] = {PieceType::BISHOP, Side::WHITE};
    position[3] = {PieceType::QUEEN, Side::WHITE};   position[4] = {PieceType::KING, Side::WHITE};

    position[56] = {PieceType::ROOK, Side::BLACK};   position[63] = {PieceType::ROOK, Side::BLACK};
    position[57] = {PieceType::KNIGHT, Side::BLACK}; position[62] = {PieceType::KNIGHT, Side::BLACK};
    position[58] = {PieceType::BISHOP, Side::BLACK}; position[61] = {PieceType::BISHOP, Side::BLACK};
    position[59] = {PieceType::QUEEN, Side::BLACK};  position[60] = {PieceType::KING, Side::BLACK};
    /*
    for (int i = 0; i < 8; i++) {
        position[i+8]   = {PAWN, WHITE};
        position[i+48]  = {PAWN, WHITE};
    }
    */
    castlingRightsKingSide[Side::WHITE]  = true;    castlingRightsKingSide[Side::BLACK]  = true;
    castlingRightsQueenSide[Side::WHITE] = true;    castlingRightsQueenSide[Side::BLACK] = true;
    enPassantPossible = false;
    lastDoublePawnPush = 64;
    sideToPlay = Side::WHITE;
}

void Board::generateMoves(const unsigned short square, std::vector<Move>& moves) {
    Log(LogLevel::DEBUG, "Generating moves");

    Piece piece = position[square];

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
            std::array<Coordinate, 8> candidates = {
                dirs::north(c),     dirs::south(c),     dirs::east(c),      dirs::west(c),
                dirs::northeast(c), dirs::southeast(c), dirs::northwest(c), dirs::southwest(c)
            };

            for (Coordinate& candidate : candidates) {
                unsigned short newSquare = COORD_TO_SQUARE(candidate);
                if (WITHIN_BOUNDS(candidate)) {
                    if (position[newSquare].type == PieceType::EMPTY) {
                        moves.emplace_back(square, newSquare);
                    } else if (position[newSquare].side == Side::BLACK) {
                        moves.emplace_back(square, newSquare, 1);
                    }
                }
            }

            if (castlingRightsKingSide[piece.side] 
                && position[square+1].type == PieceType::EMPTY
                && position[square+2].type == PieceType::EMPTY) {
                moves.emplace_back(square, square+2, 0, 0, 1, 0); // king-side castle
            }
            if (castlingRightsQueenSide[piece.side] 
                && position[square-1].type == PieceType::EMPTY
                && position[square-2].type == PieceType::EMPTY
                && position[square-3].type == PieceType::EMPTY) {
                moves.emplace_back(square, square-2, 0, 0, 1, 1); // queen-side castle
            }

        } break;

        case PieceType::QUEEN: {
            // queen moves
            // write function for casting ray in certain direction
            Side opponent = (piece.side == Side::WHITE) ? Side::BLACK : Side::WHITE;
            generateMovesInDirection(c, dirs::north,     moves, piece.side, opponent);
            generateMovesInDirection(c, dirs::south,     moves, piece.side, opponent);
            generateMovesInDirection(c, dirs::east,      moves, piece.side, opponent);
            generateMovesInDirection(c, dirs::west,      moves, piece.side, opponent);

            generateMovesInDirection(c, dirs::northeast, moves, piece.side, opponent);
            generateMovesInDirection(c, dirs::southeast, moves, piece.side, opponent);
            generateMovesInDirection(c, dirs::northwest, moves, piece.side, opponent);
            generateMovesInDirection(c, dirs::southwest, moves, piece.side, opponent);
        } break;

        case PieceType::BISHOP: {
            // bishop moves
            Side opponent = (piece.side == Side::WHITE) ? Side::BLACK : Side::WHITE;
            generateMovesInDirection(c, dirs::northeast, moves, piece.side, opponent);
            generateMovesInDirection(c, dirs::southeast, moves, piece.side, opponent);
            generateMovesInDirection(c, dirs::northwest, moves, piece.side, opponent);
            generateMovesInDirection(c, dirs::southwest, moves, piece.side, opponent);
        } break;

        case PieceType::KNIGHT: {
            // knight moves
            std::array<Coordinate, 8> candidates = {
                dirs::north(dirs::north(dirs::east(c))),
                dirs::north(dirs::north(dirs::west(c))),
                dirs::north(dirs::east(dirs::east(c))),
                dirs::north(dirs::west(dirs::west(c))),
                dirs::south(dirs::south(dirs::east(c))),
                dirs::south(dirs::south(dirs::west(c))),
                dirs::south(dirs::east(dirs::east(c))),
                dirs::south(dirs::west(dirs::west(c)))
            };

            for (Coordinate& candidate : candidates) {
                unsigned short newSquare = COORD_TO_SQUARE(candidate);
                if (WITHIN_BOUNDS(candidate)) {
                    if (position[newSquare].type == PieceType::EMPTY) {
                        moves.emplace_back(square, newSquare);
                    } else if (position[newSquare].side == Side::BLACK) {
                        moves.emplace_back(square, newSquare, 1);
                    }
                }
            }
        } break;

        case PieceType::ROOK: {
            // rook moves
            Side opponent = (piece.side == Side::WHITE) ? Side::BLACK : Side::WHITE;
            generateMovesInDirection(c, dirs::north,    moves, piece.side, opponent);
            generateMovesInDirection(c, dirs::south,    moves, piece.side, opponent);
            generateMovesInDirection(c, dirs::east,     moves, piece.side, opponent);
            generateMovesInDirection(c, dirs::west,     moves, piece.side, opponent);
        } break;
        case PieceType::PAWN: {
            // pawn moves
            Log(LogLevel::DEBUG, "Piece type: pawn");

            short forwardOffset;

            // min and max are exclusive!
            bool squareBeforeLastTwoRanks;
            unsigned short homeRank;
            unsigned short enPassantRank;

            Side opponent;

            if (piece.side == Side::WHITE) {
                forwardOffset = 8;
                squareBeforeLastTwoRanks = (square < 48);
                homeRank = 1;
                enPassantRank = 4;
                opponent = Side::BLACK;
            } else {
                forwardOffset = -8;
                squareBeforeLastTwoRanks = (square > 15);
                homeRank = 6;
                enPassantRank = 3;
                opponent = Side::WHITE;
            }

            if (position[square+forwardOffset].type == PieceType::EMPTY) {
                if (squareBeforeLastTwoRanks) {
                    moves.emplace_back(square, square+forwardOffset);
                    if (square / 8 == homeRank && position[square+forwardOffset*2].type == PieceType::EMPTY) {
                        moves.emplace_back(square, square+forwardOffset*2, 0, 0, 0, 1);  // double pawn push
                    }
                } else {
                    // promotions
                    moves.emplace_back(square, square+forwardOffset, 1, 0, 1, 1);   // queen
                    moves.emplace_back(square, square+forwardOffset, 1, 0, 1, 0);   // rook
                    moves.emplace_back(square, square+forwardOffset, 1, 0, 0, 1);   // bishop
                    moves.emplace_back(square, square+forwardOffset, 1, 0, 0, 0);   // knight
                }
            }
            // captures
            if (square+forwardOffset+1 < 64 && position[square+forwardOffset+1].side == opponent) {
                if (squareBeforeLastTwoRanks) {
                    moves.emplace_back(square, square+forwardOffset+1, 1, 1, 0, 0);
                } else {
                    // promo captures - right
                    moves.emplace_back(square, square+forwardOffset+1, 1, 1, 1, 1);   // queen
                    moves.emplace_back(square, square+forwardOffset+1, 1, 1, 1, 0);   // rook
                    moves.emplace_back(square, square+forwardOffset+1, 1, 1, 0, 1);   // bishop
                    moves.emplace_back(square, square+forwardOffset+1, 1, 1, 0, 0);   // knight
                }
            }
            if (square + forwardOffset-1 >= 0 && position[square+forwardOffset-1].side == opponent) {
                if (squareBeforeLastTwoRanks) {
                    moves.emplace_back(square, square+forwardOffset-1, 1, 1, 0, 0);
                } else {
                    // promo captures - left
                    moves.emplace_back(square, square+forwardOffset-1, 1, 1, 1, 1);   // queen
                    moves.emplace_back(square, square+forwardOffset-1, 1, 1, 1, 0);   // rook
                    moves.emplace_back(square, square+forwardOffset-1, 1, 1, 0, 1);   // bishop
                    moves.emplace_back(square, square+forwardOffset-1, 1, 1, 0, 0);   // knight
                }
            }

            // en passant capture
            if (enPassantPossible && square/8 == enPassantRank) {
                if (lastDoublePawnPush == square+1)
                    moves.emplace_back(square, square+forwardOffset+1, 0, 1, 0, 1);

                else if (lastDoublePawnPush == square-1)
                    moves.emplace_back(square, square+forwardOffset-1, 0, 1, 0, 1);
            }

        } break;
    }
    Log(LogLevel::DEBUG, "Move generation done");
}


void Board::generateMovesInDirection(
        // probably make this argument list better somehow.
        // I'd like it to be efficient (i.e. not calculate coord every time) but also concise (not too many).
        // The ONLY reason I'm using coords at all is for the WITHIN_BOUNDS check
        // There's probably some clever better way of implementing this function.
        const Coordinate coord,
        const std::function<Coordinate(Coordinate)>& directionFunc,
        std::vector<Move>& moves,
        const Side playingAs,
        const Side opponent
    ) const {
    unsigned short nextSquare;
    Coordinate nextCoord;
    Coordinate lastCoord = coord;

    for (int i = 1; i < 8; i++) {
        nextCoord = directionFunc(lastCoord);
        nextSquare = COORD_TO_SQUARE(nextCoord);

        if (WITHIN_BOUNDS(nextCoord)) {

            if (position[nextSquare].type == PieceType::EMPTY) {
                moves.emplace_back(COORD_TO_SQUARE(coord), nextSquare, 0, 0, 0, 0);
                lastCoord = nextCoord;
            } else if (position[nextSquare].side == opponent) { // capture
                moves.emplace_back(COORD_TO_SQUARE(coord), nextSquare, 0, 1, 0, 0);
                break;
            } else break;

        } else break;
    }
}

std::string Board::algebraic(Move& move) {
        std::string out = "";
        switch (position[move.before].type) {
            case (PieceType::KING): out += "K"; break;
            case (PieceType::QUEEN): out += "Q"; break;
            case (PieceType::BISHOP): out += "B"; break;
            case (PieceType::KNIGHT): out += "N"; break;
            case (PieceType::ROOK): out += "R"; break;
        }

        out += (char) (move.after%8) + 'a';
        out += std::to_string(move.after/8 + 1);

        return out;
    }