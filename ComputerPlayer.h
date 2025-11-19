#pragma once
#include "Board.h"
#include "Cell.h"
#include <optional>
#include <tuple>
#include <vector>
#include <random>

// try to make SOS, or block, or random move
class ComputerPlayer {
public:
    std::optional<std::tuple<int, int, Cell>> chooseMove(const Board& board);

private:
    std::optional<std::tuple<int, int, Cell>> findMakingMove(const Board& b);
    std::optional<std::tuple<int, int, Cell>> findBlockingMove(const Board& b);
    std::pair<int, int> randomEmptyCell(const Board& b);
};


