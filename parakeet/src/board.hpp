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
    Side sideToPlay = Side::WHITE;

    std::unordered_map<Side, bool> castlingRightsKingSide;
    std::unordered_map<Side, bool> castlingRightsQueenSide;

    bool enPassantPossible;
    unsigned short lastDoublePawnPush; // 64 when there was no last double pawn push

private:


    std::unordered_map<Side, Coordinate> kingPositions;

public:
    Board();
    Board(std::array<Piece, 64>& position, Side sideToPlay,
    bool whiteCanCastleKingSide, bool whiteCanCastleQueenSide, bool blackCanCastleKingSide, bool blackCanCastleQueenSide,
    bool enPassantPossible, unsigned short lastDoublePawnPush);

    void makeMove(const Move& move);

    void reset();

    void generateMoves(const unsigned short square, std::vector<Move>& moves);

    std::string getPositionString() const;
    bool sideInCheck(const Side& side);

private:
    void generateMovesInDirection(
        const Coordinate& coord,
        const std::function<Coordinate(Coordinate)>& directionFunc,
        std::vector<Move>& moves,
        const Side& opponent
    ) const;

    PieceType getNextOpponentPieceInDirection(
        const Coordinate& coord,
        const std::function<Coordinate(Coordinate)>& directionFunc,
        const Side& opponent
    ) const;

};