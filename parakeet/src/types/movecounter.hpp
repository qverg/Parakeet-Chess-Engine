#pragma once

struct MoveCounter {
    int moves = 0;
    int captures = 0;
    int enPassant = 0; 
    int castles = 0;
    int promotions = 0;
    int checks = 0;

    void operator+=(const MoveCounter& counter);
};