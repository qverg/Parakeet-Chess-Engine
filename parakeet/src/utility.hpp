#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cctype>

#include "types.hpp"
#include "log.hpp"
#include "board.hpp"

static std::vector<std::string> split(std::string& text, const char& delimiter);
static void logFENPosition(std::array<Piece, 64>& position);
void loadFEN(std::string fen, Board& board);
std::string getPositionString(Board& board);