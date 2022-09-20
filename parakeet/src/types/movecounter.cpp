#include "movecounter.hpp"
#include <iostream>

void MoveCounter::operator+=(const MoveCounter& counter) {
    moves += counter.moves;
    captures += counter.captures;
    enPassant += counter.enPassant;
    castles += counter.castles;
    promotions += counter.promotions;
    checks += counter.checks;
}

void MoveCounter::print(const int depth) const {
    std::cout << "Depth " << depth << std::endl;
    std::cout << "Moves:     " << moves << std::endl;
    std::cout << "Captures:  " << captures << std::endl;
    std::cout << "En passant:" << enPassant << std::endl;
    std::cout << "Castles:   " << castles << std::endl;
    std::cout << "Promotions:" << promotions << std::endl;
    std::cout << "Checks:    " << checks << std::endl;
    std::cout << std::endl;
}