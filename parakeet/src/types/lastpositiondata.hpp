#pragma once

#include "piece.hpp"
#include "../move.hpp"

#include <unordered_map>

struct LastPositionData {
    Move move;
    Side whoseTurnItWas;
    Piece pieceCaptured; // can be empty
    int materialDifference;
    bool 
        whiteCanCastleKingSide,
        blackCanCastleKingSide,
        whiteCanCastleQueenSide,
        blackCanCastleQueenSide,

        whiteInCheck,
        blackInCheck;
};