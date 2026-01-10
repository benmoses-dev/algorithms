#pragma once

#include <algorithm>
#include <cstdint>
#include <vector>

namespace algo::numbers {

inline std::vector<std::size_t> grundy(const std::vector<std::size_t> &S,
                                       const std::size_t maxH) {
    const std::size_t minS = *std::min_element(S.begin(), S.end());
    std::vector<std::size_t> dp(maxH + 1, 0);
    std::vector<std::uint8_t> inSet(maxH - minS + 1, 0);
    for (std::size_t i = 1; i <= maxH; i++) {
        fill(inSet.begin(), inSet.end(), 0);
        for (const std::size_t &s : S) {
            if (i < s) {
                continue;
            }
            inSet[dp[i - s]] = 1;
        }
        std::size_t j = 0;
        while (inSet[j]) {
            j++;
        }
        dp[i] = j;
    }
    return dp;
}

} // namespace algo::numbers
