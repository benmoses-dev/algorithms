#include "dsu.hpp"
#include <gtest/gtest.h>

using namespace algo::graph;

class DSUTest : public ::testing::Test {
  protected:
    void SetUp() override {}
};

TEST_F(DSUTest, Initialization) {
    std::vector<int> values = {1, 2, 3, 4, 5};
    DSU dsu(values);

    EXPECT_EQ(dsu.getComponents(), 5);

    for (std::size_t i = 0; i < values.size(); i++) {
        EXPECT_EQ(dsu.find(i), i);
        EXPECT_EQ(dsu.getSize(i), 1);
        EXPECT_EQ(dsu.getMaxVal(i), values[i]);
        EXPECT_EQ(dsu.getSum(i), values[i]);
    }
}

TEST_F(DSUTest, SimpleUnion) {
    std::vector<int> values = {1, 2, 3, 4};
    DSU dsu(values);

    dsu.unite(0, 1);

    EXPECT_EQ(dsu.find(0), dsu.find(1));
    EXPECT_EQ(dsu.getSize(0), 2);
    EXPECT_EQ(dsu.getSize(1), 2);
    EXPECT_EQ(dsu.getMaxVal(0), 2);
    EXPECT_EQ(dsu.getSum(0), 3);
    EXPECT_EQ(dsu.getComponents(), 3);
}

TEST_F(DSUTest, UnionSameRoot) {
    std::vector<int> values = {1, 2, 3};
    DSU dsu(values);

    dsu.unite(0, 1);
    std::size_t componentsBefore = dsu.getComponents();
    std::size_t sizeBefore = dsu.getSize(0);

    dsu.unite(0, 1);

    EXPECT_EQ(dsu.getComponents(), componentsBefore);
    EXPECT_EQ(dsu.getSize(0), sizeBefore);
}

TEST_F(DSUTest, ChainUnions) {
    std::vector<int> values = {1, 2, 3, 4, 5};
    DSU dsu(values);

    dsu.unite(0, 1);
    dsu.unite(1, 2);
    dsu.unite(2, 3);

    EXPECT_EQ(dsu.find(0), dsu.find(3));
    EXPECT_EQ(dsu.getSize(0), 4);
    EXPECT_EQ(dsu.getMaxVal(0), 4);
    EXPECT_EQ(dsu.getSum(0), 10);
    EXPECT_EQ(dsu.getComponents(), 2);
}

TEST_F(DSUTest, PathCompression) {
    std::vector<int> values = {1, 2, 3, 4, 5};
    DSU dsu(values);

    dsu.unite(0, 1);
    dsu.unite(1, 2);
    dsu.unite(2, 3);

    std::size_t root = dsu.find(3);
    EXPECT_EQ(dsu.find(3), root);
    EXPECT_EQ(dsu.find(2), root);
    EXPECT_EQ(dsu.find(1), root);
    EXPECT_EQ(dsu.find(0), root);
}

TEST_F(DSUTest, MaxValueTracking) {
    std::vector<int> values = {5, 10, 3, 15, 7};
    DSU dsu(values);

    dsu.unite(0, 1);
    EXPECT_EQ(dsu.getMaxVal(0), 10);

    dsu.unite(0, 3);
    EXPECT_EQ(dsu.getMaxVal(0), 15);

    dsu.unite(2, 4);
    EXPECT_EQ(dsu.getMaxVal(2), 7);
}

TEST_F(DSUTest, SumTracking) {
    std::vector<int> values = {10, 20, 30, 40};
    DSU dsu(values);

    dsu.unite(0, 1);
    EXPECT_EQ(dsu.getSum(0), 30);

    dsu.unite(2, 3);
    EXPECT_EQ(dsu.getSum(2), 70);

    dsu.unite(0, 2);
    EXPECT_EQ(dsu.getSum(0), 100);
}

TEST_F(DSUTest, ComponentCounting) {
    std::vector<int> values = {1, 2, 3, 4, 5, 6};
    DSU dsu(values);

    EXPECT_EQ(dsu.getComponents(), 6);

    dsu.unite(0, 1);
    EXPECT_EQ(dsu.getComponents(), 5);

    dsu.unite(2, 3);
    EXPECT_EQ(dsu.getComponents(), 4);

    dsu.unite(4, 5);
    EXPECT_EQ(dsu.getComponents(), 3);

    dsu.unite(0, 2);
    EXPECT_EQ(dsu.getComponents(), 2);

    dsu.unite(0, 4);
    EXPECT_EQ(dsu.getComponents(), 1);
}

TEST_F(DSUTest, NegativeValues) {
    std::vector<int> values = {-5, -10, 3, -2};
    DSU dsu(values);

    EXPECT_EQ(dsu.getMaxVal(0), -5);
    EXPECT_EQ(dsu.getMaxVal(1), -10);
    EXPECT_EQ(dsu.getMaxVal(2), 3);
    EXPECT_EQ(dsu.getSum(0), -5);
    EXPECT_EQ(dsu.getSum(1), -10);
}

TEST_F(DSUTest, MixedSignValues) {
    std::vector<int> values = {-10, 5, -3, 8, -1};
    DSU dsu(values);

    dsu.unite(0, 1);
    EXPECT_EQ(dsu.getSum(0), -5);
    EXPECT_EQ(dsu.getMaxVal(0), 5);

    dsu.unite(2, 3);
    EXPECT_EQ(dsu.getSum(2), 5);
    EXPECT_EQ(dsu.getMaxVal(2), 8);
}

TEST_F(DSUTest, LargeValues) {
    std::vector<int> values = {1000000, 2000000, 3000000};
    DSU dsu(values);

    dsu.unite(0, 1);
    dsu.unite(1, 2);

    EXPECT_EQ(dsu.getSum(0), 6000000);
    EXPECT_EQ(dsu.getMaxVal(0), 3000000);
}

TEST_F(DSUTest, TransitiveUnions) {
    std::vector<int> values = {1, 2, 3, 4, 5};
    DSU dsu(values);

    dsu.unite(0, 1);
    dsu.unite(3, 4);
    dsu.unite(1, 3);

    EXPECT_EQ(dsu.find(0), dsu.find(4));
    EXPECT_EQ(dsu.getSize(0), 4);
    EXPECT_EQ(dsu.getComponents(), 2);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
