#include "polygonarea.hpp"
#include <cmath>
#include <gtest/gtest.h>

using namespace algo::geometry;

class PolygonAreaTest : public ::testing::Test {
  protected:
    static constexpr ld EPS = 1e-9;

    void SetUp() override {}

    bool nearlyEqual(ld a, ld b) { return fabsl(a - b) < EPS; }
};

TEST_F(PolygonAreaTest, RightTriangle) {
    // Right triangle (3-4-5 triangle, area should be 6)
    Polygon tri = {{0, 0}, {3, 0}, {0, 4}};
    ld area = irregularArea(tri);
    EXPECT_TRUE(nearlyEqual(area, 6.0));
}

TEST_F(PolygonAreaTest, Square) {
    // Square with side length 2 (area = 4)
    Polygon square = {{0, 0}, {2, 0}, {2, 2}, {0, 2}};
    ld area = irregularArea(square);
    EXPECT_TRUE(nearlyEqual(area, 4.0));
}

TEST_F(PolygonAreaTest, ConcavePolygon) {
    // Concave polygon (arrow shape)
    Polygon concave = {{0, 0}, {4, 0}, {4, 2}, {2, 1}, {0, 2}};
    // Can compute manually: area = 6
    ld area = irregularArea(concave);
    EXPECT_TRUE(nearlyEqual(area, 6.0));
}

TEST_F(PolygonAreaTest, RegularTriangle) {
    // Regular triangle (equilateral, side = 2)
    ld area = regularArea(3, 2);
    // Exact: sqrt(3)
    EXPECT_TRUE(nearlyEqual(area, std::sqrt(3.0L)));
}

TEST_F(PolygonAreaTest, RegularSquare) {
    // Regular square (side = 2)
    ld area = regularArea(4, 2);
    EXPECT_TRUE(nearlyEqual(area, 4.0));
}

TEST_F(PolygonAreaTest, RegularHexagon) {
    // Regular hexagon (side = 2)
    ld area = regularArea(6, 2);
    // (3 * sqrt(3) / 2) * side^2 = 2 * 3 * sqrt(3) = 10.3923
    ld expected = (3.0L * std::sqrt(3.0L) / 2.0L) * 4.0L;
    EXPECT_TRUE(nearlyEqual(area, expected));
}

TEST_F(PolygonAreaTest, DegeneratePolygons) {
    // Degenerate: fewer than 3 points
    Polygon line = {{0, 0}, {1, 1}};
    ld area = irregularArea(line);
    EXPECT_TRUE(nearlyEqual(area, 0.0));
    EXPECT_TRUE(nearlyEqual(regularArea(2, 5), 0.0));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
