#include "main.hpp"

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
    Engine engine;
    std::unordered_map<unsigned short, std::vector<Move>> generatedMoves;

    enum Mode {
        BEGIN, RUNNING, TEST_MOVE_GEN
    };

    Mode mode = BEGIN;
    bool quit = false;

    std::string in;

    
    while (!quit) {
        switch (mode) {
            case BEGIN: {
                std::cout << "$Enter a FEN or $reset\n> ";

                getline(std::cin, in);

                if (in == "$reset") {
                    engine.board.reset();
                    mode = RUNNING;
                } else if (in == "$testmovegen") {
                    mode = TEST_MOVE_GEN;
                } else if (in == "$quit") {
                    quit = true;  
                } else {
                    loadFEN(in, engine.board);
                    mode = RUNNING;
                }
            } break;
            
            case RUNNING: {
                std::cout << "$Enter a move or command\n> ";
                getline(std::cin, in);

                if (in[0] == '$') { // commands
                    if (in == "$reset") {
                        engine.board.reset();
                    } else if (in == "$quit") {
                        quit = true;
                    } else if (in == "$testmovegen") {
                        mode = TEST_MOVE_GEN;
                    } else if (in == "$exitboard") {
                        mode = BEGIN;
                    } else if (in == "$getposition") {
                        std::cout << getPositionString(engine.board) << std::endl;
                    }
                    
                } else {    // move given
                    unsigned short before = stoi(in);

                    //Log(LogLevel::INFO, "Generating moves");
                    std::vector<Move> possibleMoves;
                    if (generatedMoves.find(before) == generatedMoves.end()) {
                        engine.board.generateMoves(before, possibleMoves);
                        generatedMoves[before] = possibleMoves;
                    } else {
                        possibleMoves = generatedMoves[before];
                    }

                    for (const auto& move : possibleMoves) {
                        std::cout << move.after << " ";
                    }
                    std::cout << std::endl;

                    std::cout << "Enter a target square" << std::endl;
                    getline(std::cin, in);

                    unsigned int after = stoi(in);

                    Move queriedMove;
                    for (const Move& move : possibleMoves) {
                        if (move.after == after) {
                            queriedMove = move;
                            break;
                        }
                    }

                    if (queriedMove.isLegal()) {
                        engine.board.makeMove(queriedMove);
                        generatedMoves.clear();
                        if (engine.board.sideInCheck(Side::WHITE)) std::cout << "CHECK white" << std::endl;
                        if (engine.board.sideInCheck(Side::BLACK)) std::cout << "CHECK black" << std::endl;
                        
                    } else {
                        Log(LogLevel::INFO, "Invalid move");
                    }

                }

            } break;

            case (TEST_MOVE_GEN): {
                std::cout << "$Enter a square (e.g. e4) or command\n> ";
                getline(std::cin, in);

                if (in[0] == '$') { // commands
                    if (in == "$reset") {
                        engine.board.reset();
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
                    engine.board.generateMoves(rank*8 + file, moves);
                    
                    for (Move& move : moves) {
                        std::cout << move.after << " " << algebraic(move, engine.board.position) << " ";
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