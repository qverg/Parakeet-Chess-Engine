#include "move.hpp"

Move::Move() {
    before      = 0;
    after       = 0;
    promotion   = 0;
    capture     = 0;
    special1    = 0;
    special0    = 0;
}

Move::Move(int before, int after, int promotion, int capture, 
            int special1, int special0)
    : before(before), after(after), promotion(promotion), capture(capture), special1(special1), special0(special0)
{
}

Move::Move(int before, int after, int promotion, int capture, 
            int special1, int special0, bool willBeCheck)
    : before(before), after(after), promotion(promotion), capture(capture), special1(special1), special0(special0), willBeCheck(willBeCheck)
{
}

Move::Move(int before, int after, int capture)
    : before(before), after(after), capture(capture)
{
    promotion   = 0;
    special1    = 0;
    special0    = 0;
}

Move::Move(int before, int after)
    : before(before), after(after)
{
    promotion   = 0;
    capture     = 0;
    special1    = 0;
    special0    = 0;
}

bool Move::beforeAndAfterDifferent() { return (before != after);} // This is the case when constructed
                                                // and is a sure way to tell if illegal