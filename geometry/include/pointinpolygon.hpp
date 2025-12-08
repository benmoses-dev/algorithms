#pragma once

#include <cmath>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>

namespace algo::geometry {

using i64 = std::int64_t;
using Point = std::pair<i64, i64>;
using FPoint = std::pair<double, double>;
using FPolygon = std::vector<FPoint>;
using Polygon = std::vector<Point>;

class PointInPolygon {
  private:
    const double EPSILON = 1e-9;

    bool approxEqual(const double a, const double b) const {
        return std::fabs(a - b) < EPSILON;
    }

    /**
     * This is signed positive if the point is counter clockwise relative to the two
     * vertices.
     */
    i64 cross(const Point a, const Point b, const Point c) const {
        const i64 x1 = b.first - a.first;
        const i64 y1 = b.second - a.second;
        const i64 x2 = c.first - a.first;
        const i64 y2 = c.second - a.second;
        return x1 * y2 - x2 * y1;
    }

    double cross(const FPoint a, const FPoint b, const FPoint c) const {
        const double x1 = b.first - a.first;
        const double y1 = b.second - a.second;
        const double x2 = c.first - a.first;
        const double y2 = c.second - a.second;
        return x1 * y2 - x2 * y1;
    }

    /**
     * Check whether the point is colinear and then check whether it is actually within
     * the bounds of the line.
     */
    bool onSegment(const Point a, const Point b, const Point p) const {
        if (cross(a, b, p) != 0) {
            return false;
        }
        const i64 minX = std::min(a.first, b.first);
        const i64 maxX = std::max(a.first, b.first);
        const i64 minY = std::min(a.second, b.second);
        const i64 maxY = std::max(a.second, b.second);
        return minX <= p.first && p.first <= maxX && minY <= p.second && p.second <= maxY;
    }

    bool onSegment(const FPoint a, const FPoint b, const FPoint p) const {
        if (std::fabs(cross(a, b, p)) > EPSILON) {
            return false;
        }
        const double dx1 = b.first - a.first;
        const double dy1 = b.second - a.second;
        const double dx2 = p.first - a.first;
        const double dy2 = p.second - a.second;
        const double dot = dx1 * dx2 + dy1 * dy2;
        const double sqlen = dx1 * dx1 + dy1 * dy1;
        return dot >= -EPSILON && dot <= sqlen + EPSILON;
    }

  public:
    /**
     * Simplest method for non-self-intersecting arbitrary polygons given integer
     * vertices.
     */
    bool rcpip(const Polygon &poly, const Point p) const {
        const std::size_t n = poly.size();
        if (n <= 2) {
            throw std::invalid_argument("Polygon must have more than 2 vertices!");
        }
        bool inside = false;
        for (std::size_t i = 0, j = n - 1; i < n; j = i++) {
            const Point a = poly[j];
            const Point b = poly[i];
            if (onSegment(a, b, p)) {
                return true;
            }
            const bool aboveA = a.second < p.second;
            const bool aboveB = b.second < p.second;
            if (aboveA ^ aboveB) { // p is horizontal to ab
                const i64 orient = cross(a, b, p);
                const bool asc = a.second < b.second;
                const bool leftTurn = orient > 0;
                if (asc == leftTurn) { // p is to the left of ab
                    inside = !inside;
                }
            }
        }
        return inside;
    }

    /**
     * Use this if the inputs are floating point numbers, otherwise use the integer
     * version below. Assumes vertices are given in CCW winding order.
     */
    bool wnpip(const FPolygon &poly, const FPoint p, const bool ccw) const {
        const std::size_t n = poly.size();
        if (n <= 2) {
            throw std::invalid_argument("Polygon must have more than 2 vertices!");
        }
        i64 wn = 0;
        for (std::size_t i = 0; i < n; i++) {
            const FPoint a = poly[i];
            const FPoint b = poly[(i + 1) % n];
            if (onSegment(a, b, p)) {
                return true;
            }
            const bool aBelow = a.second <= p.second;
            const bool bAbove = b.second > p.second;
            const double c = cross(a, b, p);
            const bool leftTurn = c > EPSILON;
            const bool rightTurn = c < -EPSILON;
            if (aBelow && bAbove && ((leftTurn && ccw) || (rightTurn && !ccw))) {
                wn++;
                continue;
            }
            const bool aAbove = !aBelow;
            const bool bBelow = !bAbove;
            if (aAbove && bBelow && ((rightTurn && ccw) || (leftTurn && !ccw))) {
                wn--;
            }
        }
        return wn != 0;
    }

    /**
     * Integer-based winding number algorithm to check whether a point is within an
     * arbitrary polygon. Use this if inputs are integers. Can be used on regular or
     * irregular polygons. Polygons do not need to be convex and they can include
     * self-intersections.
     */
    bool wnpip(const Polygon &poly, const Point p, const bool ccw) const {
        const std::size_t n = poly.size();
        if (n <= 2) {
            throw std::invalid_argument("Polygon must have more than 2 vertices!");
        }
        i64 wn = 0;
        for (std::size_t i = 0; i < n; i++) {
            const Point a = poly[i];
            const Point b = poly[(i + 1) % n];
            if (onSegment(a, b, p)) {
                return true;
            }
            const bool aBelow = a.second <= p.second;
            const bool bAbove = b.second > p.second;
            const i64 c = cross(a, b, p);
            const bool leftTurn = c > 0;
            const bool rightTurn = c < 0;
            if (aBelow && bAbove && ((leftTurn && ccw) || (rightTurn && !ccw))) {
                wn++;
                continue;
            }
            const bool aAbove = !aBelow;
            const bool bBelow = !bAbove;
            if (aAbove && bBelow && ((rightTurn && ccw) || (leftTurn && !ccw))) {
                wn--;
            }
        }
        return wn != 0;
    }

    /**
     * Simple orientation check for a point in a convex polygon.
     * Probably quicker than calculating the center even for regular polygons.
     * Assumes vertices are given in CCW winding order.
     */
    bool simplepicp(const Polygon &poly, const Point p) const {
        const std::size_t n = poly.size();
        if (n <= 2) {
            throw std::invalid_argument("Polygon must have more than 2 vertices!");
        }
        for (std::size_t i = 0; i < n; ++i) {
            const Point a = poly[i];
            const Point b = poly[(i + 1) % n];
            const i64 c = cross(a, b, p);
            // Assuming CCW order. Flip conditionals if CW.
            if (c < 0) {
                return false;
            }
            if (c > 0) {
                continue;
            }
            return onSegment(a, b, p);
        }
        return true;
    }

    /**
     * Use instead of above if given floating point numbers for vertices.
     * Assumes vertices are given in CCW winding order.
     */
    bool simplepicp(const FPolygon &poly, const FPoint p) const {
        const std::size_t n = poly.size();
        if (n <= 2) {
            throw std::invalid_argument("Polygon must have more than 2 vertices!");
        }
        for (std::size_t i = 0; i < n; ++i) {
            const FPoint a = poly[i];
            const FPoint b = poly[(i + 1) % n];
            const double c = cross(a, b, p);
            // Flip conditionals if CW.
            if (c < -EPSILON) {
                return false;
            }
            if (c > EPSILON) {
                continue;
            }
            return onSegment(a, b, p);
        }
        return true;
    }

    /**
     * Binary search version for point in convex polygon.
     * Vertices must be in winding order.
     * Assumes vertices are given in CCW winding order.
     * Really only necessary for large n.
     */
    bool binarypicp(const Polygon &poly, const Point p) const {
        const std::size_t n = poly.size();
        if (n <= 2) {
            throw std::invalid_argument("Polygon must have more than 2 vertices!");
        }
        const Point p0 = poly[0];
        if (cross(p0, poly[1], p) < 0) { // > 0 if CW.
            return false;
        }
        if (cross(p0, poly[n - 1], p) > 0) { // < 0 if CW.
            return false;
        }
        std::size_t low = 1;
        std::size_t high = n - 1;
        while (high - low > 1) {
            const std::size_t mid = (low + high) / 2;
            if (cross(p0, poly[mid], p) > 0) { // < 0 if CW.
                low = mid;
            } else {
                high = mid;
            }
        }
        const Point a = poly[low];
        const Point b = poly[high];
        return cross(a, b, p) >= 0;
    }

    /**
     * Floating point version (for example, lat and lon).
     * Assumes vertices are given in CCW winding order.
     */
    bool binarypicp(const FPolygon &poly, const FPoint p) const {
        const std::size_t n = poly.size();
        if (n <= 2) {
            throw std::invalid_argument("Polygon must have more than 2 vertices!");
        }
        const FPoint p0 = poly[0];
        if (cross(p0, poly[1], p) < -EPSILON) { // Use > EPSILON if CW.
            return false;
        }
        if (cross(p0, poly[n - 1], p) > EPSILON) { // Use < -EPSILON if CW.
            return false;
        }
        std::size_t low = 1;
        std::size_t high = n - 1;
        while (high - low > 1) {
            const std::size_t mid = (low + high) / 2;
            if (cross(p0, poly[mid], p) > EPSILON) { // Use < -EPSILON if CW.
                low = mid;
            } else {
                high = mid;
            }
        }
        const FPoint a = poly[low];
        const FPoint b = poly[high];
        return cross(a, b, p) > -EPSILON; // Use < EPSILON if vertices are in CW order.
    }

    /**
     * Quick point in regular hexagon, assuming flat-top.
     * Would need to be rotated first if not flat-topped (probably just use orientation
     * checks).
     * Need the center point as well as the vertices.
     */
    bool pirhexagon(const FPoint p, const FPoint c, const double s) const {
        const double x = p.first - c.first;
        const double y = p.second - c.second;
        const double sqrt3 = sqrt(3.0);
        // Quick bounding box
        if (std::fabs(x) > s) {
            return false;
        }
        if (std::fabs(y) > sqrt3 * s / 2.0) {
            return false;
        }
        // Slope check (abs(y) = -sqrt(3) * abs(x) + sqrt(3) * s)
        return (sqrt3 * std::fabs(x) + std::fabs(y) <= sqrt3 * s);
    }
};

} // namespace algo::geometry
