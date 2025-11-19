#include "ComputerPlayer.h"

static inline bool inB(const Board& b, int r, int c) { return b.inBounds(r, c); }
static inline Cell  at(const Board& b, int r, int c) { return b.at(r, c); }

std::optional<std::tuple<int, int, Cell>> ComputerPlayer::chooseMove(const Board& board) {
    if (auto m = findMakingMove(board))   return m;
    if (auto b = findBlockingMove(board)) return b;
    auto [r, c] = randomEmptyCell(board);
    // pick S or O randomly
    Cell L = (std::rand() % 2 == 0) ? Cell::S : Cell::O;
    return std::make_tuple(r, c, L);
}

std::optional<std::tuple<int, int, Cell>> ComputerPlayer::findMakingMove(const Board& b) {
    const int N = b.size();
    const int D4[4][2] = { {1,0},{0,1},{1,1},{1,-1} };

    for (int r = 0; r < N; ++r) for (int c = 0; c < N; ++c) if (b.at(r, c) == Cell::Empty) {
        for (auto d : D4) {
            int r1 = r + d[0], c1 = c + d[1], r2 = r - d[0], c2 = c - d[1];
            if (inB(b, r1, c1) && inB(b, r2, c2) && at(b, r1, c1) == Cell::S && at(b, r2, c2) == Cell::S)
                return std::make_tuple(r, c, Cell::O);

            int rO = r + d[0], cO = c + d[1], rS = r + 2 * d[0], cS = c + 2 * d[1];
            if (inB(b, rO, cO) && inB(b, rS, cS) && at(b, rO, cO) == Cell::O && at(b, rS, cS) == Cell::S)
                return std::make_tuple(r, c, Cell::S);
        }
    }
    return std::nullopt;
}

std::optional<std::tuple<int, int, Cell>> ComputerPlayer::findBlockingMove(const Board& b) {
    // block obvious S-O-S
    const int N = b.size();
    const int D4[4][2] = { {1,0},{0,1},{1,1},{1,-1} };
    for (int r = 0; r < N; ++r) for (int c = 0; c < N; ++c) if (b.at(r, c) == Cell::Empty) {
        for (auto d : D4) {
            int r1 = r + d[0], c1 = c + d[1], r2 = r - d[0], c2 = c - d[1];
            if (inB(b, r1, c1) && inB(b, r2, c2) && at(b, r1, c1) == Cell::S && at(b, r2, c2) == Cell::S)
                return std::make_tuple(r, c, Cell::O);
        }
    }
    return std::nullopt;
}

std::pair<int, int> ComputerPlayer::randomEmptyCell(const Board& b) {
    const int N = b.size();
    std::vector<std::pair<int, int>> empty;
    empty.reserve(N * N);
    for (int r = 0; r < N; ++r) for (int c = 0; c < N; ++c)
        if (b.at(r, c) == Cell::Empty) empty.emplace_back(r, c);
    if (empty.empty()) return { 0,0 };
    std::mt19937 gen(static_cast<unsigned>(time(nullptr)));
    std::uniform_int_distribution<> dis(0, static_cast<int>(empty.size()) - 1);
    return empty[dis(gen)];
}
