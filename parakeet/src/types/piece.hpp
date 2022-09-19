#pragma once

#include "piecetype.hpp"
#include "side.hpp"

struct Piece {
    PieceType type;
    Side side;
};

const Piece EMPTY_SQUARE = {PieceType::EMPTY, Side::EMPTY};