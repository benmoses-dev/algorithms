#include "segment_tree.hpp"
#include <gtest/gtest.h>
#include <limits>
#include <vector>

using namespace algo::ds;

// ============================================================================
// SegmentTreeSum Tests
// ============================================================================

class SegmentTreeSumTest : public ::testing::Test {
  protected:
    void SetUp() override {}
};

// Construction Tests
TEST_F(SegmentTreeSumTest, ConstructorBasic) {
    std::vector<ll> arr = {1, 2, 3, 4, 5};
    SegmentTreeSum tree(arr);

    EXPECT_EQ(tree.query(0, 0), 1);
    EXPECT_EQ(tree.query(0, 4), 15);
}

TEST_F(SegmentTreeSumTest, ConstructorEmptyThrows) {
    std::vector<ll> arr = {};
    EXPECT_THROW(SegmentTreeSum tree(arr), std::invalid_argument);
}

TEST_F(SegmentTreeSumTest, ConstructorSingleElement) {
    std::vector<ll> arr = {42};
    SegmentTreeSum tree(arr);

    EXPECT_EQ(tree.query(0, 0), 42);
}

TEST_F(SegmentTreeSumTest, ConstructorNegativeValues) {
    std::vector<ll> arr = {-5, 10, -3, 7, -2};
    SegmentTreeSum tree(arr);

    EXPECT_EQ(tree.query(0, 4), 7);
    EXPECT_EQ(tree.query(1, 3), 14);
}

// Range Update Tests
TEST_F(SegmentTreeSumTest, UpdateSingleElement) {
    std::vector<ll> arr = {1, 2, 3, 4, 5};
    SegmentTreeSum tree(arr);

    tree.update(2, 2, 10);
    EXPECT_EQ(tree.query(2, 2), 13); // 3 + 10
    EXPECT_EQ(tree.query(0, 4), 25); // 1+2+13+4+5
}

TEST_F(SegmentTreeSumTest, UpdateRange) {
    std::vector<ll> arr = {1, 2, 3, 4, 5};
    SegmentTreeSum tree(arr);

    tree.update(1, 3, 5);
    EXPECT_EQ(tree.query(0, 4), 30); // 1+7+8+9+5
    EXPECT_EQ(tree.query(1, 3), 24); // 7+8+9
}

TEST_F(SegmentTreeSumTest, UpdateFullRange) {
    std::vector<ll> arr = {1, 2, 3, 4, 5};
    SegmentTreeSum tree(arr);

    tree.update(0, 4, 10);
    EXPECT_EQ(tree.query(0, 4), 65); // 15 + 50
}

TEST_F(SegmentTreeSumTest, UpdateMultipleTimes) {
    std::vector<ll> arr = {1, 2, 3, 4, 5};
    SegmentTreeSum tree(arr);

    tree.update(1, 3, 2);
    tree.update(2, 4, 3);

    EXPECT_EQ(tree.query(2, 2), 8);  // 3 + 2 + 3
    EXPECT_EQ(tree.query(0, 4), 30); // 1+4+8+9+8
}

TEST_F(SegmentTreeSumTest, UpdateNegativeValue) {
    std::vector<ll> arr = {10, 10, 10, 10, 10};
    SegmentTreeSum tree(arr);

    tree.update(1, 3, -3);
    EXPECT_EQ(tree.query(0, 4), 41); // 10+7+7+7+10
}

TEST_F(SegmentTreeSumTest, UpdateOverlappingRanges) {
    std::vector<ll> arr = {1, 2, 3, 4, 5};
    SegmentTreeSum tree(arr);

    tree.update(0, 2, 5); // [6, 7, 8, 4, 5]
    tree.update(2, 4, 3); // [6, 7, 11, 7, 8]

    EXPECT_EQ(tree.query(2, 2), 11);
    EXPECT_EQ(tree.query(0, 4), 39);
}

TEST_F(SegmentTreeSumTest, UpdateOutOfBoundsThrows) {
    std::vector<ll> arr = {1, 2, 3, 4, 5};
    SegmentTreeSum tree(arr);

    EXPECT_THROW(tree.update(-1, 2, 5), std::out_of_range);
    EXPECT_THROW(tree.update(0, 5, 5), std::out_of_range);
    EXPECT_THROW(tree.update(3, 2, 5), std::out_of_range); // L > R
}

// Range Query Tests
TEST_F(SegmentTreeSumTest, QuerySingleElement) {
    std::vector<ll> arr = {1, 2, 3, 4, 5};
    SegmentTreeSum tree(arr);

    EXPECT_EQ(tree.query(0, 0), 1);
    EXPECT_EQ(tree.query(2, 2), 3);
    EXPECT_EQ(tree.query(4, 4), 5);
}

TEST_F(SegmentTreeSumTest, QueryPartialRange) {
    std::vector<ll> arr = {1, 2, 3, 4, 5};
    SegmentTreeSum tree(arr);

    EXPECT_EQ(tree.query(1, 3), 9);  // 2+3+4
    EXPECT_EQ(tree.query(0, 2), 6);  // 1+2+3
    EXPECT_EQ(tree.query(2, 4), 12); // 3+4+5
}

TEST_F(SegmentTreeSumTest, QueryFullRange) {
    std::vector<ll> arr = {1, 2, 3, 4, 5};
    SegmentTreeSum tree(arr);

    EXPECT_EQ(tree.query(0, 4), 15);
}

TEST_F(SegmentTreeSumTest, QueryAfterUpdate) {
    std::vector<ll> arr = {1, 2, 3, 4, 5};
    SegmentTreeSum tree(arr);

    tree.update(1, 3, 10);
    EXPECT_EQ(tree.query(1, 3), 39); // 12+13+14
    EXPECT_EQ(tree.query(0, 4), 45); // 1+12+13+14+5
}

TEST_F(SegmentTreeSumTest, QueryOutOfBoundsThrows) {
    std::vector<ll> arr = {1, 2, 3, 4, 5};
    SegmentTreeSum tree(arr);

    EXPECT_THROW(tree.query(-1, 2), std::out_of_range);
    EXPECT_THROW(tree.query(0, 5), std::out_of_range);
    EXPECT_THROW(tree.query(3, 2), std::out_of_range); // L > R
}

// Edge Cases
TEST_F(SegmentTreeSumTest, LargeValues) {
    std::vector<ll> arr = {1000000000LL, 1000000000LL, 1000000000LL};
    SegmentTreeSum tree(arr);

    EXPECT_EQ(tree.query(0, 2), 3000000000LL);
    tree.update(0, 2, 1000000000LL);
    EXPECT_EQ(tree.query(0, 2), 6000000000LL);
}

TEST_F(SegmentTreeSumTest, AllZeros) {
    std::vector<ll> arr(100, 0);
    SegmentTreeSum tree(arr);

    EXPECT_EQ(tree.query(0, 99), 0);
    tree.update(25, 75, 0);
    EXPECT_EQ(tree.query(0, 99), 0);
}

TEST_F(SegmentTreeSumTest, StressTestManyOperations) {
    std::vector<ll> arr(1000, 1);
    SegmentTreeSum tree(arr);

    // Many updates
    for (int i = 0; i < 100; i++) {
        tree.update(i * 10, i * 10 + 9, i);
    }

    // Verify some queries
    ll sum = tree.query(0, 999);
    EXPECT_GT(sum, 1000); // Should be much larger than initial sum
}

// ============================================================================
// SegmentTreeStats Tests
// ============================================================================

class SegmentTreeStatsTest : public ::testing::Test {
  protected:
    void SetUp() override {}
};

// Construction Tests
TEST_F(SegmentTreeStatsTest, ConstructorBasic) {
    std::vector<ll> arr = {5, 2, 8, 1, 9};
    SegmentTreeStats tree(arr);

    EXPECT_EQ(tree.queryMin(0, 4), 1);
    EXPECT_EQ(tree.queryMax(0, 4), 9);
    EXPECT_EQ(tree.queryGCD(0, 4), 1);
}

TEST_F(SegmentTreeStatsTest, ConstructorEmptyThrows) {
    std::vector<ll> arr = {};
    EXPECT_THROW(SegmentTreeStats tree(arr), std::invalid_argument);
}

TEST_F(SegmentTreeStatsTest, ConstructorSingleElement) {
    std::vector<ll> arr = {42};
    SegmentTreeStats tree(arr);

    EXPECT_EQ(tree.queryMin(0, 0), 42);
    EXPECT_EQ(tree.queryMax(0, 0), 42);
    EXPECT_EQ(tree.queryGCD(0, 0), 42);
}

TEST_F(SegmentTreeStatsTest, ConstructorNegativeValues) {
    std::vector<ll> arr = {-5, 10, -3, 7, -2};
    SegmentTreeStats tree(arr);

    EXPECT_EQ(tree.queryMin(0, 4), -5);
    EXPECT_EQ(tree.queryMax(0, 4), 10);
}

// Point Update Tests
TEST_F(SegmentTreeStatsTest, UpdatePointSingle) {
    std::vector<ll> arr = {5, 2, 8, 1, 9};
    SegmentTreeStats tree(arr);

    tree.updatePoint(2, 15);
    EXPECT_EQ(tree.queryMax(0, 4), 15);
    EXPECT_EQ(tree.queryMin(0, 4), 1);
}

TEST_F(SegmentTreeStatsTest, UpdatePointMultiple) {
    std::vector<ll> arr = {5, 2, 8, 1, 9};
    SegmentTreeStats tree(arr);

    tree.updatePoint(0, 10);
    tree.updatePoint(3, 20);
    tree.updatePoint(4, 3);

    EXPECT_EQ(tree.queryMin(0, 4), 2);
    EXPECT_EQ(tree.queryMax(0, 4), 20);
}

TEST_F(SegmentTreeStatsTest, UpdatePointOutOfBoundsThrows) {
    std::vector<ll> arr = {1, 2, 3, 4, 5};
    SegmentTreeStats tree(arr);

    EXPECT_THROW(tree.updatePoint(-1, 10), std::out_of_range);
    EXPECT_THROW(tree.updatePoint(5, 10), std::out_of_range);
}

// Min Query Tests
TEST_F(SegmentTreeStatsTest, QueryMinSingleElement) {
    std::vector<ll> arr = {5, 2, 8, 1, 9};
    SegmentTreeStats tree(arr);

    EXPECT_EQ(tree.queryMin(2, 2), 8);
}

TEST_F(SegmentTreeStatsTest, QueryMinPartialRange) {
    std::vector<ll> arr = {5, 2, 8, 1, 9};
    SegmentTreeStats tree(arr);

    EXPECT_EQ(tree.queryMin(0, 2), 2);
    EXPECT_EQ(tree.queryMin(2, 4), 1);
    EXPECT_EQ(tree.queryMin(1, 3), 1);
}

TEST_F(SegmentTreeStatsTest, QueryMinFullRange) {
    std::vector<ll> arr = {5, 2, 8, 1, 9};
    SegmentTreeStats tree(arr);

    EXPECT_EQ(tree.queryMin(0, 4), 1);
}

TEST_F(SegmentTreeStatsTest, QueryMinAfterUpdate) {
    std::vector<ll> arr = {5, 2, 8, 1, 9};
    SegmentTreeStats tree(arr);

    tree.updatePoint(3, 15); // Change min from 1 to 2
    EXPECT_EQ(tree.queryMin(0, 4), 2);

    tree.updatePoint(1, 0); // New min is 0
    EXPECT_EQ(tree.queryMin(0, 4), 0);
}

TEST_F(SegmentTreeStatsTest, QueryMinNegativeValues) {
    std::vector<ll> arr = {-5, 10, -3, 7, -2};
    SegmentTreeStats tree(arr);

    EXPECT_EQ(tree.queryMin(0, 4), -5);
    EXPECT_EQ(tree.queryMin(1, 4), -3);
}

// Max Query Tests
TEST_F(SegmentTreeStatsTest, QueryMaxSingleElement) {
    std::vector<ll> arr = {5, 2, 8, 1, 9};
    SegmentTreeStats tree(arr);

    EXPECT_EQ(tree.queryMax(2, 2), 8);
}

TEST_F(SegmentTreeStatsTest, QueryMaxPartialRange) {
    std::vector<ll> arr = {5, 2, 8, 1, 9};
    SegmentTreeStats tree(arr);

    EXPECT_EQ(tree.queryMax(0, 2), 8);
    EXPECT_EQ(tree.queryMax(2, 4), 9);
    EXPECT_EQ(tree.queryMax(1, 3), 8);
}

TEST_F(SegmentTreeStatsTest, QueryMaxFullRange) {
    std::vector<ll> arr = {5, 2, 8, 1, 9};
    SegmentTreeStats tree(arr);

    EXPECT_EQ(tree.queryMax(0, 4), 9);
}

TEST_F(SegmentTreeStatsTest, QueryMaxAfterUpdate) {
    std::vector<ll> arr = {5, 2, 8, 1, 9};
    SegmentTreeStats tree(arr);

    tree.updatePoint(4, 100);
    EXPECT_EQ(tree.queryMax(0, 4), 100);

    tree.updatePoint(4, 3);
    EXPECT_EQ(tree.queryMax(0, 4), 8);
}

// GCD Query Tests
TEST_F(SegmentTreeStatsTest, QueryGCDSingleElement) {
    std::vector<ll> arr = {12, 18, 24, 30, 36};
    SegmentTreeStats tree(arr);

    EXPECT_EQ(tree.queryGCD(2, 2), 24);
}

TEST_F(SegmentTreeStatsTest, QueryGCDPartialRange) {
    std::vector<ll> arr = {12, 18, 24, 30, 36};
    SegmentTreeStats tree(arr);

    EXPECT_EQ(tree.queryGCD(0, 2), 6); // gcd(12, 18, 24)
    EXPECT_EQ(tree.queryGCD(2, 4), 6); // gcd(24, 30, 36)
    EXPECT_EQ(tree.queryGCD(1, 3), 6); // gcd(18, 24, 30)
}

TEST_F(SegmentTreeStatsTest, QueryGCDFullRange) {
    std::vector<ll> arr = {12, 18, 24, 30, 36};
    SegmentTreeStats tree(arr);

    EXPECT_EQ(tree.queryGCD(0, 4), 6);
}

TEST_F(SegmentTreeStatsTest, QueryGCDCoprime) {
    std::vector<ll> arr = {7, 11, 13};
    SegmentTreeStats tree(arr);

    EXPECT_EQ(tree.queryGCD(0, 2), 1); // Coprime numbers
}

TEST_F(SegmentTreeStatsTest, QueryGCDAfterUpdate) {
    std::vector<ll> arr = {12, 18, 24};
    SegmentTreeStats tree(arr);

    EXPECT_EQ(tree.queryGCD(0, 2), 6);

    tree.updatePoint(1, 15); // Change to [12, 15, 24]
    EXPECT_EQ(tree.queryGCD(0, 2), 3);
}

TEST_F(SegmentTreeStatsTest, QueryGCDWithZero) {
    std::vector<ll> arr = {12, 0, 18};
    SegmentTreeStats tree(arr);

    EXPECT_EQ(tree.queryGCD(0, 2), 6); // gcd with 0 is the other number
}

// Query Out of Bounds Tests
TEST_F(SegmentTreeStatsTest, QueryOutOfBoundsThrows) {
    std::vector<ll> arr = {1, 2, 3, 4, 5};
    SegmentTreeStats tree(arr);

    EXPECT_THROW(tree.queryMin(-1, 2), std::out_of_range);
    EXPECT_THROW(tree.queryMax(0, 5), std::out_of_range);
    EXPECT_THROW(tree.queryGCD(3, 2), std::out_of_range); // L > R
}

// Combined Operations Tests
TEST_F(SegmentTreeStatsTest, CombinedOperations) {
    std::vector<ll> arr = {10, 20, 30, 40, 50};
    SegmentTreeStats tree(arr);

    EXPECT_EQ(tree.queryMin(0, 4), 10);
    EXPECT_EQ(tree.queryMax(0, 4), 50);
    EXPECT_EQ(tree.queryGCD(0, 4), 10);

    tree.updatePoint(2, 15); // [10, 20, 15, 40, 50]

    EXPECT_EQ(tree.queryMin(0, 4), 10);
    EXPECT_EQ(tree.queryMax(0, 4), 50);
    EXPECT_EQ(tree.queryGCD(0, 4), 5);
}

// Edge Cases
TEST_F(SegmentTreeStatsTest, AllSameValues) {
    std::vector<ll> arr(100, 42);
    SegmentTreeStats tree(arr);

    EXPECT_EQ(tree.queryMin(0, 99), 42);
    EXPECT_EQ(tree.queryMax(0, 99), 42);
    EXPECT_EQ(tree.queryGCD(0, 99), 42);
}

TEST_F(SegmentTreeStatsTest, PowersOfTwo) {
    std::vector<ll> arr = {2, 4, 8, 16, 32};
    SegmentTreeStats tree(arr);

    EXPECT_EQ(tree.queryMin(0, 4), 2);
    EXPECT_EQ(tree.queryMax(0, 4), 32);
    EXPECT_EQ(tree.queryGCD(0, 4), 2);
}

TEST_F(SegmentTreeStatsTest, StressTestManyUpdates) {
    std::vector<ll> arr(1000, 100);
    SegmentTreeStats tree(arr);

    // Many updates
    for (int i = 0; i < 500; i++) {
        tree.updatePoint(i, i + 1);
    }

    EXPECT_EQ(tree.queryMin(0, 499), 1);
    EXPECT_EQ(tree.queryMax(0, 499), 500);
    EXPECT_EQ(tree.queryMin(500, 999), 100);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
