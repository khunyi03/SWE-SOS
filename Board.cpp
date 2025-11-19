#include "Board.h"

bool Board::place(int r, int c, Cell letter) {
    if (!inBounds(r, c) || letter == Cell::Empty) return false;
    const std::size_t idx = static_cast<std::size_t>(r) * static_cast<std::size_t>(n_)
        + static_cast<std::size_t>(c);
    auto& slot = cells_[idx];
    if (slot != Cell::Empty) return false;
    slot = letter;
    return true;
}

