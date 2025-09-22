#include "polygonarea.hpp"
#include <cassert>
#include <iostream>

const ld EPS = 1e-9;

inline bool nearlyEqual(ld a, ld b) { return fabsl(a - b) < EPS; }

int main() {
    // Right triangle (3-4-5 triangle, area should be 6)
    {
        Polygon tri = {{0, 0}, {3, 0}, {0, 4}};
        ld area = irregularArea(tri);
        assert(nearlyEqual(area, 6.0));
    }

    // Square with side length 2 (area = 4)
    {
        Polygon square = {{0, 0}, {2, 0}, {2, 2}, {0, 2}};
        ld area = irregularArea(square);
        assert(nearlyEqual(area, 4.0));
    }

    // Concave polygon (arrow shape)
    {
        Polygon concave = {{0, 0}, {4, 0}, {4, 2}, {2, 1}, {0, 2}};
        // Can compute manually: area = 6
        ld area = irregularArea(concave);
        assert(nearlyEqual(area, 6.0));
    }

    // Regular triangle (equilateral, side = 2)
    {
        ld area = regularArea(3, 2);
        // Exact: sqrt(3)
        assert(nearlyEqual(area, std::sqrt(3.0L)));
    }

    // Regular square (side = 2)
    {
        ld area = regularArea(4, 2);
        assert(nearlyEqual(area, 4.0));
    }

    // Regular hexagon (side = 2)
    {
        ld area = regularArea(6, 2);
        // (3 * sqrt(3) / 2) * side^2 = 2 * 3 * sqrt(3) = 10.3923
        ld expected = (3.0L * std::sqrt(3.0L) / 2.0L) * 4.0L;
        assert(nearlyEqual(area, expected));
    }

    // Degenerate: fewer than 3 points
    {
        Polygon line = {{0, 0}, {1, 1}};
        ld area = irregularArea(line);
        assert(nearlyEqual(area, 0.0));
        assert(nearlyEqual(regularArea(2, 5), 0.0));
    }

    std::cout << "All polygon area tests passed!\n";
}
