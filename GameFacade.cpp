#include "GameFacade.h"
#include <algorithm>

static inline bool inB(const Board& b, int r, int c) { return b.inBounds(r, c); }
static inline Cell  at(const Board& b, int r, int c) { return b.at(r, c); }

GameFacade::GameFacade() {}

void GameFacade::newGame(const GameConfig& cfg) {
    // are we in simple or general mode
    mode_ = cfg.mode;
    result_ = GameResult::Ongoing;
    scoreBlue_ = 0;
    scoreRed_ = 0;
    lastSeq_.clear();

    board_.reset(cfg.size);

    const int N = board_.size();
    owners_.assign(N, std::vector<std::optional<Player>>(N, std::nullopt));

    tm_.reset(); // Blue goes first
}

MoveOutcome GameFacade::placeLetter(int r, int c, Cell letter) {
    MoveOutcome out;
    if (result_ != GameResult::Ongoing) return out;
    out.placed = board_.place(r, c, letter);
    if (!out.placed) return out;

    // who placed it
    if (r >= 0 && c >= 0 && r < board_.size() && c < board_.size()) {
        owners_[r][c] = tm_.current();
    }

    // check if this move made any SOS lines
    lastSeq_ = detectNewSequences(r, c, letter);
    out.newSequences = static_cast<int>(lastSeq_.size());

    // SIMPLE mode: first person to make SOS wins
    if (mode_ == GameMode::Simple) {
        if (out.newSequences > 0) {
            if (tm_.current() == Player::Blue) {
                ++scoreBlue_;
                result_ = GameResult::BlueWins;
            }
            else {
                ++scoreRed_;
                result_ = GameResult::RedWins;
            }
        }
        else if (isBoardFull()) {
            result_ = GameResult::Draw;
        }
        else {
            tm_.pass();
        }
    }
    else { // GENERAL mode
        if (out.newSequences > 0) {
            // add points for # of SOS found
            if (tm_.current() == Player::Blue) scoreBlue_ += out.newSequences;
            else                               scoreRed_ += out.newSequences;

            out.extraTurn = true; // player keeps going if they scored
        }
        else {
            tm_.pass();
        }

        // who won
        if (isBoardFull()) {
            if (scoreBlue_ > scoreRed_) result_ = GameResult::BlueWins;
            else if (scoreRed_ > scoreBlue_) result_ = GameResult::RedWins;
            else                              result_ = GameResult::Draw;
        }
    }
    out.result = result_;
    out.sequences = lastSeq_;
    return out;
}

bool GameFacade::isBoardFull() const {
    const int N = board_.size();
    for (int r = 0; r < N; ++r)
        for (int c = 0; c < N; ++c)
            if (board_.at(r, c) == Cell::Empty) return false;
    return true;
}

std::vector<Sequence> GameFacade::detectNewSequences(int r, int c, Cell L) const {
    std::vector<Sequence> seqs;
    const int D4[4][2] = { {1,0},{0,1},{1,1},{1,-1} };

    // look for S-O-S pattern
    if (L == Cell::O) {
        for (auto d : D4) {
            int r1 = r + d[0], c1 = c + d[1];
            int r2 = r - d[0], c2 = c - d[1];
            if (inB(board_, r1, c1) && inB(board_, r2, c2) &&
                at(board_, r1, c1) == Cell::S && at(board_, r2, c2) == Cell::S) {
                seqs.push_back({ {r1,c1},{r,c},{r2,c2} });
            }
        }
    }
    else { 
        for (auto d : D4) {
            int rO = r + d[0], cO = c + d[1];
            int rS = r + 2 * d[0], cS = c + 2 * d[1];
            if (inB(board_, rO, cO) && inB(board_, rS, cS) &&
                at(board_, rO, cO) == Cell::O && at(board_, rS, cS) == Cell::S) {
                seqs.push_back({ {r,c},{rO,cO},{rS,cS} });
            }
            int rO2 = r - d[0], cO2 = c - d[1];
            int rS2 = r - 2 * d[0], cS2 = c - 2 * d[1];
            if (inB(board_, rO2, cO2) && inB(board_, rS2, cS2) &&
                at(board_, rO2, cO2) == Cell::O && at(board_, rS2, cS2) == Cell::S) {
                seqs.push_back({ {rS2,cS2},{rO2,cO2},{r,c} });
            }
        }
    }
    return seqs;
}
