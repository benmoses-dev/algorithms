#include "hungarian.hpp"
#include <gtest/gtest.h>

using namespace algo::graph;

class HungarianTest : public ::testing::Test {
  protected:
    void SetUp() override {}
};

TEST_F(HungarianTest, TwoByTwoGrid) {
    std::vector<std::vector<int>> grid = {{4, 1}, {2, 3}};
    HungarianResult res = hungarian(grid);

    ASSERT_EQ(res.assignments.size(), 2);
    EXPECT_EQ(res.assignments[0], 1);
    EXPECT_EQ(res.assignments[1], 0);
    EXPECT_EQ(res.cost, 3);
    EXPECT_EQ(res.totalCols, 2);
}

TEST_F(HungarianTest, TwoByThreeGrid) {
    std::vector<std::vector<int>> grid = {{5, 8, 7}, {6, 4, 3}};
    HungarianResult res = hungarian(grid);

    int expectedCost = 5 + 3;
    ASSERT_EQ(res.assignments.size(), 2);
    EXPECT_EQ(res.cost, expectedCost);
    EXPECT_EQ(res.totalCols, 2);
}

TEST_F(HungarianTest, ThreeByThreeGrid) {
    std::vector<std::vector<int>> grid = {{90, 75, 75}, {35, 85, 55}, {125, 95, 90}};
    HungarianResult res = hungarian(grid);

    ASSERT_EQ(res.assignments.size(), 3);
    EXPECT_EQ(res.assignments[0], 1);
    EXPECT_EQ(res.assignments[1], 0);
    EXPECT_EQ(res.assignments[2], 2);
    EXPECT_EQ(res.cost, 75 + 35 + 90);
    EXPECT_EQ(res.totalCols, 3);
}

TEST_F(HungarianTest, SingleRowGrid) {
    std::vector<std::vector<int>> grid = {{10, 20, 5}};
    HungarianResult res = hungarian(grid);

    ASSERT_EQ(res.assignments.size(), 1);
    EXPECT_EQ(res.assignments[0], 2); // minimal cost = 5
    EXPECT_EQ(res.cost, 5);
    EXPECT_EQ(res.totalCols, 1);
}

TEST_F(HungarianTest, InvalidGridThrowsException) {
    std::vector<std::vector<int>> grid = {{1}, {2}};

    EXPECT_THROW({ HungarianResult res = hungarian(grid); }, std::invalid_argument);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
