#include "utility.hpp"

static void run() {
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

    /* Commands:
     * $quit            quit parakeet
     * $reset           reset board to normal starting position
     * $testmovegen     test move generation
     * $exitboard       exit the current board
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
                std::cout << "Enter a FEN or $reset\n> ";
            } break;
            
            case RUNNING: {
                std::cout << "Enter a move or command\n> ";
                getline(std::cin, in);

                if (in[0] == '$') { // commands
                    if (in == "$reset") {
                        board.reset();
                    } else if (in == "$quit") {
                        quit = true;
                    } else if (in == "$testmovegen") {
                        mode = TEST_MOVE_GEN;
                    } else if (in == "$exitboard") {
                        mode = BEGIN;
                    }
                    
                } else {    // move given
                    unsigned short before = stoi(in);

                    //Log(LogLevel::INFO, "Generating moves");
                    std::vector<Move> possibleMoves;
                    board.generateMoves(before, possibleMoves);

                    std::cout << "> ";
                    getline(std::cin, in);

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
                std::cout << "Enter a square (e.g. e4) or command\n> ";
                getline(std::cin, in);

                if (in[0] == '$') { // commands
                    if (in == "$reset") {
                        board.reset();
                    } else if (in == "$quit") {
                        quit = true;
                    } else if (in == "$exitboard") {
                        mode = BEGIN;
                    }
                } else {    
                    unsigned int file = in[0] - 'a';
                    unsigned int rank = in[1] - '1';
                    Log(LogLevel::DEBUG, "Generating on the following square:");
                    std::cout << rank*8+file << std::endl;
                    
                    std::vector<Move> moves;
                    board.generateMoves(rank*8 + file, moves);
                    
                    for (Move& move : moves) {
                        std::cout << move.after << " " << board.algebraic(move) << " ";
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