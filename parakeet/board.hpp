#pragma once

#include <array>
#include <unordered_map>
#include <functional>

#include "parakeet_main.hpp"
#include "move.hpp"

class Board {
public:
    std::array<Piece, 64> position;
    Side sideToPlay = Side::WHITE;

    std::unordered_map<Side, bool> castlingRights;

    bool enPassantPossible;
    unsigned short lastDoublePawnPush; // 64 when there was no last double pawn push

    struct Coordinate {
        int x; int y;
    };

    struct dirs {
        static Coordinate north(Coordinate c)       { return {c.x, c.y+1}; }
        static Coordinate south(Coordinate c)       { return {c.x, c.y+1}; }
        static Coordinate east(Coordinate c)        { return {c.x, c.y+1}; }
        static Coordinate west(Coordinate c)        { return {c.x, c.y+1}; }
        static Coordinate northeast(Coordinate c)   { return {c.x, c.y+1}; }
        static Coordinate southeast(Coordinate c)   { return {c.x, c.y+1}; }
        static Coordinate northwest(Coordinate c)   { return {c.x, c.y+1}; }
        static Coordinate southwest(Coordinate c)   { return {c.x, c.y+1}; }
    };

    Board();
    Board(std::array<Piece, 64>& position, Side sideToPlay, bool whiteCanCastle, bool blackCanCastle, bool enPassantPossible, unsigned short lastDoublePawnPush);

    void makeMove(const Move& move);

    void reset();

    void generateMoves(const unsigned short square, std::vector<Move>& moves);

private:
    void generateMovesInDirection(
        const Coordinate coord,
        const std::function<Coordinate(Coordinate)>& directionFunc,
        std::vector<Move>& moves,
        const Side playingAs,
        const Side opponent
    );
};