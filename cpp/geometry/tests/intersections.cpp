#include "intersections.hpp"
#include <cassert>
#include <iostream>

int main() {
    Point inter;

    // Proper intersection
    {
        Segment a({0, 0}, {4, 4});
        Segment b({0, 4}, {4, 0});
        assert(a.intersects(b, inter));
        assert(equal(inter.x, 2.0));
        assert(equal(inter.y, 2.0));
    }

    // Touching at an endpoint
    {
        Segment a({0, 0}, {4, 0});
        Segment b({4, 0}, {4, 4});
        assert(a.intersects(b, inter));
        assert(equal(inter.x, 4.0));
        assert(equal(inter.y, 0.0));
    }

    // Parallel, non-intersecting
    {
        Segment a({0, 0}, {4, 0});
        Segment b({0, 1}, {4, 1});
        assert(!a.intersects(b, inter));
    }

    // Disjoint
    {
        Segment a({0, 0}, {2, 0});
        Segment b({3, 1}, {3, -1});
        assert(!a.intersects(b, inter));
    }

    // Colinear overlapping (currently false)
    {
        Segment a({0, 0}, {4, 0});
        Segment b({2, 0}, {6, 0});
        assert(!a.intersects(b, inter));
    }

    std::cout << "All tests passed!\n";
}
