#pragma once

class Move {
    /* see https://www.chessprogramming.org/Encoding_Moves */
public:
    
    bool promotion;
    bool capture;
    bool special1;
    bool special0;

    int before;
    int after; // if before and after are both 0, there was no previous move (e.g. beginning of the game)

    bool willBeCheck = false;

    Move();
    Move( int before,  int after, bool promotion, bool capture, 
            bool special1, bool special0);
    Move( int before,  int after, bool promotion, bool capture, 
            bool special1, bool special0, bool willBeCheck);
    Move( int before,  int after, bool capture);
    Move( int before,  int after);

    bool beforeAndAfterDifferent() const;
    bool isCastle() const;
    bool isKingSideCastle() const;
    bool isQueenSideCastle() const;
    bool isEnPassant() const;

};