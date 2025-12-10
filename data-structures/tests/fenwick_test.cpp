#include "fenwick.hpp"
#include <gtest/gtest.h>
#include <stdexcept>
#include <vector>

using namespace algo::ds;

// ============================================================================
// 1D Fenwick Tree Tests
// ============================================================================

class FenwickTreeTest : public ::testing::Test {
  protected:
    void SetUp() override {}
};

// Basic Construction Tests
TEST_F(FenwickTreeTest, ConstructorWithSize) {
    FenwickTree ft(10);
    EXPECT_EQ(ft.size(), 10);
}

TEST_F(FenwickTreeTest, ConstructorWithZeroSize) {
    FenwickTree ft(0);
    EXPECT_EQ(ft.size(), 0);
}

TEST_F(FenwickTreeTest, ConstructorWithNegativeSizeThrows) {
    EXPECT_THROW({ FenwickTree(-1); }, std::invalid_argument);
    EXPECT_THROW({ FenwickTree(-100); }, std::invalid_argument);
}

TEST_F(FenwickTreeTest, ConstructorFromVector) {
    std::vector<ll> data = {1, 2, 3, 4, 5};
    FenwickTree ft(data);

    EXPECT_EQ(ft.size(), 5);
    EXPECT_EQ(ft.get(0), 1);
    EXPECT_EQ(ft.get(1), 2);
    EXPECT_EQ(ft.get(2), 3);
    EXPECT_EQ(ft.get(3), 4);
    EXPECT_EQ(ft.get(4), 5);
}

TEST_F(FenwickTreeTest, ConstructorFromEmptyVector) {
    std::vector<ll> data = {};
    FenwickTree ft(data);
    EXPECT_EQ(ft.size(), 0);
}

// Add Operation Tests
TEST_F(FenwickTreeTest, AddSingleElement) {
    FenwickTree ft(5);
    ft.add(2, 10);

    EXPECT_EQ(ft.get(2), 10);
    EXPECT_EQ(ft.get(0), 0);
    EXPECT_EQ(ft.get(1), 0);
}

TEST_F(FenwickTreeTest, AddMultipleElements) {
    FenwickTree ft(5);
    ft.add(0, 5);
    ft.add(1, 3);
    ft.add(2, 7);

    EXPECT_EQ(ft.get(0), 5);
    EXPECT_EQ(ft.get(1), 3);
    EXPECT_EQ(ft.get(2), 7);
}

TEST_F(FenwickTreeTest, AddNegativeValue) {
    FenwickTree ft(5);
    ft.add(2, 10);
    ft.add(2, -3);

    EXPECT_EQ(ft.get(2), 7);
}

TEST_F(FenwickTreeTest, AddToSameIndexMultipleTimes) {
    FenwickTree ft(5);
    ft.add(2, 5);
    ft.add(2, 3);
    ft.add(2, -2);

    EXPECT_EQ(ft.get(2), 6);
}

TEST_F(FenwickTreeTest, AddOutOfBoundsThrows) {
    FenwickTree ft(5);
    EXPECT_THROW(ft.add(-1, 10), std::out_of_range);
    EXPECT_THROW(ft.add(5, 10), std::out_of_range);
    EXPECT_THROW(ft.add(100, 10), std::out_of_range);
}

// Set Operation Tests
TEST_F(FenwickTreeTest, SetElement) {
    std::vector<ll> data = {1, 2, 3, 4, 5};
    FenwickTree ft(data);

    ft.set(2, 10);
    EXPECT_EQ(ft.get(2), 10);
}

TEST_F(FenwickTreeTest, SetMultipleElements) {
    std::vector<ll> data = {1, 2, 3, 4, 5};
    FenwickTree ft(data);

    ft.set(0, 10);
    ft.set(2, 20);
    ft.set(4, 30);

    EXPECT_EQ(ft.get(0), 10);
    EXPECT_EQ(ft.get(2), 20);
    EXPECT_EQ(ft.get(4), 30);
}

TEST_F(FenwickTreeTest, SetToZero) {
    std::vector<ll> data = {1, 2, 3, 4, 5};
    FenwickTree ft(data);

    ft.set(2, 0);
    EXPECT_EQ(ft.get(2), 0);
}

TEST_F(FenwickTreeTest, SetNegativeValue) {
    FenwickTree ft(5);
    ft.set(2, -15);
    EXPECT_EQ(ft.get(2), -15);
}

// Sum Operation Tests
TEST_F(FenwickTreeTest, SumFromZero) {
    std::vector<ll> data = {1, 2, 3, 4, 5};
    FenwickTree ft(data);

    EXPECT_EQ(ft.prefixSum(0), 1);
    EXPECT_EQ(ft.prefixSum(1), 3);
    EXPECT_EQ(ft.prefixSum(2), 6);
    EXPECT_EQ(ft.prefixSum(3), 10);
    EXPECT_EQ(ft.prefixSum(4), 15);
}

TEST_F(FenwickTreeTest, SumAfterUpdates) {
    std::vector<ll> data = {1, 2, 3, 4, 5};
    FenwickTree ft(data);

    ft.add(2, 10);
    EXPECT_EQ(ft.prefixSum(2), 16); // 1 + 2 + 13
    EXPECT_EQ(ft.prefixSum(4), 25); // 1 + 2 + 13 + 4 + 5
}

TEST_F(FenwickTreeTest, SumOutOfBoundsThrows) {
    FenwickTree ft(5);
    EXPECT_THROW(ft.prefixSum(-1), std::out_of_range);
    EXPECT_THROW(ft.prefixSum(5), std::out_of_range);
}

// Range Sum Tests
TEST_F(FenwickTreeTest, RangeSumBasic) {
    std::vector<ll> data = {1, 2, 3, 4, 5};
    FenwickTree ft(data);

    EXPECT_EQ(ft.rangeSum(0, 4), 15); // All elements
    EXPECT_EQ(ft.rangeSum(1, 3), 9);  // 2 + 3 + 4
    EXPECT_EQ(ft.rangeSum(2, 2), 3);  // Single element
}

TEST_F(FenwickTreeTest, RangeSumFromZero) {
    std::vector<ll> data = {1, 2, 3, 4, 5};
    FenwickTree ft(data);

    EXPECT_EQ(ft.rangeSum(0, 2), 6); // 1 + 2 + 3
    EXPECT_EQ(ft.rangeSum(0, 0), 1); // First element
}

TEST_F(FenwickTreeTest, RangeSumInvalidRange) {
    std::vector<ll> data = {1, 2, 3, 4, 5};
    FenwickTree ft(data);

    EXPECT_EQ(ft.rangeSum(3, 1), 0); // l > r returns 0
}

TEST_F(FenwickTreeTest, RangeSumOutOfBoundsThrows) {
    FenwickTree ft(5);
    EXPECT_THROW(ft.rangeSum(0, 5), std::out_of_range);
    EXPECT_THROW(ft.rangeSum(0, 10), std::out_of_range);
}

TEST_F(FenwickTreeTest, RangeSumWithNegativeValues) {
    std::vector<ll> data = {-5, 10, -3, 7, -2};
    FenwickTree ft(data);

    EXPECT_EQ(ft.rangeSum(0, 4), 7);
    EXPECT_EQ(ft.rangeSum(1, 3), 14); // 10 + (-3) + 7
}

// Get Operation Tests
TEST_F(FenwickTreeTest, GetElement) {
    std::vector<ll> data = {1, 2, 3, 4, 5};
    FenwickTree ft(data);

    EXPECT_EQ(ft.get(0), 1);
    EXPECT_EQ(ft.get(4), 5);
}

TEST_F(FenwickTreeTest, GetAfterAdd) {
    FenwickTree ft(5);
    ft.add(2, 10);
    ft.add(2, 5);

    EXPECT_EQ(ft.get(2), 15);
}

TEST_F(FenwickTreeTest, GetAfterSet) {
    std::vector<ll> data = {1, 2, 3, 4, 5};
    FenwickTree ft(data);

    ft.set(3, 100);
    EXPECT_EQ(ft.get(3), 100);
}

// Large Value Tests
TEST_F(FenwickTreeTest, LargeValues) {
    FenwickTree ft(3);
    const ll large = 1000000000LL;

    ft.add(0, large);
    ft.add(1, large);
    ft.add(2, large);

    EXPECT_EQ(ft.prefixSum(2), 3 * large);
}

TEST_F(FenwickTreeTest, LargeNegativeValues) {
    FenwickTree ft(3);
    const ll large = -1000000000LL;

    ft.add(0, large);
    ft.add(1, large);
    ft.add(2, large);

    EXPECT_EQ(ft.prefixSum(2), 3 * large);
}

// Edge Cases
TEST_F(FenwickTreeTest, AllZeros) {
    FenwickTree ft(5);

    EXPECT_EQ(ft.prefixSum(4), 0);
    EXPECT_EQ(ft.rangeSum(0, 4), 0);
}

TEST_F(FenwickTreeTest, SingleElementTree) {
    std::vector<ll> data = {42};
    FenwickTree ft(data);

    EXPECT_EQ(ft.get(0), 42);
    EXPECT_EQ(ft.prefixSum(0), 42);
    EXPECT_EQ(ft.rangeSum(0, 0), 42);
}

TEST_F(FenwickTreeTest, AlternatingValues) {
    std::vector<ll> data = {1, -1, 1, -1, 1, -1};
    FenwickTree ft(data);

    EXPECT_EQ(ft.prefixSum(5), 0);
    EXPECT_EQ(ft.rangeSum(0, 3), 0);
    EXPECT_EQ(ft.rangeSum(0, 4), 1);
}

TEST_F(FenwickTreeTest, StressTestManyOperations) {
    FenwickTree ft(100);

    // Add values
    for (int i = 0; i < 100; i++) {
        ft.add(i, i + 1);
    }

    // Verify sums
    EXPECT_EQ(ft.prefixSum(99), 5050); // Sum of 1 to 100

    // Modify and verify
    for (int i = 0; i < 50; i++) {
        ft.add(i, -1);
    }

    EXPECT_EQ(ft.prefixSum(49), 1225); // Reduced by 50
}

// ============================================================================
// 2D Fenwick Tree Tests
// ============================================================================

class FenwickTree2DTest : public ::testing::Test {
  protected:
    void SetUp() override {
        // Fresh setup for each test if needed
    }
};

// Basic Construction Tests
TEST_F(FenwickTree2DTest, ConstructorWithDimensions) {
    FenwickTree2D ft(5, 10);
    EXPECT_EQ(ft.rows(), 5);
    EXPECT_EQ(ft.cols(), 10);
}

TEST_F(FenwickTree2DTest, ConstructorWithZeroDimensions) {
    FenwickTree2D ft(0, 0);
    EXPECT_EQ(ft.rows(), 0);
    EXPECT_EQ(ft.cols(), 0);
}

TEST_F(FenwickTree2DTest, ConstructorWithNegativeDimensionsThrows) {
    EXPECT_THROW(FenwickTree2D(-1, 5), std::invalid_argument);
    EXPECT_THROW(FenwickTree2D(5, -1), std::invalid_argument);
    EXPECT_THROW(FenwickTree2D(-1, -1), std::invalid_argument);
}

TEST_F(FenwickTree2DTest, ConstructorFrom2DVector) {
    std::vector<std::vector<ll>> data = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    FenwickTree2D ft(data);

    EXPECT_EQ(ft.rows(), 3);
    EXPECT_EQ(ft.cols(), 3);
    EXPECT_EQ(ft.get(0, 0), 1);
    EXPECT_EQ(ft.get(1, 1), 5);
    EXPECT_EQ(ft.get(2, 2), 9);
}

TEST_F(FenwickTree2DTest, ConstructorFromEmpty2DVector) {
    std::vector<std::vector<ll>> data = {};
    FenwickTree2D ft(data);
    EXPECT_EQ(ft.rows(), 0);
    EXPECT_EQ(ft.cols(), 0);
}

// Add Operation Tests
TEST_F(FenwickTree2DTest, AddSingleElement) {
    FenwickTree2D ft(3, 3);
    ft.add(1, 1, 10);

    EXPECT_EQ(ft.get(1, 1), 10);
    EXPECT_EQ(ft.get(0, 0), 0);
}

TEST_F(FenwickTree2DTest, AddMultipleElements) {
    FenwickTree2D ft(3, 3);
    ft.add(0, 0, 5);
    ft.add(1, 1, 10);
    ft.add(2, 2, 15);

    EXPECT_EQ(ft.get(0, 0), 5);
    EXPECT_EQ(ft.get(1, 1), 10);
    EXPECT_EQ(ft.get(2, 2), 15);
}

TEST_F(FenwickTree2DTest, AddNegativeValue) {
    FenwickTree2D ft(3, 3);
    ft.add(1, 1, 10);
    ft.add(1, 1, -3);

    EXPECT_EQ(ft.get(1, 1), 7);
}

TEST_F(FenwickTree2DTest, AddOutOfBoundsThrows) {
    FenwickTree2D ft(3, 3);
    EXPECT_THROW(ft.add(-1, 0, 10), std::out_of_range);
    EXPECT_THROW(ft.add(0, -1, 10), std::out_of_range);
    EXPECT_THROW(ft.add(3, 0, 10), std::out_of_range);
    EXPECT_THROW(ft.add(0, 3, 10), std::out_of_range);
}

// Set Operation Tests
TEST_F(FenwickTree2DTest, SetElement) {
    std::vector<std::vector<ll>> data = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    FenwickTree2D ft(data);

    ft.set(1, 1, 100);
    EXPECT_EQ(ft.get(1, 1), 100);
}

TEST_F(FenwickTree2DTest, SetToZero) {
    std::vector<std::vector<ll>> data = {{1, 2, 3}, {4, 5, 6}};
    FenwickTree2D ft(data);

    ft.set(0, 0, 0);
    EXPECT_EQ(ft.get(0, 0), 0);
}

// Sum Operation Tests
TEST_F(FenwickTree2DTest, SumFromOrigin) {
    std::vector<std::vector<ll>> data = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    FenwickTree2D ft(data);

    EXPECT_EQ(ft.prefixSum(0, 0), 1);
    EXPECT_EQ(ft.prefixSum(1, 1), 12); // 1+2+4+5
    EXPECT_EQ(ft.prefixSum(2, 2), 45); // All elements
}

TEST_F(FenwickTree2DTest, SumAfterUpdates) {
    FenwickTree2D ft(3, 3);
    ft.add(0, 0, 1);
    ft.add(0, 1, 2);
    ft.add(1, 0, 3);
    ft.add(1, 1, 4);

    EXPECT_EQ(ft.prefixSum(1, 1), 10); // 1+2+3+4
}

TEST_F(FenwickTree2DTest, SumOutOfBoundsThrows) {
    FenwickTree2D ft(3, 3);
    EXPECT_THROW(ft.prefixSum(-1, 0), std::out_of_range);
    EXPECT_THROW(ft.prefixSum(0, -1), std::out_of_range);
    EXPECT_THROW(ft.prefixSum(3, 0), std::out_of_range);
    EXPECT_THROW(ft.prefixSum(0, 3), std::out_of_range);
}

// Range Sum Tests
TEST_F(FenwickTree2DTest, RangeSumBasic) {
    std::vector<std::vector<ll>> data = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    FenwickTree2D ft(data);

    EXPECT_EQ(ft.rangeSum(0, 0, 2, 2), 45); // All elements
    EXPECT_EQ(ft.rangeSum(0, 0, 1, 1), 12); // Top-left 2x2
    EXPECT_EQ(ft.rangeSum(1, 1, 2, 2), 28); // Bottom-right 2x2
}

TEST_F(FenwickTree2DTest, RangeSumSingleCell) {
    std::vector<std::vector<ll>> data = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    FenwickTree2D ft(data);

    EXPECT_EQ(ft.rangeSum(1, 1, 1, 1), 5);
}

TEST_F(FenwickTree2DTest, RangeSumRow) {
    std::vector<std::vector<ll>> data = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    FenwickTree2D ft(data);

    EXPECT_EQ(ft.rangeSum(1, 0, 1, 2), 15); // Second row: 4+5+6
}

TEST_F(FenwickTree2DTest, RangeSumColumn) {
    std::vector<std::vector<ll>> data = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    FenwickTree2D ft(data);

    EXPECT_EQ(ft.rangeSum(0, 1, 2, 1), 15); // Second column: 2+5+8
}

TEST_F(FenwickTree2DTest, RangeSumInvalidRange) {
    std::vector<std::vector<ll>> data = {{1, 2, 3}, {4, 5, 6}};
    FenwickTree2D ft(data);

    EXPECT_EQ(ft.rangeSum(2, 2, 1, 1), 0); // x1 > x2
    EXPECT_EQ(ft.rangeSum(1, 2, 1, 1), 0); // y1 > y2
}

TEST_F(FenwickTree2DTest, RangeSumOutOfBoundsThrows) {
    FenwickTree2D ft(3, 3);
    EXPECT_THROW(ft.rangeSum(0, 0, 3, 2), std::out_of_range);
    EXPECT_THROW(ft.rangeSum(0, 0, 2, 3), std::out_of_range);
}

TEST_F(FenwickTree2DTest, RangeSumWithNegativeValues) {
    std::vector<std::vector<ll>> data = {{-1, 2, -3}, {4, -5, 6}, {-7, 8, -9}};
    FenwickTree2D ft(data);

    EXPECT_EQ(ft.rangeSum(0, 0, 2, 2), -5);
    EXPECT_EQ(ft.rangeSum(0, 0, 1, 1), 0); // -1+2+4-5
}

// Get Operation Tests
TEST_F(FenwickTree2DTest, GetElement) {
    std::vector<std::vector<ll>> data = {{1, 2, 3}, {4, 5, 6}};
    FenwickTree2D ft(data);

    EXPECT_EQ(ft.get(0, 0), 1);
    EXPECT_EQ(ft.get(1, 2), 6);
}

TEST_F(FenwickTree2DTest, GetAfterAdd) {
    FenwickTree2D ft(3, 3);
    ft.add(1, 1, 10);
    ft.add(1, 1, 5);

    EXPECT_EQ(ft.get(1, 1), 15);
}

TEST_F(FenwickTree2DTest, GetAfterSet) {
    std::vector<std::vector<ll>> data = {{1, 2}, {3, 4}};
    FenwickTree2D ft(data);

    ft.set(0, 1, 100);
    EXPECT_EQ(ft.get(0, 1), 100);
}

// Large Value Tests
TEST_F(FenwickTree2DTest, LargeValues) {
    FenwickTree2D ft(2, 2);
    const ll large = 1000000000LL;

    ft.add(0, 0, large);
    ft.add(1, 1, large);

    EXPECT_EQ(ft.prefixSum(1, 1), 2 * large);
}

// Edge Cases
TEST_F(FenwickTree2DTest, AllZeros) {
    FenwickTree2D ft(3, 3);

    EXPECT_EQ(ft.prefixSum(2, 2), 0);
    EXPECT_EQ(ft.rangeSum(0, 0, 2, 2), 0);
}

TEST_F(FenwickTree2DTest, SingleCellTree) {
    std::vector<std::vector<ll>> data = {{42}};
    FenwickTree2D ft(data);

    EXPECT_EQ(ft.get(0, 0), 42);
    EXPECT_EQ(ft.prefixSum(0, 0), 42);
    EXPECT_EQ(ft.rangeSum(0, 0, 0, 0), 42);
}

TEST_F(FenwickTree2DTest, RectangularMatrix) {
    std::vector<std::vector<ll>> data = {{1, 2, 3, 4}, {5, 6, 7, 8}};
    FenwickTree2D ft(data);

    EXPECT_EQ(ft.rows(), 2);
    EXPECT_EQ(ft.cols(), 4);
    EXPECT_EQ(ft.prefixSum(1, 3), 36); // All elements
}

TEST_F(FenwickTree2DTest, StressTestManyOperations) {
    FenwickTree2D ft(10, 10);

    // Add values
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            ft.add(i, j, i * 10 + j + 1);
        }
    }

    // Verify sum of all elements: 1+2+...+100 = 5050
    EXPECT_EQ(ft.prefixSum(9, 9), 5050);

    // Verify partial sums
    EXPECT_EQ(ft.rangeSum(0, 0, 4, 4), 575); // First 5x5
}

TEST_F(FenwickTree2DTest, CheckerboardPattern) {
    FenwickTree2D ft(4, 4);

    // Set checkerboard pattern: 1 at (i+j) even, 0 otherwise
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if ((i + j) % 2 == 0) {
                ft.add(i, j, 1);
            }
        }
    }

    EXPECT_EQ(ft.prefixSum(3, 3), 8); // 8 cells with value 1
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
