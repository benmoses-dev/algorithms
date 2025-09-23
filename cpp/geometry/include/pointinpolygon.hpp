#pragma once

#include <cmath>
#include <utility>
#include <vector>

namespace algo::geometry {

using ll = long long;
using ul = unsigned long;
using Point = std::pair<int, int>;
using FPoint = std::pair<double, double>;
using FPolygon = std::vector<FPoint>;
using Polygon = std::vector<Point>;

class PointInPolygon {
  private:
    const double EPSILON = 1e-9;

    bool approxEqual(double a, double b) { return std::fabs(a - b) < EPSILON; }

    /**
     * This is signed positive if the point is counter clockwise relative to the two
     * vertices.
     */
    ll cross(Point a, Point b, Point c) {
        ll x1 = b.first - a.first;
        ll y1 = b.second - a.second;
        ll x2 = c.first - a.first;
        ll y2 = c.second - a.second;
        return x1 * y2 - x2 * y1;
    }

    double cross(FPoint a, FPoint b, FPoint c) {
        double x1 = b.first - a.first;
        double y1 = b.second - a.second;
        double x2 = c.first - a.first;
        double y2 = c.second - a.second;
        return x1 * y2 - x2 * y1;
    }

    /**
     * Check whether the point is colinear and then check whether it is actually within
     * the bounds of the line.
     */
    bool onSegment(Point a, Point b, Point p) {
        if (cross(a, b, p) != 0)
            return false;
        int minX = std::min(a.first, b.first);
        int maxX = std::max(a.first, b.first);
        int minY = std::min(a.second, b.second);
        int maxY = std::max(a.second, b.second);
        return minX <= p.first && p.first <= maxX && minY <= p.second && p.second <= maxY;
    }

    bool onSegment(FPoint a, FPoint b, FPoint p) {
        if (std::fabs(cross(a, b, p)) > EPSILON)
            return false;
        double dx1 = b.first - a.first;
        double dy1 = b.second - a.second;
        double dx2 = p.first - a.first;
        double dy2 = p.second - a.second;
        double dot = dx1 * dx2 + dy1 * dy2;
        double sqlen = dx1 * dx1 + dy1 * dy1;
        return dot >= -EPSILON && dot <= sqlen + EPSILON;
    }

  public:
    /**
     * Simplest method for non-self-intersecting arbitrary polygons given integer
     * vertices.
     */
    bool rcpip(const Polygon &poly, Point p) {
        ul n = poly.size();
        bool inside = false;
        for (ul i = 0, j = n - 1; i < n; j = i++) {
            Point a = poly[j];
            Point b = poly[i];
            if (onSegment(a, b, p)) {
                return true;
            }
            bool aboveA = a.second < p.second;
            bool aboveB = b.second < p.second;
            if (aboveA ^ aboveB) { // p is horizontal to ab
                ll orient = cross(a, b, p);
                bool asc = a.second < b.second;
                bool leftTurn = orient > 0;
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
    bool wnpip(const FPolygon &poly, FPoint p, bool ccw) {
        int wn = 0;
        ul n = poly.size();
        for (ul i = 0; i < n; ++i) {
            FPoint a = poly[i];
            FPoint b = poly[(i + 1) % n];
            if (onSegment(a, b, p)) {
                return true;
            }
            bool aBelow = a.second <= p.second;
            bool bAbove = b.second > p.second;
            double c = cross(a, b, p);
            bool leftTurn = c > EPSILON;
            bool rightTurn = c < -EPSILON;
            if (aBelow && bAbove && ((leftTurn && ccw) || (rightTurn && !ccw))) {
                wn++;
                continue;
            }
            bool aAbove = !aBelow;
            bool bBelow = !bAbove;
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
    bool wnpip(const Polygon &poly, Point p, bool ccw) {
        int wn = 0;
        ul n = poly.size();
        for (ul i = 0; i < n; i++) {
            Point a = poly[i];
            Point b = poly[(i + 1) % n];
            if (onSegment(a, b, p)) {
                return true;
            }
            bool aBelow = a.second <= p.second;
            bool bAbove = b.second > p.second;
            ll c = cross(a, b, p);
            bool leftTurn = c > 0;
            bool rightTurn = c < 0;
            if (aBelow && bAbove && ((leftTurn && ccw) || (rightTurn && !ccw))) {
                wn++;
                continue;
            }
            bool aAbove = !aBelow;
            bool bBelow = !bAbove;
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
    bool simplepicp(const Polygon &poly, Point p) {
        ul n = poly.size();
        for (ul i = 0; i < n; ++i) {
            Point a = poly[i];
            Point b = poly[(i + 1) % n];
            ll c = cross(a, b, p);
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
    bool simplepicp(const FPolygon &poly, FPoint p) {
        ul n = poly.size();
        for (ul i = 0; i < n; ++i) {
            FPoint a = poly[i];
            FPoint b = poly[(i + 1) % n];
            double c = cross(a, b, p);
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
    bool binarypicp(const Polygon &poly, Point p) {
        ul n = poly.size();
        Point p0 = poly[0];
        if (cross(p0, poly[1], p) < 0) { // > 0 if CW.
            return false;
        }
        if (cross(p0, poly[n - 1], p) > 0) { // < 0 if CW.
            return false;
        }
        ul low = 1;
        ul high = n - 1;
        while (high - low > 1) {
            ul mid = (low + high) / 2;
            if (cross(p0, poly[mid], p) > 0) { // < 0 if CW.
                low = mid;
            } else {
                high = mid;
            }
        }
        Point a = poly[low];
        Point b = poly[high];
        return cross(a, b, p) >= 0;
    }

    /**
     * Floating point version (for example, lat and lon).
     * Assumes vertices are given in CCW winding order.
     */
    bool binarypicp(const FPolygon &poly, FPoint p) {
        ul n = poly.size();
        FPoint p0 = poly[0];
        if (cross(p0, poly[1], p) < -EPSILON) { // Use > EPSILON if CW.
            return false;
        }
        if (cross(p0, poly[n - 1], p) > EPSILON) { // Use < -EPSILON if CW.
            return false;
        }
        ul low = 1;
        ul high = n - 1;
        while (high - low > 1) {
            ul mid = (low + high) / 2;
            if (cross(p0, poly[mid], p) > EPSILON) { // Use < -EPSILON if CW.
                low = mid;
            } else {
                high = mid;
            }
        }
        FPoint a = poly[low];
        FPoint b = poly[high];
        return cross(a, b, p) > -EPSILON; // Use < EPSILON if vertices are in CW order.
    }

    /**
     * Quick point in regular hexagon, assuming flat-top.
     * Would need to be rotated first if not flat-topped (probably just use orientation
     * checks).
     * Need the center point as well as the vertices.
     */
    bool pirhexagon(FPoint p, FPoint c, double s) {
        double x = p.first - c.first;
        double y = p.second - c.second;
        double sqrt3 = sqrt(3.0);
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
