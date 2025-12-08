#pragma once

#include <stdexcept>
#include <vector>

namespace algo::dp {

struct Item {
    int v, w;
};

inline std::vector<std::pair<int, Item>> knapsack(const std::vector<Item> &items,
                                                  const int cap) {
    if (cap < 0) {
        throw std::invalid_argument("Capacity must be non-negative");
    }
    const std::size_t C = static_cast<std::size_t>(cap);
    const std::size_t n = items.size();
    std::vector<std::vector<int>> dp(n + 1, std::vector<int>(C + 1, 0));

    for (std::size_t i = 1; i <= n; i++) {
        const std::size_t idx = i - 1; // Item index
        for (std::size_t w = 0; w <= C; w++) {
            dp[i][w] = dp[i - 1][w]; // Without
            const int prevW = static_cast<int>(w) - items[idx].w;
            if (prevW < 0) {
                continue;
            }
            const int with = dp[i - 1][static_cast<std::size_t>(prevW)] + items[idx].v;
            if (with >= dp[i][w]) {
                dp[i][w] = with;
            }
        }
    }

    std::vector<std::pair<int, Item>> added;
    std::size_t w = C;
    for (std::size_t i = n; i > 0; i--) {
        const std::size_t idx = i - 1;
        const int val = dp[i][w];
        const int prevW = static_cast<int>(w) - items[idx].w;
        if (prevW < 0) {
            throw std::runtime_error("Something weird happened...");
        }
        const std::size_t p = static_cast<std::size_t>(prevW);
        const int prev = dp[i - 1][p];
        if (val == prev + items[idx].v) {
            added.emplace_back(idx, items[idx]);
            w = p;
        }
    }

    return added;
}

} // namespace algo::dp
