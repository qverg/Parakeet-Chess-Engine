// a square is an unsigned short (only needs 6 bits)

#include "parakeet_main.hpp"

int main() {
    Board board;
    std::cout << generateMoves(0, board)[0].after << std::endl; // <-- TEST LINE!
}