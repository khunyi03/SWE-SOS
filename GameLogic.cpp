#include "GameLogic.h"

GameLogic::GameLogic() {
    newGame(8, 0); // Default 8x8 simple game
}

void GameLogic::newGame(int size, int mode) {
    // Basic validation
    if (size > MAX_BOARD_SIZE) {
        boardSize = MAX_BOARD_SIZE;
    }
    else if (size < 3) {
        boardSize = 3;
    }
    else {
        boardSize = size;
    }

    gameMode = mode;
    currentPlayer = 0; // Blue player

    // Reset board
    for (int r = 0; r < MAX_BOARD_SIZE; r++) {
        for (int c = 0; c < MAX_BOARD_SIZE; c++) {
            board[r][c] = 0; // 0 = empty
        }
    }
}

bool GameLogic::makeMove(int r, int c, int letter) {
    // Check if move is valid
    if (r < 0 || r >= boardSize || c < 0 || c >= boardSize) {
        return false; // Out of bounds
    }
    if (board[r][c] != 0) {
        return false; // Cell not empty
    }

    board[r][c] = letter; // Place the letter (1 or 2)

    // Switch player
    if (currentPlayer == 0) {
        currentPlayer = 1; // to Red
    }
    else {
        currentPlayer = 0; // to Blue
    }

    return true;
}

int GameLogic::getCell(int r, int c) {
    return board[r][c];
}

int GameLogic::getTurn() {
    return currentPlayer;
}