#include "intersections.hpp"
#include <gtest/gtest.h>

using namespace algo::geometry;

class IntersectionsTest : public ::testing::Test {
  protected:
    void SetUp() override {}
};

TEST_F(IntersectionsTest, ProperIntersection) {
    Segment a({0, 0}, {4, 4});
    Segment b({0, 4}, {4, 0});
    Point inter;

    ASSERT_TRUE(a.intersects(b, inter));
    EXPECT_TRUE(equal(inter.x, 2.0));
    EXPECT_TRUE(equal(inter.y, 2.0));
}

TEST_F(IntersectionsTest, TouchingAtEndpoint) {
    Segment a({0, 0}, {4, 0});
    Segment b({4, 0}, {4, 4});
    Point inter;

    ASSERT_TRUE(a.intersects(b, inter));
    EXPECT_TRUE(equal(inter.x, 4.0));
    EXPECT_TRUE(equal(inter.y, 0.0));
}

TEST_F(IntersectionsTest, ParallelNonIntersecting) {
    Segment a({0, 0}, {4, 0});
    Segment b({0, 1}, {4, 1});
    Point inter;

    EXPECT_FALSE(a.intersects(b, inter));
}

TEST_F(IntersectionsTest, Disjoint) {
    Segment a({0, 0}, {2, 0});
    Segment b({3, 1}, {3, -1});
    Point inter;

    EXPECT_FALSE(a.intersects(b, inter));
}

TEST_F(IntersectionsTest, ColinearOverlapping) {
    // Colinear overlapping (currently false)
    Segment a({0, 0}, {4, 0});
    Segment b({2, 0}, {6, 0});
    Point inter;

    EXPECT_FALSE(a.intersects(b, inter));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
