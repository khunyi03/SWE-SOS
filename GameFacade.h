#pragma once
#include <vector>
#include <optional>
#include "Board.h"
#include "Cell.h"
#include "Player.h"
#include "GameConfig.h"
#include "TurnManager.h"
#include "Sequence.h"

// Tells us how the game is going overall.
enum class GameResult { Ongoing, BlueWins, RedWins, Draw };

// When a move happens, we return this to explain what changed.
struct MoveOutcome {
    bool placed = false;          // did we actually put a letter down
    int  newSequences = 0;        // how many SOS combos were made in this move
    bool extraTurn = false;       // true if player gets to go again
    GameResult result = GameResult::Ongoing;  // if game is still going or ended
    std::vector<Sequence> sequences; // stores the SOS line(s) found
};

class GameFacade {
public:
    GameFacade();

    // Start a completely new match using the given settings.
    void newGame(const GameConfig& cfg);
    void reset(const GameConfig& cfg) { newGame(cfg); }

    // Try to place S or O at (r,c). Returns info about what happened.
    MoveOutcome placeLetter(int r, int c, Cell letter);

    const Board& board()   const { return board_; }
    Player        currentPlayer() const { return tm_.current(); }
    GameResult    result()  const { return result_; }
    GameMode      mode()    const { return mode_; }

    int scoreBlue() const { return scoreBlue_; }
    int scoreRed()  const { return scoreRed_; }

    // Last found sequences
    const std::vector<Sequence>& lastSequences() const { return lastSeq_; }

    // Tells us who put a letter in what cell.
    std::optional<Player> ownerAt(int r, int c) const {
        if (r < 0 || c < 0) return std::nullopt;
        if (r >= static_cast<int>(owners_.size())) return std::nullopt;
        if (c >= static_cast<int>(owners_[r].size())) return std::nullopt;
        return owners_[r][c];
    }

private:
    // Finds any new SOS lines.
    std::vector<Sequence> detectNewSequences(int r, int c, Cell letter) const;

    // Checks if every cell is filled.
    bool isBoardFull() const;

    Board       board_{ 3 };
    TurnManager tm_;
    GameMode    mode_{ GameMode::Simple };
    GameResult  result_{ GameResult::Ongoing };

    int scoreBlue_ = 0;
    int scoreRed_ = 0;
    std::vector<Sequence> lastSeq_;
    std::vector<std::vector<std::optional<Player>>> owners_;
};
