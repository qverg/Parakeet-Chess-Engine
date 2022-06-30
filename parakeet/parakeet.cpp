// a square is an unsigned char

#include "parakeet_main.hpp"

int main() {
    Board board;
    std::cout << generateMove(0, board)[0].after << std::endl;
}