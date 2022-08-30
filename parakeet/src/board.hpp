#pragma once

#include <array>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>

#include "types.hpp"
#include "move.hpp"

struct Coordinate {
    int x; int y;

    static Coordinate distance(const Coordinate& c1, const Coordinate& c2);
};

namespace dirs {
    inline Coordinate south      (Coordinate c)   { return {c.x, c.y-1};   }
    inline Coordinate north      (Coordinate c)   { return {c.x, c.y+1};   }
    inline Coordinate east       (Coordinate c)   { return {c.x+1, c.y};   }
    inline Coordinate west       (Coordinate c)   { return {c.x-1, c.y};   }
    inline Coordinate northeast  (Coordinate c)   { return {c.x+1, c.y+1}; }
    inline Coordinate southeast  (Coordinate c)   { return {c.x+1, c.y-1}; }
    inline Coordinate northwest  (Coordinate c)   { return {c.x-1, c.y+1}; }
    inline Coordinate southwest  (Coordinate c)   { return {c.x-1, c.y-1}; }
};

class Board {
public:
    std::array<Piece, 64> position;
    Side sideToPlay;
    
    std::unordered_map<Side, bool> castlingRightsKingSide;
    std::unordered_map<Side, bool> castlingRightsQueenSide;

    std::unordered_map<Side, bool> check;
    
    bool enPassantPossible;
    unsigned short lastDoublePawnPush; // 64 when there was no last double pawn push

private:
    struct {
        std::unordered_map<Side, Coordinate> positions;
        std::unordered_map<Side, std::array<Coordinate, 8>> knightAttacks;
    } kingsData;

public:
    Board();
    Board(std::array<Piece, 64>& position, Side sideToPlay,
    bool whiteCanCastleKingSide, bool whiteCanCastleQueenSide, bool blackCanCastleKingSide, bool blackCanCastleQueenSide,
    bool enPassantPossible, unsigned short lastDoublePawnPush);

    void makeMove(const Move& move);

    void reset();

    void generateMoves(const unsigned short square, std::vector<Move>& moves);

    std::string getPositionString() const;

    bool sideInCheck(const Side& side, const bool includeKnights=true);
    static bool sideInCheck(
        const Side& side,
        const std::array<Piece, 64>& position,
        std::unordered_map<Side, Coordinate>& kingPositions,
        std::unordered_map<Side, std::array<Coordinate, 8>>& knightAttacksAroundKings,
        const bool includeKnights = false
    );

private:
    void generateMovesInDirection(
        const Coordinate& coord,
        const std::function<Coordinate(Coordinate)>& directionFunc,
        std::vector<Move>& moves,
        const Side& opponent
    );

    PieceType getNextOpponentPieceInDirection(
        const Coordinate& coord,
        const std::function<Coordinate(Coordinate)>& directionFunc,
        const Side& opponent
    ) const;

    static PieceType getNextOpponentPieceInDirection(
        const Coordinate& coord,
        const std::array<Piece, 64>& position,
        const std::function<Coordinate(Coordinate)>& directionFunc,
        const Side& opponent
    );

    static void getKnightAttackCoordsAtCoord(const Coordinate& coord, std::array<Coordinate, 8>& attacks);

    bool pawnAttackingOppKingAtCoord(const Coordinate& pawnCoord, const Side& pawnSide);

    static void makeHypotheticalMoveInPosition(
        const std::array<Piece, 64>& oldPosition,
        std::array<Piece, 64>& newPosition,
        const unsigned int before,
        const unsigned int after,
        const Piece piece,
        const bool enPassant = false
    );

    bool getChecksIfMove(
        std::unordered_map<Side, bool>& checks,
        const int before,
        const int after,
        const Piece& piece,
        const bool isKing = false,
        const bool isKnight = false,
        const bool enPassant = false
        );

    // Adds move to moves if it doesn't place the moving side in check. Also adjusts willBeCheck in move.
    void addMoveIfAcceptable(
        std::vector<Move>& moves,
        Move move,
        const Side& opponent,
        const bool isKing = false,
        const bool isKnight = false,
        const bool enPassant = false
    );

    // Adds all promotions if pawn move acceptable
    void addAllPromotionsIfAcceptable(
        std::vector<Move>& moves,
        Move move, // in the form {before, after, capture}
        const Side& opponent
    );


};