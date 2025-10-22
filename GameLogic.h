#pragma once

// Use a max size defined here
#define MAX_BOARD_SIZE 10

class GameLogic {
public:
    // Game state is stored in simple ints
    int board[MAX_BOARD_SIZE][MAX_BOARD_SIZE]; // 0=empty, 1=S, 2=O
    int boardSize;
    int gameMode;      // 0=Simple, 1=General
    int currentPlayer; // 0=Blue, 1=Red

public:
    GameLogic();
    void newGame(int size, int mode);
    bool makeMove(int r, int c, int letter);

    // Getters
    int getCell(int r, int c);
    int getTurn();
};
