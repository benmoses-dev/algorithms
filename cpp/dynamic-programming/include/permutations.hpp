#pragma once

#include <stdexcept>
#include <vector>

namespace algo::dp {

using ll = long long;

inline ll permutations(std::vector<std::size_t> &options, const int sum) {
    if (sum < 0) {
        throw std::invalid_argument("Sum must be non-negative");
    }
    const std::size_t s = static_cast<std::size_t>(sum);
    // Ways to sum to an increasing amount (i)
    std::vector<ll> dp(s + 1, 0);
    dp[0] = 1; // The empty set
    for (std::size_t i = 1; i <= s; i++) {
        /**
         * If you can sum to x using y permutations, then you can sum to (x + option = i)
         * by using y + 1 permutations - simply add the option. Repeat for every possible
         * option.
         */
        for (const auto &option : options) {
            if (option <= i) {
                dp[i] += dp[i - option];
            }
        }
    }
    return dp[s];
}

} // namespace algo::dp
