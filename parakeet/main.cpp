// a square is an unsigned short (only needs 6 bits)

#include <iostream>
#include <string>
#include <vector>

#include "types.hpp"
#include "log.hpp"
#include "board.hpp"

void generateMoves(const unsigned short square, const Board& board, std::vector<Move>& moves);

void run() {
    /* This function communicates with the user to control the engine.
     * All user commands begin with a dollar sign $
     * 
     * In BEGIN mode, the user can enter a position or issue the command $reset, which wil set up the board from the beginning and enter RUNNING mode.
     * In RUNNING mode, the user enters moves and receives the engine's output.
     * Moves are given as follows:
     * > [number of the origin square]
     * > [number of the destination square]
     * for example,
     * ************************
     * Enter a move or command
     * > 12
     * > 28
     * [engine output]
     * ************************
     * will move whatever is on e2 to e4.
     * 
     */

    Board board;

    enum Mode {
        BEGIN, RUNNING, TEST_MOVE_GEN
    };

    Mode mode = BEGIN;
    bool quit = false;

    std::string in;

    
    while (!quit) {
        switch (mode) {
            case BEGIN: {
                std::cout << "Enter a position or $reset\n> ";
                std::cin >> in;

                if (in == "$reset") {
                    board.reset();
                    mode = RUNNING;
                } else if (in == "$testmovegen") {
                    mode = TEST_MOVE_GEN;
                } else if (in == "quit") {
                    quit = true;  
                } else {
                    std::cout << "Taking positions not yet implemented" << std::endl;
                }


            } break;
            
            case RUNNING: {
                std::cout << "Enter a move or command\n> ";
                std::cin >> in;

                if (in[0] == '$') { // commands
                    if (in == "$reset") {
                        board.reset();
                    } else if (in == "$quit") {
                        quit = true;
                    } else if (in == "$testmovegen") {
                        mode = TEST_MOVE_GEN;
                    }
                    
                } else {    // move given
                    unsigned short before = stoi(in);

                    //Log(LogLevel::INFO, "Generating moves");
                    std::vector<Move> possibleMoves;
                    board.generateMoves(before, possibleMoves);

                    std::cout << "> ";
                    std::cin >> in;

                    unsigned int after = stoi(in);

                    Move queriedMove;
                    for (Move& move : possibleMoves) {
                        if (move.after == after) {
                            queriedMove = move;
                            break;
                        }
                    }

                    if (queriedMove.legal()) {
                        board.makeMove(queriedMove);
                        // evaluation here?
                    }

                }

            } break;

            case (TEST_MOVE_GEN): {
                std::cout << "Enter a square or command\n> ";
                std::cin >> in;

                if (in[0] == '$') { // commands
                    if (in == "$reset") {
                        board.reset();
                    } else if (in == "$quit") {
                        quit = true;
                    } else if (in == "$exit") {
                        mode = BEGIN;
                    }
                } else {
                    unsigned int origin = stoi(in);
                    std::vector<Move> moves;
                    board.generateMoves(origin, moves);

                    for (Move& move : moves) {
                        std::cout << board.algebraic(move) << " ";
                    }
                    std::cout << std::endl;
                }

            } break;
        }
    }
    
}

int main() {
    run();
}