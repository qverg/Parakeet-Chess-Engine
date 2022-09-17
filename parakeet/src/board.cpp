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
    check[piece.side] = false;  // you can't move so that you will be in check (implemented during move gen)
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

    Coordinate c = SQUARE_TO_COORD(square);

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
                        addMoveIfAcceptable(moves, {square, newSquare}, opponent, true, false);
                    } else if (position[newSquare].side == opponent) {
                        addMoveIfAcceptable(moves, {square, newSquare, 1}, opponent, true, false);
                    }
                }
            }
            if (!check[piece->side]) {
                if (castlingRightsKingSide[piece->side] 
                    && position[square+1].type == PieceType::EMPTY
                    && position[square+2].type == PieceType::EMPTY) {
                    
                    addMoveIfAcceptable(moves, {square, square+2, 0, 0, 1, 0}, opponent, true); // king-side castle
                }

                if (castlingRightsQueenSide[piece->side] 
                    && position[square-1].type == PieceType::EMPTY
                    && position[square-2].type == PieceType::EMPTY
                    && position[square-3].type == PieceType::EMPTY) {

                    addMoveIfAcceptable(moves, {square, square-2, 0, 0, 1, 1}, opponent, true); // queen-side castle
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
                        addMoveIfAcceptable(moves, {square, newSquare}, opponent, false, true);
                    } else if (position[newSquare].side == opponent) {
                        addMoveIfAcceptable(moves, {square, newSquare, 1}, opponent, false, true);
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
            int forwardOffset;

            bool squareBeforeLastTwoRanks;
            unsigned short homeRank;
            unsigned short enPassantRank;

            if (piece->side == Side::WHITE) {
                forwardOffset = 8;
                squareBeforeLastTwoRanks = (square < 48);
                homeRank = 1;
                enPassantRank = 4;
            } else {
                forwardOffset = -8;
                squareBeforeLastTwoRanks = (square > 15);
                homeRank = 6;
                enPassantRank = 3;
            }

            if (position[square+forwardOffset].type == PieceType::EMPTY) {
                if (squareBeforeLastTwoRanks) {
                    Move move = {square, square+forwardOffset};
                    addMoveIfAcceptable(moves, move, opponent);   // single pawn push
                    if (square / 8 == homeRank && position[square+forwardOffset*2].type == PieceType::EMPTY) {
                        addMoveIfAcceptable(moves, {square, square+forwardOffset*2, 0, 0, 0, 1}, opponent); // double pawn push
                    }
                } else {
                    // promotions
                    addAllPromotionsIfAcceptable(moves, {square, square+forwardOffset, 0}, opponent);
                }
            }
            // captures
            // right
            if (c.x < 7 && square+forwardOffset+1 < 64 && position[square+forwardOffset+1].side == opponent) {
                if (squareBeforeLastTwoRanks) {
                    addMoveIfAcceptable(moves, {square, square+forwardOffset+1, 1}, opponent);
                } else {
                    // promo captures - right
                    addAllPromotionsIfAcceptable(moves, {square, square+forwardOffset+1, 1}, opponent);
                }
            }
            // left
            if (c.x > 0 && square + forwardOffset-1 >= 0 && position[square+forwardOffset-1].side == opponent) {
                if (squareBeforeLastTwoRanks) {
                    addMoveIfAcceptable(moves, {square, square+forwardOffset-1, 1}, opponent);
                } else {
                    // promo captures - left
                    addAllPromotionsIfAcceptable(moves, {square, square+forwardOffset-1, 1}, opponent);
                }
            }

            // en passant capture
            if (enPassantPossible && square/8 == enPassantRank) {
                if (lastDoublePawnPush == square+1)
                    addMoveIfAcceptable(moves, {square, square+forwardOffset+1, 0, 1, 0, 1}, opponent, false, false, true);

                else if (lastDoublePawnPush == square-1)
                    addMoveIfAcceptable(moves, {square, square+forwardOffset-1, 0, 1, 0, 1}, opponent, false, false, true);
            }

        } break;
    }

    Log(LogLevel::DEBUG, "Move generation done");
}

void Board::addMoveIfAcceptable(
    std::vector<Move>& moves,
    Move move,
    const Side& opponent,
    const bool isKing,
    const bool isKnight,
    const bool enPassant
    ) {

    Piece* piece = &position[move.before];
    std::unordered_map<Side, bool> checksIfMove;
    getChecksIfMove(checksIfMove, move.before, move.after, *piece, isKing, isKnight, enPassant);

    if (checksIfMove[piece->side]) return;  // we can't put ourselves in check

    if (isKing && move.special1) {

        int duringCastleSquare;
        if (!move.special0) duringCastleSquare = move.after-1;  // king-side castle
        else duringCastleSquare = move.after+1;                 // queen-side castle
        
        std::array<Piece, 64> duringCastle;
        makeHypotheticalMoveInPosition(position, duringCastle, move.before, duringCastleSquare, *piece);

        Coordinate afterCoord = SQUARE_TO_COORD(duringCastleSquare);

        std::unordered_map<Side, Coordinate> kingPositions = kingsData.positions;
        kingPositions[piece->side] = afterCoord;

        std::unordered_map<Side, std::array<Coordinate, 8>> knightAttacksAroundKings = kingsData.knightAttacks;
        getKnightAttackCoordsAtCoord(afterCoord, knightAttacksAroundKings[piece->side]);

        if (sideInCheck(piece->side, duringCastle, kingPositions, knightAttacksAroundKings, true)) return;
    }


    if (checksIfMove[opponent])
        move.willBeCheck = true;
    
    moves.emplace_back(move);
    
}

void Board::addAllPromotionsIfAcceptable(
    std::vector<Move>& moves,
    const Move move, // in the form {before, after, capture} (not by reference because rvalues need to be possible)
    const Side& opponent
    ) {

    Side& side = position[move.before].side;

    std::array<Piece, 64> queenPromo;
    makeHypotheticalMoveInPosition(position, queenPromo, move.before, move.after, {PieceType::QUEEN, side});

    // check if we're not putting ourselves in check
    if (sideInCheck(side, queenPromo, kingsData.positions, kingsData.knightAttacks, true)) return;


    std::array<Piece, 64> rookPromo;
    std::array<Piece, 64> bishopPromo;
    std::array<Piece, 64> knightPromo;

    makeHypotheticalMoveInPosition(position, rookPromo, move.before, move.after, {PieceType::ROOK, side});
    makeHypotheticalMoveInPosition(position, bishopPromo, move.before, move.after, {PieceType::BISHOP, side});
    makeHypotheticalMoveInPosition(position, knightPromo, move.before, move.after, {PieceType::KNIGHT, side});

    moves.emplace_back((int)move.before, (int)move.after, 1, (int)move.capture, 1, 1,       // these casts are scuffed but oh well I mean they're ints anyway surely the compiler gets rid of this
        sideInCheck(opponent, queenPromo, kingsData.positions, kingsData.knightAttacks));   // queen promo

    moves.emplace_back((int)move.before, (int)move.after, 1, (int)move.capture, 0, 0,
        sideInCheck(opponent, knightPromo, kingsData.positions, kingsData.knightAttacks, true));  // knight promo

    moves.emplace_back((int)move.before, (int)move.after, 1, (int)move.capture, 1, 0,
        sideInCheck(opponent, rookPromo, kingsData.positions, kingsData.knightAttacks));    // rook promo

    moves.emplace_back((int)move.before, (int)move.after, 1, (int)move.capture, 0, 1,
        sideInCheck(opponent, bishopPromo, kingsData.positions, kingsData.knightAttacks));  // bishop promo

    // (see https://www.chessprogramming.org/Encoding_Moves)
}

// Just makes before empty and after the new piece
void Board::makeHypotheticalMoveInPosition(
        const std::array<Piece, 64>& oldPosition,
        std::array<Piece, 64>& newPosition,
        const unsigned int before,
        const unsigned int after,
        const Piece piece,
        const bool enPassant
    ) {
    
    newPosition = oldPosition;
    newPosition[after] = piece;
    newPosition[before] = EMPTY_SQUARE;

    if (enPassant) {
        if (after-before == 9 || after-before == -7) newPosition[before+1] = EMPTY_SQUARE;
        else if (after-before == 7 || after-before == -9) newPosition[before-1] = EMPTY_SQUARE;
    }
}

bool Board::getChecksIfMove(
        std::unordered_map<Side, bool>& checks,
        const int before,
        const int after,
        const Piece& piece,
        const bool isKing,
        const bool isKnight,
        const bool enPassant
        ) {

    std::array<Piece, 64> hypotheticalPos;
    makeHypotheticalMoveInPosition(position, hypotheticalPos, before, after, piece, enPassant);

    if (isKing) {
        Coordinate afterCoord = SQUARE_TO_COORD(after);

        std::unordered_map<Side, Coordinate> kingPositions = kingsData.positions;
        kingPositions[piece.side] = afterCoord;

        std::unordered_map<Side, std::array<Coordinate, 8>> knightAttacksAroundKings = kingsData.knightAttacks;
        getKnightAttackCoordsAtCoord(afterCoord, knightAttacksAroundKings[piece.side]);

        checks[Side::WHITE] =  sideInCheck(Side::WHITE,hypotheticalPos,kingPositions,knightAttacksAroundKings,true);
        checks[Side::BLACK] =  sideInCheck(Side::BLACK,hypotheticalPos,kingPositions,knightAttacksAroundKings,true);
    } else {
        checks[Side::WHITE] =  sideInCheck(Side::WHITE,hypotheticalPos,kingsData.positions,
            kingsData.knightAttacks, true);
        checks[Side::BLACK] =  sideInCheck(Side::BLACK,hypotheticalPos,kingsData.positions,
            kingsData.knightAttacks, true);
    }
}

void Board::generateMovesInDirection(
        const Coordinate& coord,
        const std::function<Coordinate(Coordinate)>& directionFunc,
        std::vector<Move>& moves,
        const Side& opponent
    ) {

    unsigned short nextSquare;
    Coordinate nextCoord;
    Coordinate lastCoord = coord;

    for (int i = 1; i < 8; i++) {
        nextCoord = directionFunc(lastCoord);
        nextSquare = COORD_TO_SQUARE(nextCoord);

        if (WITHIN_BOUNDS(nextCoord)) {

            if (position[nextSquare].type == PieceType::EMPTY) {
                addMoveIfAcceptable(moves, {COORD_TO_SQUARE(coord), nextSquare, 0, 0, 0, 0}, opponent);
                lastCoord = nextCoord;
            } else if (position[nextSquare].side == opponent) { // capture
                addMoveIfAcceptable(moves, {COORD_TO_SQUARE(coord), nextSquare, 0, 1, 0, 0}, opponent);
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
    return getNextOpponentPieceInDirection(coord, position, directionFunc, opponent);
}

PieceType Board::getNextOpponentPieceInDirection(
        const Coordinate& coord,
        const std::array<Piece, 64>& position,
        const std::function<Coordinate(Coordinate)>& directionFunc,
        const Side& opponent
    ) {
    
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

bool Board::sideInCheck(const Side& side, const bool includeKnights) {
    return sideInCheck(side, position, kingsData.positions, kingsData.knightAttacks, includeKnights);
}

bool Board::sideInCheck(
    const Side& side,
    const std::array<Piece, 64>& position,
    std::unordered_map<Side, Coordinate>& kingPositions,
    std::unordered_map<Side, std::array<Coordinate, 8>>& knightAttacksAroundKings,
    const bool includeKnights
    ) {
    /* We check for knight positions and cast rays around the king*/
    Log(LogLevel::INFO, "Checking for check!"); // Leaving this here to optimise when we're looking for checks later

    Coordinate king_pos = kingPositions[side];
    
    Side opponent = (side == Side::WHITE) ? Side::BLACK : Side::WHITE;

    // Checking only for opponent is built in to the following lambda!
    auto checkForPieceAtCoord = [&](Coordinate coord, PieceType type) {
        if (WITHIN_BOUNDS(coord)) {
            const Piece* piece_at_coord = &position[COORD_TO_SQUARE(coord)];
            if (piece_at_coord->side == opponent && piece_at_coord->type == type)
                return true;
        }
        return false;
    };

    // Look for knights
    if (includeKnights){
        std::array<Coordinate, 8> candidates;
        getKnightAttackCoordsAtCoord(king_pos, candidates);

        for (Coordinate& candidate : knightAttacksAroundKings[side]) {
            if (checkForPieceAtCoord(candidate, PieceType::KNIGHT)) return true;
        }
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
        PieceType check_n = getNextOpponentPieceInDirection(king_pos, position, dirs::north, opponent);
        if (check_n == PieceType::QUEEN || check_n == PieceType::ROOK) return true;

        PieceType check_s = getNextOpponentPieceInDirection(king_pos, position, dirs::south, opponent);
        if (check_s == PieceType::QUEEN || check_s == PieceType::ROOK) return true;

        PieceType check_e = getNextOpponentPieceInDirection(king_pos, position, dirs::east, opponent);
        if (check_e == PieceType::QUEEN || check_e == PieceType::ROOK) return true;

        PieceType check_w = getNextOpponentPieceInDirection(king_pos, position, dirs::west, opponent);
        if (check_w == PieceType::QUEEN || check_w == PieceType::ROOK) return true;

        //Log(LogLevel::DEBUG, "No lateral checks found");

        PieceType check_ne = getNextOpponentPieceInDirection(king_pos, position, dirs::northeast, opponent);
        if (check_ne == PieceType::QUEEN || check_ne == PieceType::BISHOP) return true;

        PieceType check_nw = getNextOpponentPieceInDirection(king_pos, position, dirs::northwest, opponent);
        if (check_nw == PieceType::QUEEN || check_nw == PieceType::BISHOP) return true;

        PieceType check_se = getNextOpponentPieceInDirection(king_pos, position, dirs::southeast, opponent);
        if (check_se == PieceType::QUEEN || check_se == PieceType::BISHOP) return true;

        PieceType check_sw = getNextOpponentPieceInDirection(king_pos, position, dirs::southwest, opponent);
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

