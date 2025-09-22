#include "pointinpolygon.hpp"
#include <cassert>
#include <climits>
#include <iostream>

int main() {
    PointInPolygon pip;

    {
        // ----------------------
        // Integer square (CCW)
        // ----------------------
        Polygon square = {{0, 0}, {10, 0}, {10, 10}, {0, 10}}; // CCW
        Point insideSquare{5, 5};
        Point edgeSquare{0, 5};
        Point vertexSquare{0, 0};
        Point outsideSquare{15, 5};
        Point cornerSquare{10, 10};

        // Ray casting (integer) should include edge/vertex as inside (onSegment -> true)
        assert(pip.rcpip(square, insideSquare) == true);
        assert(pip.rcpip(square, edgeSquare) == true);
        assert(pip.rcpip(square, vertexSquare) == true);
        assert(pip.rcpip(square, cornerSquare) == true);
        assert(pip.rcpip(square, outsideSquare) == false);

        // Integer winding number (ccw = true for this CCW polygon)
        assert(pip.wnpip(square, insideSquare, true) == true);
        assert(pip.wnpip(square, edgeSquare, true) == true);
        assert(pip.wnpip(square, outsideSquare, true) == false);

        // Simple convex orientation test (integer) - true for inside, true for
        // edge/vertex
        assert(pip.simplepicp(square, insideSquare) == true);
        assert(pip.simplepicp(square, edgeSquare) == true);
        assert(pip.simplepicp(square, outsideSquare) == false);

        // Binary search convex test (integer) - requires convex CCW polygon
        assert(pip.binarypicp(square, insideSquare) == true);
        assert(pip.binarypicp(square, edgeSquare) == true);
        assert(pip.binarypicp(square, outsideSquare) == false);
    }

    {
        // ----------------------
        // Floating-point pentagon (CCW)
        // ----------------------
        FPolygon fPentagon = {
            {0.0, 0.0}, {4.0, 0.0}, {5.0, 3.0}, {2.0, 5.0}, {-1.0, 2.5}}; // CCW pentagon

        FPoint fInside{2.5, 2.0};  // Strictly inside
        FPoint fEdge{4.5, 1.5};    // On edge between {4,0} and {5,3}
        FPoint fVertex{0.0, 0.0};  // First vertex
        FPoint fOutside{6.0, 3.0}; // Clearly outside

        // Winding number (floating, CCW)
        assert(pip.wnpip(fPentagon, fInside, true) == true);
        assert(pip.wnpip(fPentagon, fEdge, true) == true);   // Edge counted as inside
        assert(pip.wnpip(fPentagon, fVertex, true) == true); // Vertex counted as inside
        assert(pip.wnpip(fPentagon, fOutside, true) == false);

        // Simple convex orientation (floating)
        assert(pip.simplepicp(fPentagon, fInside) == true);
        assert(pip.simplepicp(fPentagon, fEdge) == true);
        assert(pip.simplepicp(fPentagon, fVertex) == true);
        assert(pip.simplepicp(fPentagon, fOutside) == false);

        // Binary search convex test (floating)
        assert(pip.binarypicp(fPentagon, fInside) == true);
        assert(pip.binarypicp(fPentagon, fEdge) == true);
        assert(pip.binarypicp(fPentagon, fVertex) == true);
        assert(pip.binarypicp(fPentagon, fOutside) == false);
    }

    {
        // ----------------------
        // Concave polygon test (CCW "L" shape) - integer and floating-point
        // ----------------------
        // Integer version
        Polygon concaveInt = {{0, 0}, {4, 0}, {4, 2}, {2, 2}, {2, 4}, {0, 4}}; // CCW
        Point insideInt{1, 3};    // Inside vertical leg
        Point outsideInt{3, 3};   // Outside in "bite"
        Point inTopInt{4, 1};     // Inside top section
        Point onSegmentInt{2, 2}; // On joint

        // Ray-casting
        assert(pip.rcpip(concaveInt, insideInt) == true);
        assert(pip.rcpip(concaveInt, inTopInt) == true);
        assert(pip.rcpip(concaveInt, outsideInt) == false);
        assert(pip.rcpip(concaveInt, onSegmentInt) == true);

        // Winding number (integer, CCW)
        assert(pip.wnpip(concaveInt, insideInt, true) == true);
        assert(pip.wnpip(concaveInt, inTopInt, true) == true);
        assert(pip.wnpip(concaveInt, outsideInt, true) == false);
        assert(pip.wnpip(concaveInt, onSegmentInt, true) == true);

        // ----------------------
        // Floating-point version
        // ----------------------
        FPolygon concaveFloat = {{0.0, 0.0}, {4.0, 0.0}, {4.0, 2.0},
                                 {2.0, 2.0}, {2.0, 4.0}, {0.0, 4.0}}; // CCW
        FPoint insideF{1.0, 3.0};
        FPoint outsideF{3.0, 3.0};
        FPoint inTopF{4.0, 1.0};
        FPoint onSegmentF{2.0, 2.0};

        // Floating-point winding number
        assert(pip.wnpip(concaveFloat, insideF, true) == true);
        assert(pip.wnpip(concaveFloat, inTopF, true) == true);
        assert(pip.wnpip(concaveFloat, outsideF, true) == false);
        assert(pip.wnpip(concaveFloat, onSegmentF, true) == true);
    }

    {
        // ----------------------
        // Floating-point triangle (CCW and CW)
        // ----------------------
        FPolygon fTriCCW = {{0.0, 0.0}, {5.0, 0.0}, {2.5, 5.0}}; // CCW triangle
        FPoint fInside{2.5, 1.0};
        FPoint fEdge{2.5, 0.0}; // on base edge
        FPoint fOut{5.1, 0.0};

        // CCW polygon
        assert(pip.wnpip(fTriCCW, fInside, true) == true);
        assert(pip.wnpip(fTriCCW, fEdge, true) == true);
        assert(pip.wnpip(fTriCCW, fOut, true) == false);

        // Reverse polygon to make CW
        FPolygon fTriCW = {fTriCCW.rbegin(), fTriCCW.rend()};

        // CW polygon, ccw=false
        assert(pip.wnpip(fTriCW, fInside, false) == true);
        assert(pip.wnpip(fTriCW, fEdge, false) == true);
        assert(pip.wnpip(fTriCW, fOut, false) == false);
    }

    {
        // ----------------------
        // Self-intersecting polygon - integer
        // CCW and CW versions
        // ----------------------
        Polygon bowtieCCW = {{0, 0}, {4, 0}, {2, 2}, {4, 4}, {0, 4}, {2, 2}}; // CCW
        Polygon bowtieCW = {bowtieCCW.rbegin(), bowtieCCW.rend()};            // CW

        // Test points
        Point insideTop{3, 3};
        Point insideBottom{1, 1};
        Point outsideTop{0, 5};
        Point outsideBottom{1, -1};
        Point onEdge{2, 2};   // Intersection point (center)
        Point onVertex{0, 0}; // First vertex

        // --- CCW polygon ---
        assert(pip.wnpip(bowtieCCW, insideTop, true) == true);
        assert(pip.wnpip(bowtieCCW, insideBottom, true) == true);
        assert(pip.wnpip(bowtieCCW, outsideTop, true) == false);
        assert(pip.wnpip(bowtieCCW, outsideBottom, true) == false);
        assert(pip.wnpip(bowtieCCW, onEdge, true) ==
               true); // Intersection counted as inside
        assert(pip.wnpip(bowtieCCW, onVertex, true) == true); // vertex counted as inside

        // --- CW polygon ---
        assert(pip.wnpip(bowtieCW, insideTop, false) == true);
        assert(pip.wnpip(bowtieCW, insideBottom, false) == true);
        assert(pip.wnpip(bowtieCW, outsideTop, false) == false);
        assert(pip.wnpip(bowtieCW, outsideBottom, false) == false);
        assert(pip.wnpip(bowtieCW, onEdge, false) == true);
        assert(pip.wnpip(bowtieCW, onVertex, false) == true);

        // Floating-point version
        FPolygon fLoopCCW = {{0.0, 0.0}, {4.0, 0.0}, {4.0, 4.0}, {0.0, 4.0},
                             {0.0, 1.0}, {1.0, 1.0}, {1.0, 2.0}, {0.0, 2.0}};
        FPolygon fLoopCW(fLoopCCW.rbegin(), fLoopCCW.rend());

        FPoint fInsideTop{3.99, 3.99};
        FPoint fOutsideTop{2.00, 4.01};
        FPoint fInsideBottom{0.01, 0.01};
        FPoint fOutsideBottom{1.00, -0.01};
        FPoint fOutsideLeft{-0.01, 2.00};
        FPoint fOutsideRight{4.01, 2.00};
        FPoint fOnEdge{4.00, 2.00};
        FPoint fOnVertex{0.00, 0.00};
        FPoint fInLoop{1.01, 1.5};

        // Winding number floating-point, CCW
        assert(pip.wnpip(fLoopCCW, fInsideTop, true) == true);
        assert(pip.wnpip(fLoopCCW, fInsideBottom, true) == true);
        assert(pip.wnpip(fLoopCCW, fOutsideTop, true) == false);
        assert(pip.wnpip(fLoopCCW, fOutsideBottom, true) == false);
        assert(pip.wnpip(fLoopCCW, fOutsideLeft, true) == false);
        assert(pip.wnpip(fLoopCCW, fOutsideRight, true) == false);
        assert(pip.wnpip(fLoopCCW, fOnEdge, true) == true);
        assert(pip.wnpip(fLoopCCW, fOnVertex, true) == true);
        assert(pip.wnpip(fLoopCCW, fInLoop, true) == true);

        // Winding number floating-point, CW
        assert(pip.wnpip(fLoopCW, fInsideTop, false) == true);
        assert(pip.wnpip(fLoopCW, fInsideBottom, false) == true);
        assert(pip.wnpip(fLoopCW, fOutsideTop, false) == false);
        assert(pip.wnpip(fLoopCW, fOutsideBottom, false) == false);
        assert(pip.wnpip(fLoopCW, fOutsideLeft, false) == false);
        assert(pip.wnpip(fLoopCW, fOutsideRight, false) == false);
        assert(pip.wnpip(fLoopCW, fOnEdge, false) == true);
        assert(pip.wnpip(fLoopCW, fOnVertex, false) == true);
        assert(pip.wnpip(fLoopCW, fInLoop, false) == true);

        // Todo: Test overlapping polygons.
    }

    {
        // ----------------------
        // Hexagon quick test (flat-top regular hexagon centred at origin)
        // ----------------------
        FPoint hexCenter{0.0, 0.0};
        double s = 2.0; // Side length parameter used by pirhexagon
        // Center
        assert(pip.pirhexagon({0.0, 0.0}, hexCenter, s) == true);
        // Point on flat-top edge (s, 0)
        assert(pip.pirhexagon({s, 0.0}, hexCenter, s) == true);
        // Outside
        assert(pip.pirhexagon({3.0, 0.0}, hexCenter, s) == false);
        // Near corner but inside
        assert(pip.pirhexagon({1.0, 0.9}, hexCenter, s) == true);
    }

    {
        // ----------------------
        // Additional edge-case checks
        // ----------------------
        // Single-point polygon: any point equal to that point should be inside.
        Polygon single = {{42, 42}};
        assert(pip.rcpip(single, {42, 42}) == true);
        assert(pip.rcpip(single, {0, 0}) == false);

        // Two-point polygon (segment): points on the segment should be inside.
        Polygon seg = {{0, 0}, {10, 0}};
        assert(pip.rcpip(seg, {5, 0}) == true);
        assert(pip.rcpip(seg, {5, 1}) == false);

        // Large coordinates
        Polygon big = {{1000000, 1000000},
                       {2000000, 1000000},
                       {2000000, 2000000},
                       {1000000, 2000000}};
        assert(pip.rcpip(big, {1500000, 1500000}) == true);
        assert(pip.rcpip(big, {0, 0}) == false);
    }

    std::cout << "All PointInPolygon tests passed.\n";
    return 0;
}
