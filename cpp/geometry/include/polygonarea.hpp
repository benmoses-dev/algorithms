#pragma once

#include <cmath>
#include <cstdint>
#include <vector>

namespace algo::geometry {

using Point = std::pair<std::int64_t, std::int64_t>;
using Polygon = std::vector<Point>;
using ld = long double;

/**
 * Using winding cross-product:
 * Area = abs(sum((Xi * Yi+1) - (Xi+1 * Yi))) / 2
 */
inline ld irregularArea(const Polygon &c) {
    ld sum = 0.0;
    std::size_t n = c.size();
    if (n < 3)
        return 0.0;
    for (std::size_t i = 0; i < n; i++) {
        ld Xi = c[i].first;
        ld Yi = c[i].second;
        ld Ynext = c[(i + 1) % n].second;
        ld Xnext = c[(i + 1) % n].first;
        sum += (Xi * Ynext) - (Xnext * Yi);
    }
    return std::fabs(sum) / 2.0;
}

/**
 * For a regular polygon with n sides of length l:
 * Area = (n * l * l) / (4.0 * tan(pi / n))
 */
inline ld regularArea(std::size_t n, std::size_t l) {
    if (n < 3)
        return 0.0;
    return (n * l * l) / (4.0 * tanl(M_PI / static_cast<ld>(n)));
}

} // namespace algo::geometry
