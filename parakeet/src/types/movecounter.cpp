#include "movecounter.hpp"

void MoveCounter::operator+=(const MoveCounter& counter) {
    moves += counter.moves;
    captures += counter.captures;
    enPassant += counter.enPassant;
    castles += counter.castles;
    promotions += counter.promotions;
    checks += counter.checks;
}