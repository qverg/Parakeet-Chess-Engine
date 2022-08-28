#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "types.hpp"
#include "log.hpp"
#include "board.hpp"

static std::vector<std::string> split(std::string& text, const char& delimiter);
static void logFENPosition(std::array<Piece, 64>& position);
void loadFEN(std::string fen, Board& board);
std::string getPositionString(Board& board);
std::string algebraic(const Move& move, const std::array<Piece, 64>& position);