#pragma once
#include <vector>
#include <algorithm>
#include <cstddef>
#include "Cell.h"

class Board {
public:
    // Make a board, default size is 3x3. Calls reset() to set everything up.
    explicit Board(int n = 3) : n_(3), cells_(9, Cell::Empty) { reset(n); }

    int size() const { return n_; }

    // Check if row/col is actually inside the board.
    bool inBounds(int r, int c) const { return r >= 0 && r < n_ && c >= 0 && c < n_; }

    // Get whatever is in this spot. If it’s out of bounds, just say it's Empty.
    Cell at(int r, int c) const {
        if (!inBounds(r, c)) return Cell::Empty;
        const std::size_t idx = static_cast<std::size_t>(r) * static_cast<std::size_t>(n_)
            + static_cast<std::size_t>(c);
        return cells_[idx];
    }

    bool place(int r, int c, Cell letter);

    // Reset the board to size n. Also making sure it doesn't go crazy big.
    void reset(int n) {
        // Make sure size is at least 1 and not way too big.
        if (n < 1) n = 3;
        if (n > 10) n = 10;
        n_ = n;

        // Resize the cell list for the board. Max is a 10x10 board.
        const std::size_t wanted =
            static_cast<std::size_t>(n_) * static_cast<std::size_t>(n_);
        const std::size_t total = std::min<std::size_t>(wanted, 100u);
        cells_.assign(total, Cell::Empty);
    }

    // Just set every cell back to Empty (used when starting a new game).
    void clear() { std::fill(cells_.begin(), cells_.end(), Cell::Empty); }

private:
    int n_;
    std::vector<Cell> cells_;
};

