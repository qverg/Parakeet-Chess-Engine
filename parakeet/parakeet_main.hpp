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
    unsigned short type : 3;
    unsigned short side : 1;
};

const Piece EMPTY_SQUARE = {EMPTY, WHITE};

class Move {
    /* see https://www.chessprogramming.org/Encoding_Moves */
public:
    unsigned short before : 6;
    unsigned short after : 6;     // if before and after are both 0, there was no previous move (e.g. beginning of the game)
    unsigned short promotion : 1;
    unsigned short capture : 1;
    unsigned short special1 : 1;
    unsigned short special0 : 1;

    Move() {
        before      = 0;
        after       = 0;
        promotion   = 0;
        capture     = 0;
        special1    = 0;
        special0    = 0;
    }

    Move(unsigned short before, unsigned short after, unsigned short promotion, unsigned short capture, unsigned short special1, unsigned short special0)
        : before(before), after(after), promotion(promotion), capture(capture), special1(special1), special0(special0)
    {
    }

    Move(unsigned short before, unsigned short after, unsigned short capture)
        : before(before), after(after), capture(capture)
    {
        promotion   = 0;
        special1    = 0;
        special0    = 0;
    }

    Move(unsigned short before, unsigned short after)
        : before(before), after(after)
    {
        promotion   = 0;
        capture     = 0;
        special1    = 0;
        special0    = 0;
    }
};

class Board {
public:
    std::array<Piece, 64> position;
    Move lastMove;

    bool castlingRights[2];
    bool enPassantPossible;
    unsigned short sideToPlay : 1;

    Board() {
        castlingRights[WHITE] = true; castlingRights[BLACK] = true;
        enPassantPossible = false;
        sideToPlay = WHITE;
        std::cout << "Board constructed" << std::endl;
    }

    Board(std::array<Piece, 64>& position, bool whiteCanCastle, bool blackCanCastle, bool enPassantPossible, unsigned short sideToPlay)
        : position(position), enPassantPossible(enPassantPossible), sideToPlay(sideToPlay)
    {
        castlingRights[WHITE] = whiteCanCastle;
        castlingRights[BLACK] = blackCanCastle;
    }

    void makeMove(const Move& move) {
        Piece piece = position[move.before];

        position[move.after] = piece;
        position[move.before] = EMPTY_SQUARE;

        if (enPassantPossible) enPassantPossible = false;

        if (move.promotion) {
            if (move.special1 && move.special0) { // queen-promotion
                position[move.after].type = QUEEN;
            } else if (move.special1 && !move.special0) { // rook-promotion
                position[move.after].type = ROOK;
            } else if (!move.special1 && move.special0) { // bishop-promotion
                position[move.after].type = BISHOP;
            } else { // knight-promotion
                position[move.after].type = KNIGHT;
            }
        } else if (move.capture) {
            if (move.special0) { // en passant
                if (piece.side == WHITE) {
                    position[move.after-8] = EMPTY_SQUARE;
                } else {
                    position[move.after+8] = EMPTY_SQUARE;
                }
            } 
        } else {
            if (!move.special1 && move.special0) { // double pawn push
                enPassantPossible = true;
            } else if (move.special1 && !move.special0) { // king-side castle
                position[move.after-1] = {ROOK, piece.side};
                position[move.after+1] = EMPTY_SQUARE;
            } else if (move.special1 && move.special0) { // queen-side castle
                position[move.after+1] = {ROOK, piece.side};
                position[move.after-2] = EMPTY_SQUARE;
            } 
        }

        if (piece.type == KING || piece.type == ROOK) {
            castlingRights[piece.side] = false;
        }
    }

    void reset() {
        position[0] = {ROOK, WHITE};    position[7] = {ROOK, WHITE};
        position[1] = {KNIGHT, WHITE};  position[6] = {KNIGHT, WHITE};
        position[2] = {BISHOP, WHITE};  position[5] = {BISHOP, WHITE};
        position[3] = {QUEEN, WHITE};   position[4] = {KING, WHITE};

        position[56] = {ROOK, BLACK};   position[63] = {ROOK, BLACK};
        position[57] = {KNIGHT, BLACK}; position[62] = {KNIGHT, BLACK};
        position[58] = {BISHOP, BLACK}; position[61] = {BISHOP, BLACK};
        position[59] = {QUEEN, BLACK};  position[60] = {KING, BLACK};

        for (int i = 0; i < 8; i++) {
            position[i+8]   = {PAWN, WHITE};
            position[i+48]  = {PAWN, WHITE};
        }

        for (int i = 16; i < 56; i++) {
            position[i] = EMPTY_SQUARE;
        }
    }
};

// function declarations
std::vector<Move> generateMoves(const unsigned short square, const Board& board);