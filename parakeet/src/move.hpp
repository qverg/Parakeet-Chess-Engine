#pragma once

class Move {
    /* see https://www.chessprogramming.org/Encoding_Moves */
public:
    
    int promotion    : 1;
    int capture      : 1;
    int special1     : 1;
    int special0     : 1;

    int before;
    int after; // if before and after are both 0, there was no previous move (e.g. beginning of the game)

    bool willBeCheck = false;

    Move();
    Move(int before, int after, int promotion, int capture, 
            int special1, int special0);
    Move(int before, int after, int promotion, int capture, 
            int special1, int special0, bool willBeCheck);
    Move(int before, int after, int capture);
    Move(int before, int after);

    bool beforeAndAfterDifferent() const;
    bool isCastle() const;
    bool isEnPassant() const;

};