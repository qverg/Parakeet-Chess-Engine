#pragma once

#include <array>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>

#include "move.hpp"
#include "types/piece.hpp"
#include "types/coordinate.hpp"
#include "types/piecetype.hpp"

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

    int materialDifference;

private:

    static std::unordered_map<PieceType, int>* pieceValues_ptr;
    static std::array<std::vector<int>, 64> knightAttacksAtSquare;
    static std::array<std::vector<int>, 64> kingMovesAtSquare;

    struct {
        std::unordered_map<Side, int> positions;
        std::unordered_map<Side, std::vector<int>*> knightAttacks;
    } kingsData;

public:
    Board();
    Board(std::array<Piece, 64>& position, Side sideToPlay,
    bool whiteCanCastleKingSide, bool whiteCanCastleQueenSide, bool blackCanCastleKingSide, bool blackCanCastleQueenSide,
    bool enPassantPossible, unsigned short lastDoublePawnPush, int materialDifference=0);

    void setPieceValues(std::unordered_map<PieceType, int>& pieceValues);

    void makeMove(const Move& move);

    void reset();

    void generateMoves(const unsigned short square, std::vector<Move>& moves) const;
    void generateAllMoves(std::vector<Move>& moves) const;

    std::string getPositionString() const;

    bool sideInCheck(const Side& side, const bool includeKnights=true) const;
    bool sideInCheck(
        const Side& side,
        const std::array<Piece, 64>& position,
        const std::unordered_map<Side, int>& kingPositions,
        const std::unordered_map<Side, std::vector<int>*>& knightAttacksAroundKings,
        const bool includeKnights = false
    ) const;

    static void fillKnightAttacksArray();
    static void fillKingMovesArray();

private:
    void generateMovesInDirection(
        const Coordinate& coord,
        const int& square,
        const std::function<Coordinate(Coordinate)>& directionFunc,
        std::vector<Move>& moves,
        const Side& opponent
    ) const;

    PieceType getNextOpponentPieceInDirection(
        const Coordinate& coord,
        const std::function<Coordinate(Coordinate)>& directionFunc,
        const Side& opponent
    ) const;

    PieceType getNextOpponentPieceInDirection(
        const Coordinate& coord,
        const std::array<Piece, 64>& position,
        const std::function<Coordinate(Coordinate)>& directionFunc,
        const Side& opponent
    ) const;


    void makeHypotheticalMoveInPosition(
        const std::array<Piece, 64>& oldPosition,
        std::array<Piece, 64>& newPosition,
        const unsigned int before,
        const unsigned int after,
        const Piece piece,
        const bool enPassant = false
    ) const;

    void getChecksIfMove(
        std::unordered_map<Side, bool>& checks,
        const int before,
        const int after,
        const Piece& piece,
        const bool isKing = false,
        const bool isKnight = false,
        const bool enPassant = false
        ) const;

    // Adds move to moves if it doesn't place the moving side in check. Also adjusts willBeCheck in move.
    void addMoveIfAcceptable(
        std::vector<Move>& moves,
        Move move,
        const Side& opponent,
        const bool isKing = false,
        const bool isKnight = false,
        const bool enPassant = false
    ) const;

    // Adds all promotions if pawn move acceptable
    void addAllPromotionsIfAcceptable(
        std::vector<Move>& moves,
        Move move, // in the form {before, after, capture}
        const Side& opponent
    ) const;
};