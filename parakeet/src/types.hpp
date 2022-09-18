#pragma once

// constants
enum class Side {
    WHITE = 0, BLACK = 1, EMPTY
};

enum class PieceType {
    EMPTY, KING, QUEEN, BISHOP, KNIGHT, ROOK, PAWN
};

// types
struct Piece {
    PieceType type;
    Side side;
};

const Piece EMPTY_SQUARE = {PieceType::EMPTY, Side::EMPTY};
