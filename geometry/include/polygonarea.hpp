#pragma once

#include <algorithm>
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
    const std::size_t n = c.size();
    if (n < 3) {
        return 0.0;
    }
    for (std::size_t i = 0; i < n; i++) {
        const ld Xi = c[i].first;
        const ld Yi = c[i].second;
        const ld Ynext = c[(i + 1) % n].second;
        const ld Xnext = c[(i + 1) % n].first;
        sum += (Xi * Ynext) - (Xnext * Yi);
    }
    return std::fabs(sum) / 2.0;
}

/**
 * For a regular polygon with n sides of length l:
 * Area = (n * l * l) / (4.0 * tan(pi / n))
 */
inline ld regularArea(const std::size_t n, const std::size_t l) {
    if (n < 3) {
        return 0.0;
    }
    return (n * l * l) / (4.0 * tanl(M_PI / static_cast<ld>(n)));
}

/**
 * Cross product of vectors (OA x OB)
 * Positive => O->A->B makes a counter-clockwise turn
 * Negative => clockwise turn
 * Zero     => collinear
 */
inline std::int64_t cross(const Point &O, const Point &A, const Point &B) {
    return (A.first - O.first) * (B.second - O.second) -
           (A.second - O.second) * (B.first - O.first);
}

/**
 * Compute the convex hull of a set of points using a monotonic chain.
 *
 * Properties:
 *  - Time complexity: O(n log n)
 *  - Uses only integer arithmetic
 *  - Returns hull vertices in counter-clockwise order
 *  - Does NOT repeat the first point at the end
 */
inline Polygon convexHull(Polygon points) {
    std::sort(points.begin(), points.end());
    points.erase(std::unique(points.begin(), points.end()), points.end());
    const std::size_t n = points.size();
    if (n <= 2) {
        return points;
    }
    Polygon hull;
    hull.reserve(n * 2);
    for (const Point &p : points) {
        while (hull.size() >= 2) {
            const std::size_t m = hull.size();
            /**
             * This keeps collinear points along the same edge, adding them to the hull in
             * lexicographical order. To remove these points and only keep the very first
             * and very last collinear point for each collinear edge chain, change this to
             * > 0.
             */
            if (cross(hull[m - 2], hull[m - 1], p) >= 0) {
                break;
            }
            hull.pop_back();
        }
        hull.emplace_back(p);
    }
    const std::size_t lowerSize = hull.size();
    for (std::size_t i = n - 1; i > 0; i--) {
        const Point &p = points[i - 1];
        while (hull.size() > lowerSize) {
            const std::size_t m = hull.size();
            if (cross(hull[m - 2], hull[m - 1], p) >= 0) {
                break;
            }
            hull.pop_back();
        }
        hull.emplace_back(p);
    }
    hull.pop_back();
    return hull;
}

/**
 * Returns the area of a triangle given the vertices.
 */
inline ld triArea(const Polygon &triangle) {
    const auto &[x1, y1] = triangle[0];
    const auto &[x2, y2] = triangle[1];
    const auto &[x3, y3] = triangle[2];
    return fabsl(((ld)x1 * (y2 - y3)) + ((ld)x2 * (y3 - y1)) + ((ld)x3 * (y1 - y2))) /
           2.0;
};

/**
 * Returns the area of a cyclic quadrilateral given the lengths of the sides
 */
inline double cyclicQuad(const double a, const double b, const double c, const double d) {
    if (a <= 0 || b <= 0 || c <= 0 || d <= 0) {
        return 0.0;
    }
    const double s = (a + b + c + d) / 2.0;
    const double squared = (s - a) * (s - b) * (s - c) * (s - d);
    return squared > 0.0 ? std::sqrt(squared) : 0.0;
}

} // namespace algo::geometry
