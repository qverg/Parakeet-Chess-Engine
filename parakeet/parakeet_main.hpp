#pragma once
#include <iostream>
#include <vector>
#include <array>
#include <unordered_map>

#define LOG(x) std::cout << x << std::endl

// constants
enum class Side {
    WHITE = 0, BLACK = 1
};

enum class PieceType {
    EMPTY, KING, QUEEN, BISHOP, KNIGHT, ROOK, PAWN
};

// types
struct Piece {
    PieceType type;
    Side side;
};

const Piece EMPTY_SQUARE = {PieceType::EMPTY, Side::WHITE};

class Move {
    /* see https://www.chessprogramming.org/Encoding_Moves */
public:
    
    unsigned short promotion : 1;
    unsigned short capture : 1;
    unsigned short special1 : 1;
    unsigned short special0 : 1;

    unsigned short before : 6;
    unsigned short after : 6;     // if before and after are both 0, there was no previous move (e.g. beginning of the game)

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

    std::unordered_map<Side, bool> castlingRights;
    bool enPassantPossible;
    Side sideToPlay = Side::WHITE;

    Board() {
        castlingRights[Side::WHITE] = true; castlingRights[Side::BLACK] = true;
        enPassantPossible = false;
        sideToPlay = Side::WHITE;
        std::cout << "Board constructed" << std::endl;
    }

    Board(std::array<Piece, 64>& position, bool whiteCanCastle, bool blackCanCastle, bool enPassantPossible, Side sideToPlay)
        : position(position), enPassantPossible(enPassantPossible), sideToPlay(sideToPlay)
    {
        castlingRights[Side::WHITE] = whiteCanCastle;
        castlingRights[Side::BLACK] = blackCanCastle;
    }

    void makeMove(const Move& move) {
        Piece piece = position[move.before];

        position[move.after] = piece;
        position[move.before] = EMPTY_SQUARE;

        if (enPassantPossible) enPassantPossible = false;

        if (move.promotion) {
            if (move.special1 && move.special0) { // queen-promotion
                position[move.after].type = PieceType::QUEEN;
            } else if (move.special1 && !move.special0) { // rook-promotion
                position[move.after].type = PieceType::ROOK;
            } else if (!move.special1 && move.special0) { // bishop-promotion
                position[move.after].type = PieceType::BISHOP;
            } else { // knight-promotion
                position[move.after].type = PieceType::KNIGHT;
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
            } else if (move.special1 && !move.special0) { // king-side castle
                position[move.after-1] = {PieceType::ROOK, piece.side};
                position[move.after+1] = EMPTY_SQUARE;
            } else if (move.special1 && move.special0) { // queen-side castle
                position[move.after+1] = {PieceType::ROOK, piece.side};
                position[move.after-2] = EMPTY_SQUARE;
            } 
        }

        if (piece.type == PieceType::KING || move.before == 0 || move.before == 7) {
            castlingRights[piece.side] = false;
        }
    }

    void reset() {

        for (int i = 0; i < 64; i++) {
            position[i] = EMPTY_SQUARE;
        }

        position[0] = {PieceType::ROOK, Side::WHITE};    position[7] = {PieceType::ROOK, Side::WHITE};
        position[1] = {PieceType::KNIGHT, Side::WHITE};  position[6] = {PieceType::KNIGHT, Side::WHITE};
        position[2] = {PieceType::BISHOP, Side::WHITE};  position[5] = {PieceType::BISHOP, Side::WHITE};
        position[3] = {PieceType::QUEEN, Side::WHITE};   position[28] = {PieceType::KING, Side::WHITE};

        position[56] = {PieceType::ROOK, Side::BLACK};   position[63] = {PieceType::ROOK, Side::BLACK};
        position[57] = {PieceType::KNIGHT, Side::BLACK}; position[62] = {PieceType::KNIGHT, Side::BLACK};
        position[58] = {PieceType::BISHOP, Side::BLACK}; position[61] = {PieceType::BISHOP, Side::BLACK};
        position[59] = {PieceType::QUEEN, Side::BLACK};  position[60] = {PieceType::KING, Side::BLACK};
        /*
        for (int i = 0; i < 8; i++) {
            position[i+8]   = {PAWN, WHITE};
            position[i+48]  = {PAWN, WHITE};
        }
        */
       castlingRights[Side::WHITE] = true;
       castlingRights[Side::BLACK] = true;
       enPassantPossible = true;
       sideToPlay = Side::WHITE;
    }
};

// function declarations
std::vector<Move> generateMoves(const unsigned short square, const Board& board);