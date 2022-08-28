#include "utility.hpp"

static std::vector<std::string> split(std::string& text, const char& delimiter) {
    std::vector<std::string> words;

    std::string current_word = "";
    for (const char& c : text) {
        if (c == delimiter) {
            if (current_word != "") {
                words.push_back(current_word);
                current_word = "";
            }
        } else {
            current_word += c;
        }
    }

    if (current_word != "") {
        words.push_back(current_word);
        current_word = "";
    }

    return words;
}

static void logFENPosition(std::array<Piece, 64>& position) {

    std::string out = "\n";
    for (int i = 56; i >= 0; i++) {
        if (position[i].side == Side::WHITE) {
            if (position[i].type == PieceType::PAWN) out += "P";
            else if (position[i].type == PieceType::ROOK) out += "R";
            else if (position[i].type == PieceType::KNIGHT) out += "N";
            else if (position[i].type == PieceType::BISHOP) out += "B";
            else if (position[i].type == PieceType::QUEEN) out += "Q";
            else if (position[i].type == PieceType::KING) out += "K";
        } else {
            if (position[i].type == PieceType::PAWN) out += "p";
            else if (position[i].type == PieceType::ROOK) out += "r";
            else if (position[i].type == PieceType::KNIGHT) out += "n";
            else if (position[i].type == PieceType::BISHOP) out += "b";
            else if (position[i].type == PieceType::QUEEN) out += "q";
            else if (position[i].type == PieceType::KING) out += "k";
            else if (position[i].type == PieceType::EMPTY) out += ".";
        } 

        if ((i+1)%8 == 0) {
            out += "\n";
            i -= 16;
        }
    }

    Log(LogLevel::DEBUG, out);
}

void loadFEN(std::string fen, Board& board) {
    std::unordered_map<char, Piece> pieceRef;
    pieceRef['p'] = {PieceType::PAWN,   Side::BLACK};
    pieceRef['r'] = {PieceType::ROOK,   Side::BLACK};
    pieceRef['n'] = {PieceType::KNIGHT, Side::BLACK};
    pieceRef['b'] = {PieceType::BISHOP, Side::BLACK};
    pieceRef['q'] = {PieceType::QUEEN,  Side::BLACK};
    pieceRef['k'] = {PieceType::KING,   Side::BLACK};
    pieceRef['R'] = {PieceType::ROOK,   Side::WHITE};
    pieceRef['P'] = {PieceType::PAWN,   Side::WHITE};
    pieceRef['N'] = {PieceType::KNIGHT, Side::WHITE};
    pieceRef['B'] = {PieceType::BISHOP, Side::WHITE};
    pieceRef['Q'] = {PieceType::QUEEN,  Side::WHITE};
    pieceRef['K'] = {PieceType::KING,   Side::WHITE};

    // 0 = piece placement
    // 1 = active colour
    // 2 = castling rights
    // 3 = possible en passant targets
    // 4 = halfmove clock
    // 5 = fullmove number
    // (separated by spaces)

    std::array<Piece, 64> position;
    for (int i = 0; i < 64; i++) {
        position[i] = EMPTY_SQUARE;
    }

    std::vector<std::string> info = split(fen, ' ');
    

    // Piece placement
    std::vector<std::string> piece_placement = split(info[0], '/');

    for (int rank = 7; rank >= 0; rank--) {
        int file = 0;
        for (const char& c : piece_placement[7-rank]) {
            if (isdigit(c)) file += c-'0';
            else {
                position[rank*8 + file] = pieceRef[c];
                file++;
            }
        }
    }
    Log(LogLevel::DEBUG, "Generated FEN Position:");
    logFENPosition(position);

    // Active colour
    Side active_colour;
    if (info[1] == "w") {
        active_colour = Side::WHITE;
        Log(LogLevel::INFO, "FEN reader says active colour white");
    } else {
        active_colour = Side::BLACK;
        Log(LogLevel::INFO, "FEN reader says active colour black");
    }

    // Castling rights
    bool whiteCanCastleKingSide = false;
    bool whiteCanCastleQueenSide = false;
    bool blackCanCastleKingSide = false;
    bool blackCanCastleQueenSide = false;

    for (const char& c : info[2]) {
        if (c == 'K') whiteCanCastleKingSide = true;
        else if (c == 'Q') whiteCanCastleQueenSide = true;
        else if (c == 'k') blackCanCastleKingSide = true;
        else if (c == 'q') blackCanCastleQueenSide = true;
    }

    // Possible En Passant Targets
    // https://www.chess.com/terms/fen-chess
    bool enPassantPossible = false;
    unsigned short lastDoublePawnPush = 64;

    if (info[3] != "-") {
        int possibleEnPassantFile = info[3][0] - 'a';

        int enPassantRank;
        int offset;
        if (active_colour == Side::WHITE) {
            enPassantRank = 4;
            offset = -1;
        } else {
            enPassantRank = 3;
            offset = 1;
        }

        lastDoublePawnPush = enPassantRank*8 + possibleEnPassantFile;
        if (possibleEnPassantFile > 0 && position[lastDoublePawnPush-1].side == active_colour
                && position[lastDoublePawnPush-1].type == PieceType::PAWN) {
            enPassantPossible = true;
            Log(LogLevel::INFO, "FEN reader says en passant possible");
        }

        if (possibleEnPassantFile < 7 && position[lastDoublePawnPush+1].side == active_colour
                && position[lastDoublePawnPush+1].type == PieceType::PAWN) {
            enPassantPossible = true;
            Log(LogLevel::INFO, "FEN reader says en passant possible");
        }

    }

    // Halfmove clock


    // Fullmove number


    // Generate & return board (pointer?)
    board = Board(
        position, active_colour,
        whiteCanCastleKingSide, whiteCanCastleQueenSide,
        blackCanCastleKingSide, blackCanCastleQueenSide,
        enPassantPossible, lastDoublePawnPush
    );
}

std::string getPositionString(Board& board) {
    std::string symbols = ".kqbnrp";
    std::string out = "";
    for (const auto& piece : board.position) {
        if (piece.type == PieceType::EMPTY) {
            out += ".";
        } else {
            char offset = (piece.side == Side::WHITE) ? 'A'-'a' : 0;
            out += symbols[(int)piece.type] + offset;

        }
    }
    return out;
}

std::string algebraic(const Move& move, const std::array<Piece, 64>& position) {
    std::string out = "";
    switch (position[move.before].type) {
        case (PieceType::KING):     out += "K"; break;
        case (PieceType::QUEEN):    out += "Q"; break;
        case (PieceType::BISHOP):   out += "B"; break;
        case (PieceType::KNIGHT):   out += "N"; break;
        case (PieceType::ROOK):     out += "R"; break;
    }

    out += (char) (move.after%8) + 'a';
    out += std::to_string(move.after/8 + 1);

    return out;
}