#pragma once

#include <algorithm>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vector>

namespace algo::graph {

const int INF = std::numeric_limits<int>::max();

struct HungarianResult {
    std::vector<int> assignments;
    int cost;
    std::size_t totalCols;
};

inline HungarianResult hungarian(const std::vector<std::vector<int>> &input) {
    const std::size_t n = input.size();
    if (n == 0 || input[0].empty()) {
        throw std::invalid_argument("Grid cannot be empty");
    }
    const std::size_t m = input[0].size();
    if (n > m) {
        throw std::invalid_argument("Cannot have more rows than columns");
    }
    for (const auto &row : input) {
        if (row.size() != m) {
            throw std::invalid_argument("Grid must be rectangular.");
        }
    }
    const std::size_t N = std::max(n, m);
    std::vector<std::vector<int>> grid(N + 1, std::vector<int>(N + 1, INF));

    // Copy input grid into padded [1..n][1..m]
    for (std::size_t i = 0; i < n; i++) {
        for (std::size_t j = 0; j < m; j++) {
            grid[i + 1][j + 1] = input[i][j];
        }
    }

    std::vector<int> u(N + 1), v(N + 1);
    std::vector<std::size_t> matchCol(N + 1, 0);

    for (std::size_t row = 1; row <= N; row++) {
        std::vector<int> minSlack(N + 1, INF);
        std::vector<std::size_t> path(N + 1, 0);
        std::vector<uint8_t> colUsed(N + 1, 0);

        matchCol[0] = row;
        std::size_t currentCol = 0;

        do {
            colUsed[currentCol] = 1;
            const std::size_t matchedRow = matchCol[currentCol];

            int delta = INF;
            std::size_t nextCol = 0;

            for (std::size_t col = 1; col <= N; col++) {
                if (colUsed[col] == 1)
                    continue;

                const int reduced = grid[matchedRow][col] - u[matchedRow] - v[col];

                if (reduced < minSlack[col]) {
                    minSlack[col] = reduced;
                    path[col] = currentCol;
                }
                if (minSlack[col] < delta) {
                    delta = minSlack[col];
                    nextCol = col;
                }
            }
            currentCol = nextCol;

            for (std::size_t col = 0; col <= N; col++) {
                if (colUsed[col] == 1) {
                    v[col] -= delta;
                    u[matchCol[col]] += delta;
                } else {
                    minSlack[col] -= delta;
                }
            }
        } while (matchCol[currentCol] != 0);

        do {
            const std::size_t prevCol = path[currentCol];
            matchCol[currentCol] = matchCol[prevCol];
            currentCol = prevCol;
        } while (currentCol != 0);
    }

    std::vector<int> assignment(n, -1);
    int cost = 0;
    std::size_t count = 0;

    for (std::size_t col = 1; col <= m; col++) {
        std::size_t row = matchCol[col];
        if (row >= 1 && row <= n) {
            assignment[row - 1] = (int)col - 1;
            count++;
            cost += grid[row][col];
        }
    }

    return HungarianResult{assignment, cost, count};
}

} // namespace algo::graph
