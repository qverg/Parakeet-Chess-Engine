#include "board.hpp"
#include "log.hpp"

#define WITHIN_BOUNDS(c)    c.x >= 0 && c.x < 8 && c.y >= 0 && c.y < 8
#define COORD_TO_SQUARE(c)  c.y * 8 + c.x
#define SQUARE_TO_COORD(sq) {sq%8, sq/8}

Board::Board() {
    castlingRightsKingSide[Side::WHITE]  = true;    castlingRightsKingSide[Side::BLACK]  = true;
    castlingRightsQueenSide[Side::WHITE] = true;    castlingRightsQueenSide[Side::BLACK] = true;
    enPassantPossible = false;
    sideToPlay = Side::WHITE;
    lastDoublePawnPush = 64;
}

// inefficient!! only use when time is unimportant
Board::Board(std::array<Piece, 64>& position, Side sideToPlay,
    bool whiteCanCastleKingSide, bool whiteCanCastleQueenSide, bool blackCanCastleKingSide, bool blackCanCastleQueenSide,
    bool enPassantPossible, unsigned short lastDoublePawnPush)
    : position(position), sideToPlay(sideToPlay), enPassantPossible(enPassantPossible), lastDoublePawnPush(lastDoublePawnPush)
{
    castlingRightsKingSide[Side::WHITE]  = whiteCanCastleKingSide;
    castlingRightsKingSide[Side::BLACK]  = blackCanCastleKingSide;
    castlingRightsQueenSide[Side::WHITE] = whiteCanCastleQueenSide;
    castlingRightsQueenSide[Side::BLACK] = blackCanCastleQueenSide;

    for (int i = 0; i < 64; i++) {
        if (position[i].type == PieceType::KING) kingPositions[position[i].side] = SQUARE_TO_COORD(i);
    }
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
        } else if (!move.special1 && !move.special0) { // knight-promotion
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
        kingPositions[piece.side] = SQUARE_TO_COORD(move.after);
    }

    if (piece.side == Side::WHITE) {
        if (move.before == 0) castlingRightsQueenSide[Side::WHITE] = false;
        else if (move.before == 7) castlingRightsKingSide[Side::WHITE] = false;

        if (move.after == 56) castlingRightsQueenSide[Side::BLACK] = false;
        else if (move.after == 63) castlingRightsKingSide[Side::BLACK] = false;

    } else {
        if (move.before == 56) castlingRightsQueenSide[Side::BLACK] = false;
        else if (move.before == 63) castlingRightsKingSide[Side::BLACK] = false;

        if (move.after == 0) castlingRightsQueenSide[Side::WHITE] = false;
        else if (move.after == 7) castlingRightsKingSide[Side::WHITE] = false;
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
    
    for (int i = 0; i < 8; i++) {
        position[i+8]   = {PieceType::PAWN, Side::WHITE};
        position[i+48]  = {PieceType::PAWN, Side::BLACK};
    }

    kingPositions[Side::WHITE] = {4,0}; kingPositions[Side::BLACK] = {4,7};
    castlingRightsKingSide[Side::WHITE]  = true;    castlingRightsKingSide[Side::BLACK]  = true;
    castlingRightsQueenSide[Side::WHITE] = true;    castlingRightsQueenSide[Side::BLACK] = true;
    enPassantPossible = false;
    lastDoublePawnPush = 64;
    sideToPlay = Side::WHITE;
}

void Board::generateMoves(const unsigned short square, std::vector<Move>& moves) {
    Log(LogLevel::DEBUG, "Generating moves");

    Piece piece = position[square];
    Side opponent = (piece.side == Side::WHITE) ? Side::BLACK : Side::WHITE;

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
                    } else if (position[newSquare].side == opponent) {
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
            generateMovesInDirection(c, dirs::north,     moves, opponent);
            generateMovesInDirection(c, dirs::south,     moves, opponent);
            generateMovesInDirection(c, dirs::east,      moves, opponent);
            generateMovesInDirection(c, dirs::west,      moves, opponent);

            generateMovesInDirection(c, dirs::southeast, moves, opponent);
            generateMovesInDirection(c, dirs::northeast, moves, opponent);
            generateMovesInDirection(c, dirs::northwest, moves, opponent);
            generateMovesInDirection(c, dirs::southwest, moves, opponent);
        } break;

        case PieceType::BISHOP: {
            // bishop moves
            generateMovesInDirection(c, dirs::northeast, moves, opponent);
            generateMovesInDirection(c, dirs::southeast, moves, opponent);
            generateMovesInDirection(c, dirs::northwest, moves, opponent);
            generateMovesInDirection(c, dirs::southwest, moves, opponent);
        } break;

        case PieceType::KNIGHT: {

            // knight moves
            std::array<Coordinate, 8> candidates = {
                dirs::north(dirs::north(dirs::east(c))),
                dirs::north(dirs::north(dirs::west(c))),
                dirs::north(dirs::east (dirs::east(c))),
                dirs::north(dirs::west (dirs::west(c))),
                dirs::south(dirs::south(dirs::east(c))),
                dirs::south(dirs::south(dirs::west(c))),
                dirs::south(dirs::east (dirs::east(c))),
                dirs::south(dirs::west (dirs::west(c)))
            };

            for (Coordinate& candidate : candidates) {
                if (WITHIN_BOUNDS(candidate)) {
                    unsigned short newSquare = COORD_TO_SQUARE(candidate);
                    if (position[newSquare].type == PieceType::EMPTY) {
                        moves.emplace_back(square, newSquare);
                    } else if (position[newSquare].side == opponent) {
                        moves.emplace_back(square, newSquare, 1);
                    }
                }
            }
        } break;

        case PieceType::ROOK: {
            // rook moves
            generateMovesInDirection(c, dirs::north,    moves, opponent);
            generateMovesInDirection(c, dirs::south,    moves, opponent);
            generateMovesInDirection(c, dirs::east,     moves, opponent);
            generateMovesInDirection(c, dirs::west,     moves, opponent);
        } break;
        case PieceType::PAWN: {
            // pawn moves
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
                    moves.emplace_back(square, square+forwardOffset, 1, 0, 0, 0);   // knight
                    moves.emplace_back(square, square+forwardOffset, 1, 0, 0, 1);   // bishop
                }
            }
            // captures
            // right
            if (c.x < 7 && square+forwardOffset+1 < 64 && position[square+forwardOffset+1].side == opponent) {
                if (squareBeforeLastTwoRanks) {
                    moves.emplace_back(square, square+forwardOffset+1, 0, 1, 0, 0);
                } else {
                    // promo captures - right
                    moves.emplace_back(square, square+forwardOffset+1, 1, 1, 1, 1);   // queen
                    moves.emplace_back(square, square+forwardOffset+1, 1, 1, 1, 0);   // rook
                    moves.emplace_back(square, square+forwardOffset+1, 1, 1, 0, 0);   // knight
                    moves.emplace_back(square, square+forwardOffset+1, 1, 1, 0, 1);   // bishop
                }
            }
            // left
            if (c.x > 0 && square + forwardOffset-1 >= 0 && position[square+forwardOffset-1].side == opponent) {
                if (squareBeforeLastTwoRanks) {
                    moves.emplace_back(square, square+forwardOffset-1, 0, 1, 0, 0);
                } else {
                    // promo captures - left
                    moves.emplace_back(square, square+forwardOffset-1, 1, 1, 1, 1);   // queen
                    moves.emplace_back(square, square+forwardOffset-1, 1, 1, 1, 0);   // rook
                    moves.emplace_back(square, square+forwardOffset-1, 1, 1, 0, 0);   // knight
                    moves.emplace_back(square, square+forwardOffset-1, 1, 1, 0, 1);   // bishop
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
        const Coordinate& coord,
        const std::function<Coordinate(Coordinate)>& directionFunc,
        std::vector<Move>& moves,
        const Side& opponent
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

PieceType Board::getNextOpponentPieceInDirection(
        const Coordinate& coord,
        const std::function<Coordinate(Coordinate)>& directionFunc,
        const Side& opponent
    ) const {
    
    unsigned short nextSquare;
    Coordinate nextCoord;
    Coordinate lastCoord = coord;

    for (int i = 1; i < 8; i++) {
        nextCoord = directionFunc(lastCoord);

        if (WITHIN_BOUNDS(nextCoord)) {
            nextSquare = COORD_TO_SQUARE(nextCoord);
            PieceType nextType = position[nextSquare].type;

            if (position[nextSquare].side == opponent) { // capture
                return nextType;
            } else if (nextType != PieceType::EMPTY) break;

        } else break;
        lastCoord = nextCoord;
    }

    return PieceType::EMPTY;

}

bool Board::sideInCheck(const Side& side) {
    /* We check for knight positions and cast rays around the king*/
    Coordinate king_pos = kingPositions[side];
    
    Side opponent = (side == Side::WHITE) ? Side::BLACK : Side::WHITE;

    auto checkForPieceAtCoord = [&](Coordinate coord, PieceType type) {
        if (WITHIN_BOUNDS(coord)) {
            Piece* piece_at_coord = &position[COORD_TO_SQUARE(coord)];
            if (piece_at_coord->side == opponent && piece_at_coord->type == type)
                return true;
        }
        return false;
    };

    // Look for knights
    std::array<Coordinate, 8> candidates = {
                dirs::north(dirs::north(dirs::east(king_pos))),
                dirs::north(dirs::north(dirs::west(king_pos))),
                dirs::north(dirs::east (dirs::east(king_pos))),
                dirs::north(dirs::west (dirs::west(king_pos))),
                dirs::south(dirs::south(dirs::east(king_pos))),
                dirs::south(dirs::south(dirs::west(king_pos))),
                dirs::south(dirs::east (dirs::east(king_pos))),
                dirs::south(dirs::west (dirs::west(king_pos)))
    };

    for (Coordinate& candidate : candidates) {
        if (checkForPieceAtCoord(candidate, PieceType::KNIGHT)) return true;
    }

    //Log(LogLevel::DEBUG, "No knight checks found");

    // Look for enemy pawns in front

    if (side == Side::WHITE) {
        Coordinate right = dirs::northeast(king_pos);
        if (checkForPieceAtCoord(right, PieceType::PAWN)) return true;

        Coordinate left = dirs::northwest(king_pos);
        if (checkForPieceAtCoord(left, PieceType::PAWN)) return true;

    } else {
        Coordinate right = dirs::southeast(king_pos);
        if (checkForPieceAtCoord(right, PieceType::PAWN)) return true;

        Coordinate left = dirs::southwest(king_pos);
        if (checkForPieceAtCoord(left, PieceType::PAWN)) return true;
    }

    //Log(LogLevel::DEBUG, "No pawn checks found");

    // Look for sliding pieces
    {   // namespace for all these variables
        PieceType check_n = getNextOpponentPieceInDirection(king_pos, dirs::north, opponent);
        if (check_n == PieceType::QUEEN || check_n == PieceType::ROOK) return true;

        PieceType check_s = getNextOpponentPieceInDirection(king_pos, dirs::south, opponent);
        if (check_s == PieceType::QUEEN || check_s == PieceType::ROOK) return true;

        PieceType check_e = getNextOpponentPieceInDirection(king_pos, dirs::east, opponent);
        if (check_e == PieceType::QUEEN || check_e == PieceType::ROOK) return true;

        PieceType check_w = getNextOpponentPieceInDirection(king_pos, dirs::west, opponent);
        if (check_w == PieceType::QUEEN || check_w == PieceType::ROOK) return true;

        //Log(LogLevel::DEBUG, "No lateral checks found");

        PieceType check_ne = getNextOpponentPieceInDirection(king_pos, dirs::northeast, opponent);
        if (check_ne == PieceType::QUEEN || check_ne == PieceType::BISHOP) return true;

        PieceType check_nw = getNextOpponentPieceInDirection(king_pos, dirs::northwest, opponent);
        if (check_nw == PieceType::QUEEN || check_nw == PieceType::BISHOP) return true;

        PieceType check_se = getNextOpponentPieceInDirection(king_pos, dirs::southeast, opponent);
        if (check_se == PieceType::QUEEN || check_se == PieceType::BISHOP) return true;

        PieceType check_sw = getNextOpponentPieceInDirection(king_pos, dirs::southwest, opponent);
        if (check_sw == PieceType::QUEEN || check_sw == PieceType::BISHOP) return true;

        //Log(LogLevel::DEBUG, "No diagonal checks found");
    }

    return false;
}