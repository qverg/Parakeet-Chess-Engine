#pragma once
#include <unordered_map>
#include <bitset>

using std::unordered_map;

namespace bitboards {
    #define bitboard std::bitset<64>

    unordered_map<bitboard, bitboard> kingMoves;
    unordered_map<bitboard, bitboard> queenMoves;
    unordered_map<bitboard, bitboard> bishopMoves;
    unordered_map<bitboard, bitboard> knightMoves;
    unordered_map<bitboard, bitboard> rookMoves;

    unordered_map<bitboard, bitboard> blackPawnMovesNormal;
    unordered_map<bitboard, bitboard> blackPawnMovesCaptures;
    unordered_map<bitboard, bitboard> whitePawnMovesNormal;
    unordered_map<bitboard, bitboard> whitePawnMovesCaptures;
}

enum Colours {
    BLACK = -1, WHITE = 1
};