#pragma once
enum class GameMode { Simple, General };

struct GameConfig {
    int size = 3;                           // valid: 3-10
    GameMode mode = GameMode::Simple;
};

