#pragma once
#include <vector>
#include <optional>
#include "Board.h"
#include "Cell.h"
#include "Player.h"
#include "GameConfig.h"
#include "TurnManager.h"
#include "Sequence.h"

// Game results
enum class GameResult { Ongoing, BlueWins, RedWins, Draw };


struct MoveOutcome {
    bool placed = false;
    int  newSequences = 0;
    bool extraTurn = false;
    GameResult result = GameResult::Ongoing;  
    std::vector<Sequence> sequences; 
};

class GameFacade {
public:
    GameFacade();

    // Start a new game using the given settings.
    void newGame(const GameConfig& cfg);
    void reset(const GameConfig& cfg) { newGame(cfg); }

    MoveOutcome placeLetter(int r, int c, Cell letter);

    const Board& board()   const { return board_; }
    Player        currentPlayer() const { return tm_.current(); }
    GameResult    result()  const { return result_; }
    GameMode      mode()    const { return mode_; }

    int scoreBlue() const { return scoreBlue_; }
    int scoreRed()  const { return scoreRed_; }

    const std::vector<Sequence>& lastSequences() const { return lastSeq_; }

    // Who put a letter in what cell
    std::optional<Player> ownerAt(int r, int c) const {
        if (r < 0 || c < 0) return std::nullopt;
        if (r >= static_cast<int>(owners_.size())) return std::nullopt;
        if (c >= static_cast<int>(owners_[r].size())) return std::nullopt;
        return owners_[r][c];
    }

private:
    // Finds any new SOS lines.
    std::vector<Sequence> detectNewSequences(int r, int c, Cell letter) const;

    bool isBoardFull() const;

    void handleSimpleMove(Player current, MoveOutcome& out);
    void handleGeneralMove(Player current, MoveOutcome& out);

    Board       board_{ 3 };
    TurnManager tm_;
    GameMode    mode_{ GameMode::Simple };
    GameResult  result_{ GameResult::Ongoing };

    int scoreBlue_ = 0;
    int scoreRed_ = 0;
    std::vector<Sequence> lastSeq_;
    std::vector<std::vector<std::optional<Player>>> owners_;
};
