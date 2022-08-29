#include <cmath>    // std::abs

#include "board.hpp"
#include "log.hpp"

#define WITHIN_BOUNDS(c)    c.x >= 0 && c.x < 8 && c.y >= 0 && c.y < 8
#define COORD_TO_SQUARE(c)  c.y * 8 + c.x
#define SQUARE_TO_COORD(sq) {sq%8, sq/8}

Coordinate Coordinate::distance(const Coordinate& c1, const Coordinate& c2) {
    return {std::abs(c1.x-c2.x), std::abs(c1.y-c2.y)};
}

Board::Board() {
    castlingRightsKingSide[Side::WHITE]  = true;    castlingRightsKingSide[Side::BLACK]  = true;
    castlingRightsQueenSide[Side::WHITE] = true;    castlingRightsQueenSide[Side::BLACK] = true;

    check[Side::WHITE] = false; check[Side::BLACK] = false;

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
        if (position[i].type == PieceType::KING) {
            kingsData.positions[position[i].side] = SQUARE_TO_COORD(i);
            getKnightAttackCoordsAtCoord(SQUARE_TO_COORD(i), kingsData.knightAttacks[position[i].side]);
        }
    }

    check[Side::WHITE] = sideInCheck(Side::WHITE);
    check[Side::BLACK] = sideInCheck(Side::BLACK);
}

void Board::makeMove(const Move& move) {
    Piece piece = position[move.before];    // has to be by value (no pointer!)


    if (enPassantPossible) enPassantPossible = false;

    if (move.promotion) {
        if (move.special1 && move.special0) { // queen-promotion
            piece.type = PieceType::QUEEN;
        } else if (move.special1 && !move.special0) { // rook-promotion
            piece.type = PieceType::ROOK;
        } else if (!move.special1 && move.special0) { // bishop-promotion
            piece.type = PieceType::BISHOP;
        } else if (!move.special1 && !move.special0) { // knight-promotion
            piece.type = PieceType::KNIGHT;
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
            if (!check[piece.side]) {
                position[move.after-1] = {PieceType::ROOK, piece.side};
                position[move.after+1] = EMPTY_SQUARE;
            } else {
                return;
            }
        } else if (move.special1 && move.special0) { // queen-side castle
            if (!check[piece.side]) {
                position[move.after+1] = {PieceType::ROOK, piece.side};
                position[move.after-2] = EMPTY_SQUARE;
            } else {
                return;
            }
        }
    }

    // castling rights
    if (piece.type == PieceType::KING) {
        castlingRightsKingSide[piece.side] = false;
        castlingRightsQueenSide[piece.side] = false;
        kingsData.positions[piece.side] = SQUARE_TO_COORD(move.after);
        getKnightAttackCoordsAtCoord(kingsData.positions[piece.side], kingsData.knightAttacks[piece.side]);
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

    position[move.after] = piece;
    position[move.before] = EMPTY_SQUARE;

    sideToPlay = (sideToPlay == Side::WHITE) ? Side::BLACK : Side::WHITE;
    
    check[sideToPlay] = move.willBeCheck;
    check[piece.side] = false;  // you can't move so that you will be in check -
                                // this has to be implemented during move generation
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

    kingsData.positions[Side::WHITE] = {4,0}; 
    kingsData.positions[Side::BLACK] = {4,7};

    getKnightAttackCoordsAtCoord(kingsData.positions[Side::WHITE], kingsData.knightAttacks[Side::WHITE]);
    getKnightAttackCoordsAtCoord(kingsData.positions[Side::BLACK], kingsData.knightAttacks[Side::BLACK]);

    castlingRightsKingSide[Side::WHITE]  = true;    castlingRightsKingSide[Side::BLACK]  = true;
    castlingRightsQueenSide[Side::WHITE] = true;    castlingRightsQueenSide[Side::BLACK] = true;
    enPassantPossible = false;
    lastDoublePawnPush = 64;
    sideToPlay = Side::WHITE;
}

void Board::generateMoves(const unsigned short square, std::vector<Move>& moves) {
    Piece* piece = &position[square];
    if (piece->side != sideToPlay) {
        return;
    }

    Log(LogLevel::DEBUG, "Generating moves");

    Side opponent = (piece->side == Side::WHITE) ? Side::BLACK : Side::WHITE;

    Coordinate c = {square % 8, square / 8};

    switch(piece->type) {
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
                        moves.emplace_back(square, newSquare, false);
                    } else if (position[newSquare].side == opponent) {
                        moves.emplace_back(square, newSquare, 1, false);
                    }
                }
            }

            if (castlingRightsKingSide[piece->side] 
                && position[square+1].type == PieceType::EMPTY
                && position[square+2].type == PieceType::EMPTY) {

                if (getNextOpponentPieceInDirection(c, dirs::west, opponent) == PieceType::KING) {
                    moves.emplace_back(square, square+2, 0, 0, 1, 0, true); // king-side castle
                } else {
                    if (piece->side == Side::WHITE) {
                        if (getNextOpponentPieceInDirection(c, dirs::north, Side::BLACK) == PieceType::KING)
                            moves.emplace_back(square, square+2, 0, 0, 1, 0, true); // king-side castle
                        else moves.emplace_back(square, square+2, 0, 0, 1, 0, false); // king-side castle

                    } else {
                        if (getNextOpponentPieceInDirection(c, dirs::south, Side::WHITE) == PieceType::KING)
                            moves.emplace_back(square, square+2, 0, 0, 1, 0, true); // king-side castle
                        else moves.emplace_back(square, square+2, 0, 0, 1, 0, false); // king-side castle
                    }
                }
            }

            if (castlingRightsQueenSide[piece->side] 
                && position[square-1].type == PieceType::EMPTY
                && position[square-2].type == PieceType::EMPTY
                && position[square-3].type == PieceType::EMPTY) {

                if (getNextOpponentPieceInDirection(c, dirs::east, opponent) == PieceType::KING) {
                    moves.emplace_back(square, square-2, 0, 0, 1, 1, true); // queen-side castle
                } else {
                    if (piece->side == Side::WHITE) {
                        if (getNextOpponentPieceInDirection(c, dirs::north, Side::BLACK) == PieceType::KING)
                            moves.emplace_back(square, square-2, 0, 0, 1, 1, true); // queen-side castle
                        else moves.emplace_back(square, square-2, 0, 0, 1, 1, false); // queen-side castle
                    } else {
                        if (getNextOpponentPieceInDirection(c, dirs::south, Side::WHITE) == PieceType::KING)
                            moves.emplace_back(square, square-2, 0, 0, 1, 1, true); // queen-side castle
                        else moves.emplace_back(square, square-2, 0, 0, 1, 1, false); // queen-side castle
                    }
                }
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
            std::array<Coordinate, 8> candidates;
            getKnightAttackCoordsAtCoord(c, candidates);

            for (Coordinate& candidate : candidates) {
                if (WITHIN_BOUNDS(candidate)) {
                    unsigned short newSquare = COORD_TO_SQUARE(candidate);
                    if (position[newSquare].type == PieceType::EMPTY) {
                        moves.emplace_back(square, newSquare, false);
                    } else if (position[newSquare].side == opponent) {

                        Coordinate distanceToOpponentKing = Coordinate::distance(kingsData.positions[opponent], candidate);
                        if ((distanceToOpponentKing.x == 2 && distanceToOpponentKing.y == 1)
                            || (distanceToOpponentKing.x == 1 && distanceToOpponentKing.y == 2)) {
                            moves.emplace_back(square, newSquare, 1, true); // will be attacking opponent king
                        } else {
                            moves.emplace_back(square, newSquare, 1, false);   // will not be attacking opponent king
                        }


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

            if (piece->side == Side::WHITE) {
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
                    moves.emplace_back(square, square+forwardOffset, pawnAttackingOppKingAtCoord(SQUARE_TO_COORD(square+forwardOffset), opponent));
                    if (square / 8 == homeRank && position[square+forwardOffset*2].type == PieceType::EMPTY) {
                        moves.emplace_back(square, square+forwardOffset*2, 0, 0, 0, 1,
                        pawnAttackingOppKingAtCoord(SQUARE_TO_COORD(square+forwardOffset*2), opponent));  // double pawn push
                    }
                } else {
                    // promotions
                    // the willBeCheck arguments are all false temporarily!!
                    moves.emplace_back(square, square+forwardOffset, 1, 0, 1, 1, false);   // queen
                    moves.emplace_back(square, square+forwardOffset, 1, 0, 1, 0, false);   // rook
                    moves.emplace_back(square, square+forwardOffset, 1, 0, 0, 0, false);   // knight
                    moves.emplace_back(square, square+forwardOffset, 1, 0, 0, 1, false);   // bishop
                }
            }
            // captures
            // right
            if (c.x < 7 && square+forwardOffset+1 < 64 && position[square+forwardOffset+1].side == opponent) {
                if (squareBeforeLastTwoRanks) {
                    moves.emplace_back(square, square+forwardOffset+1, 0, 1, 0, 0, pawnAttackingOppKingAtCoord(SQUARE_TO_COORD(square+forwardOffset+1), opponent));
                } else {
                    // promo captures - right
                    moves.emplace_back(square, square+forwardOffset+1, 1, 1, 1, 1, false);   // queen
                    moves.emplace_back(square, square+forwardOffset+1, 1, 1, 1, 0, false);   // rook
                    moves.emplace_back(square, square+forwardOffset+1, 1, 1, 0, 0, false);   // knight
                    moves.emplace_back(square, square+forwardOffset+1, 1, 1, 0, 1, false);   // bishop
                }
            }
            // left
            if (c.x > 0 && square + forwardOffset-1 >= 0 && position[square+forwardOffset-1].side == opponent) {
                if (squareBeforeLastTwoRanks) {
                    moves.emplace_back(square, square+forwardOffset-1, 0, 1, 0, 0, pawnAttackingOppKingAtCoord(SQUARE_TO_COORD(square+forwardOffset-1), opponent));
                } else {
                    // promo captures - left
                    moves.emplace_back(square, square+forwardOffset-1, 1, 1, 1, 1, false);   // queen
                    moves.emplace_back(square, square+forwardOffset-1, 1, 1, 1, 0, false);   // rook
                    moves.emplace_back(square, square+forwardOffset-1, 1, 1, 0, 0, false);   // knight
                    moves.emplace_back(square, square+forwardOffset-1, 1, 1, 0, 1, false);   // bishop
                }
            }

            // en passant capture
            if (enPassantPossible && square/8 == enPassantRank) {
                if (lastDoublePawnPush == square+1)
                    moves.emplace_back(square, square+forwardOffset+1, 0, 1, 0, 1, pawnAttackingOppKingAtCoord(SQUARE_TO_COORD(square+forwardOffset+1), opponent));

                else if (lastDoublePawnPush == square-1)
                    moves.emplace_back(square, square+forwardOffset-1, 0, 1, 0, 1, pawnAttackingOppKingAtCoord(SQUARE_TO_COORD(square+forwardOffset-1), opponent));
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
                if (getNextOpponentPieceInDirection(nextCoord, directionFunc, opponent) == PieceType::KING)
                    moves.emplace_back(COORD_TO_SQUARE(coord), nextSquare, 0, 0, 0, 0, true);
                else moves.emplace_back(COORD_TO_SQUARE(coord), nextSquare, 0, 0, 0, 0, false);
                lastCoord = nextCoord;
            } else if (position[nextSquare].side == opponent) { // capture
                if (getNextOpponentPieceInDirection(nextCoord, directionFunc, opponent) == PieceType::KING)
                    moves.emplace_back(COORD_TO_SQUARE(coord), nextSquare, 0, 1, 0, 0, true);
                else moves.emplace_back(COORD_TO_SQUARE(coord), nextSquare, 0, 1, 0, 0, false);
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
    Log(LogLevel::INFO, "Checking for check!"); // Leaving this here to optimise when we're looking for checks later


    Coordinate king_pos = kingsData.positions[side];
    
    Side opponent = (side == Side::WHITE) ? Side::BLACK : Side::WHITE;

    // Checking only for opponent built in to lambda!
    auto checkForPieceAtCoord = [&](Coordinate coord, PieceType type) {
        if (WITHIN_BOUNDS(coord)) {
            Piece* piece_at_coord = &position[COORD_TO_SQUARE(coord)];
            if (piece_at_coord->side == opponent && piece_at_coord->type == type)
                return true;
        }
        return false;
    };

    // Look for knights
    std::array<Coordinate, 8> candidates;
    getKnightAttackCoordsAtCoord(king_pos, candidates);

    for (Coordinate& candidate : kingsData.knightAttacks[side]) {
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

void Board::getKnightAttackCoordsAtCoord(const Coordinate& coord, std::array<Coordinate, 8>& attacks) {
    attacks = {
                dirs::north(dirs::north(dirs::east(coord))),
                dirs::north(dirs::north(dirs::west(coord))),
                dirs::north(dirs::east (dirs::east(coord))),
                dirs::north(dirs::west (dirs::west(coord))),
                dirs::south(dirs::south(dirs::east(coord))),
                dirs::south(dirs::south(dirs::west(coord))),
                dirs::south(dirs::east (dirs::east(coord))),
                dirs::south(dirs::west (dirs::west(coord)))
    };
}

bool Board::pawnAttackingOppKingAtCoord(const Coordinate& pawnCoord, const Side& pawnSide) {
        if (pawnSide == Side::WHITE) {
            Coordinate kingCoord = kingsData.positions[Side::BLACK];
            if (kingCoord.y == pawnCoord.y + 1 && (kingCoord.x == pawnCoord.x+1 || kingCoord.x == pawnCoord.x-1))
                    return true;

        } else {
            Coordinate kingCoord = kingsData.positions[Side::WHITE];
            if (kingCoord.y == pawnCoord.y - 1 && (kingCoord.x == pawnCoord.x+1 || kingCoord.x == pawnCoord.x-1))
                    return true;
        }
        return false;
    }