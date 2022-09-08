#include "engine.hpp"

Engine::Engine() {
    board = Board();
}

float Engine::evaluate() {
    evaluate(board);
}


float Engine::evaluate(Board& board) {
    return 0.0f;
}

void Engine::search() {
    search(board);
}

void Engine::search(Board& initialBoard) {
    
}