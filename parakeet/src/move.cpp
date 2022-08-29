#include "move.hpp"

Move::Move() {
    before      = 0;
    after       = 0;
    promotion   = 0;
    capture     = 0;
    special1    = 0;
    special0    = 0;

    willBeCheck = false;
}

Move::Move(unsigned short before, unsigned short after, unsigned short promotion, unsigned short capture, 
            unsigned short special1, unsigned short special0, bool willBeCheck)
    : before(before), after(after), promotion(promotion), capture(capture), special1(special1), special0(special0), willBeCheck(willBeCheck)
{
}

Move::Move(unsigned short before, unsigned short after, unsigned short capture, bool willBeCheck)
    : before(before), after(after), capture(capture), willBeCheck(willBeCheck)
{
    promotion   = 0;
    special1    = 0;
    special0    = 0;
}

Move::Move(unsigned short before, unsigned short after, bool willBeCheck)
    : before(before), after(after), willBeCheck(willBeCheck)
{
    promotion   = 0;
    capture     = 0;
    special1    = 0;
    special0    = 0;

    willBeCheck = false;
}

bool Move::isLegal() { return (before != after);} // This is the case when constructed
                                                // and is a sure way to tell if illegal