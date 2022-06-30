#pragma once
#include <iostream>
#include <vector>
#include <array>

// constants
enum sides {
    WHITE = 0, BLACK = 1
};

enum pieceTypes {
    EMPTY, KING, QUEEN, BISHOP, KNIGHT, ROOK, PAWN
};

// types
struct Piece {
    unsigned int type : 3;
    unsigned int side : 1;
};

class Move {
    /* see https://www.chessprogramming.org/Encoding_Moves */
public:
    unsigned int before : 6;
    unsigned int after : 6;     // if before and after are both 0, there was no previous move (e.g. beginning of the game)
    unsigned int promotion : 1;
    unsigned int capture : 1;
    unsigned int special1 : 1;
    unsigned int special2 : 1;

    Move() {
        before      = 0;
        after       = 0;
        promotion   = 0;
        capture     = 0;
        special1    = 0;
        special2    = 0;
    }

    Move(unsigned int before, unsigned int after, unsigned int promotion, unsigned int capture, unsigned int special1, unsigned int special2)
        : before(before), after(after), promotion(promotion), capture(capture), special1(special1), special2(special2)
    {
    }

    Move(unsigned int before, unsigned int after, unsigned int capture)
        : before(before), after(after), capture(capture)
    {
        promotion   = 0;
        special1    = 0;
        special2    = 0;
    }

    Move(unsigned int before, unsigned int after)
        : before(before), after(after)
    {
        promotion   = 0;
        capture     = 0;
        special1    = 0;
        special2    = 0;
    }
};

class Board {
public:
    std::array<Piece, 64> position;
    Move lastMove;

    bool whiteCanCastle;
    bool blackCanCastle;
    bool enPassantPossible;
    unsigned int sideToPlay : 1;

    Board() {
        whiteCanCastle = true;
        blackCanCastle = true;
        enPassantPossible = false;
        sideToPlay = WHITE;
        std::cout << "Board constructed" << std::endl;
    }
};

// function declarations
std::vector<Move> generateMove(const unsigned char square, const Board& board);