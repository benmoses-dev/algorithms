#pragma once

#include <cstdint>
#include <vector>

namespace algo::numbers {

using u64 = std::uint64_t;

/**
 * Use Pascal's triangle to compute binomial coefficients.
 */
inline std::vector<std::vector<u64>> pascalsTriangle(u64 maxN, u64 maxR) {
    std::vector<std::vector<u64>> pt(maxN + 1, std::vector<u64>(maxR + 1, 0));
    for (std::size_t i = 0; i <= maxN; i++) {
        pt[i][0] = 1;
        for (std::size_t j = 1; j <= std::min(i, maxR); j++) {
            pt[i][j] = pt[i - 1][j - 1] + pt[i - 1][j];
        }
    }
    return pt;
}

} // namespace algo::numbers
