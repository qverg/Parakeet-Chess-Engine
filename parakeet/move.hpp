
class Move {
    /* see https://www.chessprogramming.org/Encoding_Moves */
public:
    
    unsigned short promotion    : 1;
    unsigned short capture      : 1;
    unsigned short special1     : 1;
    unsigned short special0     : 1;

    unsigned short before       : 6;
    unsigned short after        : 6; // if before and after are both 0, there was no previous move (e.g. beginning of the game)

    Move();
    Move(unsigned short before, unsigned short after, unsigned short promotion, unsigned short capture, unsigned short special1, unsigned short special0);
    Move(unsigned short before, unsigned short after, unsigned short capture);
    Move(unsigned short before, unsigned short after);

    bool legal();

};