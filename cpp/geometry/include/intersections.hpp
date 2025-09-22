#include <cmath>

const double EPSILON = 1e-9;

inline bool equal(double a, double b) { return fabs(a - b) < EPSILON; }

struct Vec {
    double x, y;

    bool operator==(const Vec &other) const {
        return equal(other.x, x) && equal(other.y, y);
    }

    bool operator!=(const Vec &other) const { return !(*this == other); }

    double cross(const Vec &other) const { return (x * other.y) - (y * other.x); }
};

struct Point {
    double x, y;

    bool operator<(const Point &other) const {
        if (!equal(x, other.x))
            return x < other.x;
        if (!equal(y, other.y))
            return y < other.y;
        return false;
    }

    bool operator==(const Point &other) const {
        return equal(other.x, x) && equal(other.y, y);
    }

    bool operator!=(const Point &other) const { return !(*this == other); }

    Vec vec(const Point &other) const { return {x - other.x, y - other.y}; }
};

struct Segment {
    Point start;
    Point end;
    Vec vec;

    Segment(Point s, Point e)
        : start(s), end(e), vec({end.x - start.x, end.y - start.y}) {};

    /**
     * Check whether two line segments intersect and compute the intersection point.
     *
     * Parametric equations:
     *   Segment 1: P(t) = start1 + t * vone, for t in [0, 1]
     *   Segment 2: Q(u) = start2 + u * vtwo, for u in [0, 1]
     *
     * When P(t) == Q(u) - the intersection point lies on both segments:
     *   => start1 + t * vone = start2 + u * vtwo
     *   => t * vone - u * vtwo = start2 - start1 = startDiff
     *
     * Take the cross product with vtwo:
     *   => cross(t * vone - u * vtwo, vtwo) = cross(startDiff, vtwo)
     *   => t * cross(vone, vtwo) = cross(startDiff, vtwo)
     *   => t = cross(startDiff, vtwo) / cross(vone, vtwo)
     *
     * Take the cross product with vone:
     *   => u = cross(startDiff, vone) / cross(vone, vtwo)
     */
    bool intersects(const Segment &other, Point &intersection) const {
        double c = vec.cross(other.vec);
        if (equal(c, 0)) {
            return false; // Segments are parallel or colinear
        }
        Vec startDiff = other.start.vec(start);
        double t = startDiff.cross(other.vec) / c;
        double u = startDiff.cross(vec) / c;
        // Both t and u must be in [0, 1] for the intersection to lie on both segments
        if (t < 0 - EPSILON || t > 1 + EPSILON || u < 0 - EPSILON || u > 1 + EPSILON) {
            return false;
        }
        // Get the intersection point using parametric form: P(t) = start1 + t * vone
        double distanceX = t * vec.x;
        double distanceY = t * vec.y;
        intersection = {start.x + distanceX, start.y + distanceY};
        return true;
    }
};
