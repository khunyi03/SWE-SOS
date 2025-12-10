#pragma once
#include "Player.h"

class TurnManager {
public:
    TurnManager() : current_(Player::Blue) {}
    Player current() const { return current_; }
    void reset() { current_ = Player::Blue; }
    void pass() { current_ = (current_ == Player::Blue ? Player::Red : Player::Blue); }
private:
    Player current_;
};

