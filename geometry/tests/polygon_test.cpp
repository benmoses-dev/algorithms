#include "pointinpolygon.hpp"
#include <gtest/gtest.h>
#include <stdexcept>

using namespace algo::geometry;

class PointInPolygonTest : public ::testing::Test {
  protected:
    PointInPolygon pip;

    void SetUp() override {}
};

TEST_F(PointInPolygonTest, IntegerSquareCCW) {
    Polygon square = {{0, 0}, {10, 0}, {10, 10}, {0, 10}}; // CCW
    Point insideSquare{5, 5};
    Point edgeSquare{0, 5};
    Point vertexSquare{0, 0};
    Point outsideSquare{15, 5};
    Point cornerSquare{10, 10};

    // Ray casting (integer) should include edge/vertex as inside (onSegment -> true)
    EXPECT_TRUE(pip.rcpip(square, insideSquare));
    EXPECT_TRUE(pip.rcpip(square, edgeSquare));
    EXPECT_TRUE(pip.rcpip(square, vertexSquare));
    EXPECT_TRUE(pip.rcpip(square, cornerSquare));
    EXPECT_FALSE(pip.rcpip(square, outsideSquare));

    // Integer winding number (ccw = true for this CCW polygon)
    EXPECT_TRUE(pip.wnpip(square, insideSquare, true));
    EXPECT_TRUE(pip.wnpip(square, edgeSquare, true));
    EXPECT_FALSE(pip.wnpip(square, outsideSquare, true));

    // Simple convex orientation test (integer) - true for inside, true for edge/vertex
    EXPECT_TRUE(pip.simplepicp(square, insideSquare));
    EXPECT_TRUE(pip.simplepicp(square, edgeSquare));
    EXPECT_FALSE(pip.simplepicp(square, outsideSquare));

    // Binary search convex test (integer) - requires convex CCW polygon
    EXPECT_TRUE(pip.binarypicp(square, insideSquare));
    EXPECT_TRUE(pip.binarypicp(square, edgeSquare));
    EXPECT_FALSE(pip.binarypicp(square, outsideSquare));
}

TEST_F(PointInPolygonTest, FloatingPointPentagonCCW) {
    FPolygon fPentagon = {
        {0.0, 0.0}, {4.0, 0.0}, {5.0, 3.0}, {2.0, 5.0}, {-1.0, 2.5}}; // CCW pentagon

    FPoint fInside{2.5, 2.0};  // Strictly inside
    FPoint fEdge{4.5, 1.5};    // On edge between {4,0} and {5,3}
    FPoint fVertex{0.0, 0.0};  // First vertex
    FPoint fOutside{6.0, 3.0}; // Clearly outside

    // Winding number (floating, CCW)
    EXPECT_TRUE(pip.wnpip(fPentagon, fInside, true));
    EXPECT_TRUE(pip.wnpip(fPentagon, fEdge, true));   // Edge counted as inside
    EXPECT_TRUE(pip.wnpip(fPentagon, fVertex, true)); // Vertex counted as inside
    EXPECT_FALSE(pip.wnpip(fPentagon, fOutside, true));

    // Simple convex orientation (floating)
    EXPECT_TRUE(pip.simplepicp(fPentagon, fInside));
    EXPECT_TRUE(pip.simplepicp(fPentagon, fEdge));
    EXPECT_TRUE(pip.simplepicp(fPentagon, fVertex));
    EXPECT_FALSE(pip.simplepicp(fPentagon, fOutside));

    // Binary search convex test (floating)
    EXPECT_TRUE(pip.binarypicp(fPentagon, fInside));
    EXPECT_TRUE(pip.binarypicp(fPentagon, fEdge));
    EXPECT_TRUE(pip.binarypicp(fPentagon, fVertex));
    EXPECT_FALSE(pip.binarypicp(fPentagon, fOutside));
}

TEST_F(PointInPolygonTest, ConcavePolygonInteger) {
    // CCW "L" shape
    Polygon concaveInt = {{0, 0}, {4, 0}, {4, 2}, {2, 2}, {2, 4}, {0, 4}}; // CCW
    Point insideInt{1, 3};    // Inside vertical leg
    Point outsideInt{3, 3};   // Outside in "bite"
    Point inTopInt{4, 1};     // Inside top section
    Point onSegmentInt{2, 2}; // On joint

    // Ray-casting
    EXPECT_TRUE(pip.rcpip(concaveInt, insideInt));
    EXPECT_TRUE(pip.rcpip(concaveInt, inTopInt));
    EXPECT_FALSE(pip.rcpip(concaveInt, outsideInt));
    EXPECT_TRUE(pip.rcpip(concaveInt, onSegmentInt));

    // Winding number (integer, CCW)
    EXPECT_TRUE(pip.wnpip(concaveInt, insideInt, true));
    EXPECT_TRUE(pip.wnpip(concaveInt, inTopInt, true));
    EXPECT_FALSE(pip.wnpip(concaveInt, outsideInt, true));
    EXPECT_TRUE(pip.wnpip(concaveInt, onSegmentInt, true));
}

TEST_F(PointInPolygonTest, ConcavePolygonFloatingPoint) {
    FPolygon concaveFloat = {{0.0, 0.0}, {4.0, 0.0}, {4.0, 2.0},
                             {2.0, 2.0}, {2.0, 4.0}, {0.0, 4.0}}; // CCW
    FPoint insideF{1.0, 3.0};
    FPoint outsideF{3.0, 3.0};
    FPoint inTopF{4.0, 1.0};
    FPoint onSegmentF{2.0, 2.0};

    // Floating-point winding number
    EXPECT_TRUE(pip.wnpip(concaveFloat, insideF, true));
    EXPECT_TRUE(pip.wnpip(concaveFloat, inTopF, true));
    EXPECT_FALSE(pip.wnpip(concaveFloat, outsideF, true));
    EXPECT_TRUE(pip.wnpip(concaveFloat, onSegmentF, true));
}

TEST_F(PointInPolygonTest, FloatingPointTriangleCCWandCW) {
    FPolygon fTriCCW = {{0.0, 0.0}, {5.0, 0.0}, {2.5, 5.0}}; // CCW triangle
    FPoint fInside{2.5, 1.0};
    FPoint fEdge{2.5, 0.0}; // on base edge
    FPoint fOut{5.1, 0.0};

    // CCW polygon
    EXPECT_TRUE(pip.wnpip(fTriCCW, fInside, true));
    EXPECT_TRUE(pip.wnpip(fTriCCW, fEdge, true));
    EXPECT_FALSE(pip.wnpip(fTriCCW, fOut, true));

    // Reverse polygon to make CW
    FPolygon fTriCW = {fTriCCW.rbegin(), fTriCCW.rend()};

    // CW polygon, ccw=false
    EXPECT_TRUE(pip.wnpip(fTriCW, fInside, false));
    EXPECT_TRUE(pip.wnpip(fTriCW, fEdge, false));
    EXPECT_FALSE(pip.wnpip(fTriCW, fOut, false));
}

TEST_F(PointInPolygonTest, SelfIntersectingPolygonInteger) {
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
    EXPECT_TRUE(pip.wnpip(bowtieCCW, insideTop, true));
    EXPECT_TRUE(pip.wnpip(bowtieCCW, insideBottom, true));
    EXPECT_FALSE(pip.wnpip(bowtieCCW, outsideTop, true));
    EXPECT_FALSE(pip.wnpip(bowtieCCW, outsideBottom, true));
    EXPECT_TRUE(pip.wnpip(bowtieCCW, onEdge, true));   // Intersection counted as inside
    EXPECT_TRUE(pip.wnpip(bowtieCCW, onVertex, true)); // vertex counted as inside

    // --- CW polygon ---
    EXPECT_TRUE(pip.wnpip(bowtieCW, insideTop, false));
    EXPECT_TRUE(pip.wnpip(bowtieCW, insideBottom, false));
    EXPECT_FALSE(pip.wnpip(bowtieCW, outsideTop, false));
    EXPECT_FALSE(pip.wnpip(bowtieCW, outsideBottom, false));
    EXPECT_TRUE(pip.wnpip(bowtieCW, onEdge, false));
    EXPECT_TRUE(pip.wnpip(bowtieCW, onVertex, false));
}

TEST_F(PointInPolygonTest, SelfIntersectingPolygonFloatingPoint) {
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
    EXPECT_TRUE(pip.wnpip(fLoopCCW, fInsideTop, true));
    EXPECT_TRUE(pip.wnpip(fLoopCCW, fInsideBottom, true));
    EXPECT_FALSE(pip.wnpip(fLoopCCW, fOutsideTop, true));
    EXPECT_FALSE(pip.wnpip(fLoopCCW, fOutsideBottom, true));
    EXPECT_FALSE(pip.wnpip(fLoopCCW, fOutsideLeft, true));
    EXPECT_FALSE(pip.wnpip(fLoopCCW, fOutsideRight, true));
    EXPECT_TRUE(pip.wnpip(fLoopCCW, fOnEdge, true));
    EXPECT_TRUE(pip.wnpip(fLoopCCW, fOnVertex, true));
    EXPECT_TRUE(pip.wnpip(fLoopCCW, fInLoop, true));

    // Winding number floating-point, CW
    EXPECT_TRUE(pip.wnpip(fLoopCW, fInsideTop, false));
    EXPECT_TRUE(pip.wnpip(fLoopCW, fInsideBottom, false));
    EXPECT_FALSE(pip.wnpip(fLoopCW, fOutsideTop, false));
    EXPECT_FALSE(pip.wnpip(fLoopCW, fOutsideBottom, false));
    EXPECT_FALSE(pip.wnpip(fLoopCW, fOutsideLeft, false));
    EXPECT_FALSE(pip.wnpip(fLoopCW, fOutsideRight, false));
    EXPECT_TRUE(pip.wnpip(fLoopCW, fOnEdge, false));
    EXPECT_TRUE(pip.wnpip(fLoopCW, fOnVertex, false));
    EXPECT_TRUE(pip.wnpip(fLoopCW, fInLoop, false));
}

TEST_F(PointInPolygonTest, RegularHexagon) {
    // Flat-top regular hexagon centred at origin
    FPoint hexCenter{0.0, 0.0};
    double s = 2.0; // Side length parameter used by pirhexagon

    // Center
    EXPECT_TRUE(pip.pirhexagon({0.0, 0.0}, hexCenter, s));
    // Point on flat-top edge (s, 0)
    EXPECT_TRUE(pip.pirhexagon({s, 0.0}, hexCenter, s));
    // Outside
    EXPECT_FALSE(pip.pirhexagon({3.0, 0.0}, hexCenter, s));
    // Near corner but inside
    EXPECT_TRUE(pip.pirhexagon({1.0, 0.9}, hexCenter, s));
}

TEST_F(PointInPolygonTest, EdgeCases) {
    // Single-point polygon is not valid
    Polygon single = {{42, 42}};
    EXPECT_THROW({ pip.rcpip(single, {42, 42}); }, std::invalid_argument);

    // Two-point polygon (segment) is not valid
    Polygon seg = {{0, 0}, {10, 0}};
    EXPECT_THROW({ pip.rcpip(seg, {5, 0}); }, std::invalid_argument);

    // Large coordinates
    Polygon big = {
        {1000000, 1000000}, {2000000, 1000000}, {2000000, 2000000}, {1000000, 2000000}};
    EXPECT_TRUE(pip.rcpip(big, {1500000, 1500000}));
    EXPECT_FALSE(pip.rcpip(big, {0, 0}));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
