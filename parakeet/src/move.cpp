#include "move.hpp"

Move::Move() {
    before      = 0;
    after       = 0;
    promotion   = 0;
    capture     = 0;
    special1    = 0;
    special0    = 0;
}

Move::Move(unsigned short before, unsigned short after, unsigned short promotion, unsigned short capture, unsigned short special1, unsigned short special0)
    : before(before), after(after), promotion(promotion), capture(capture), special1(special1), special0(special0)
{
}

Move::Move(unsigned short before, unsigned short after, unsigned short capture)
    : before(before), after(after), capture(capture)
{
    promotion   = 0;
    special1    = 0;
    special0    = 0;
}

Move::Move(unsigned short before, unsigned short after)
    : before(before), after(after)
{
    promotion   = 0;
    capture     = 0;
    special1    = 0;
    special0    = 0;
}

bool Move::isLegal() { return (before != after);} // This is the case when constructed
                                                // and is a sure way to tell if illegal