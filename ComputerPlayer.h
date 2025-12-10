#pragma once
#include "Board.h"
#include "Cell.h"
#include <optional>
#include <tuple>
#include <vector>
#include <random>

class Players
{
public:
    virtual ~Players() = default;

    // true if this controller is an AI
    virtual bool isComputer() const = 0;
    virtual std::optional<std::tuple<int, int, Cell>> chooseMove(const Board& board) = 0;
};

class HumanPlayer : public Players
{
public:
    bool isComputer() const override { return false; }
    std::optional<std::tuple<int, int, Cell>> chooseMove(const Board&) override
    {
        return std::nullopt;
    }
};

class ComputerPlayer : public Players
{
public:
    bool isComputer() const override { return true; }
    std::optional<std::tuple<int, int, Cell>> chooseMove(const Board& board) override;

private:
    std::optional<std::tuple<int, int, Cell>> findMakingMove(const Board& b);
    std::optional<std::tuple<int, int, Cell>> findBlockingMove(const Board& b);
    std::pair<int, int> randomEmptyCell(const Board& b);
};
