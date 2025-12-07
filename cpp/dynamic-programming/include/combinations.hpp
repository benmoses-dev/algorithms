#pragma once

#include <stdexcept>
#include <vector>

namespace algo::dp {

using ll = long long;

inline ll combinations(std::vector<std::size_t> &options, const int total) {
    if (total < 0) {
        throw std::invalid_argument("Total must be non-negative");
    }
    const std::size_t t = static_cast<std::size_t>(total);
    std::vector<ll> dp(t + 1, 0);
    dp[0] = 1;
    /**
     * Fix the options to preserve order and prevent permutations.
     */
    for (const std::size_t &option : options) {
        for (std::size_t i = option; i <= t; i++) {
            dp[i] += dp[i - option];
        }
    }
    return dp[t];
}

} // namespace algo::dp
