#include "mst.hpp"
#include <gtest/gtest.h>

using namespace algo::mst;

class MSTTest : public ::testing::Test {
  protected:
    void SetUp() override {}
};

TEST_F(MSTTest, SimpleTree) {
    // Triangle: 0-1 (weight 1), 1-2 (weight 2), and 0-2 (weight 3)
    // MST should use the edges with weights 1 and 2
    const std::vector<Edge> edges = {{0, 1, 1}, {1, 2, 2}, {0, 2, 3}};

    const auto [mstEdges, weight] = mst(edges, 3);

    EXPECT_EQ(mstEdges.size(), 2);
    EXPECT_EQ(weight, 3);
}

TEST_F(MSTTest, LineGraph) {
    // 0 -- 1 -- 2 -- 3
    const std::vector<Edge> edges = {{0, 1, 5}, {1, 2, 3}, {2, 3, 7}};

    const auto [mstEdges, weight] = mst(edges, 4);

    EXPECT_EQ(mstEdges.size(), 3);
    EXPECT_EQ(weight, 15);
}

TEST_F(MSTTest, SingleVertex) {
    const std::vector<Edge> edges = {};

    const auto [mstEdges, weight] = mst(edges, 1);

    EXPECT_EQ(mstEdges.size(), 0);
    EXPECT_EQ(weight, 0);
}

TEST_F(MSTTest, TwoVertices) {
    const std::vector<Edge> edges = {{0, 1, 10}};

    const auto [mstEdges, weight] = mst(edges, 2);

    ASSERT_EQ(mstEdges.size(), 1);
    EXPECT_EQ(weight, 10);
    EXPECT_EQ(mstEdges[0].w, 10);
}

TEST_F(MSTTest, CompleteGraph) {
    // K4: complete graph with 4 vertices
    const std::vector<Edge> edges = {{0, 1, 1}, {0, 2, 4}, {0, 3, 3},
                                     {1, 2, 2}, {1, 3, 5}, {2, 3, 6}};

    const auto [mstEdges, weight] = mst(edges, 4);

    EXPECT_EQ(mstEdges.size(), 3);
    EXPECT_EQ(weight, 6); // 1 + 2 + 3
}

TEST_F(MSTTest, ParallelEdges) {
    // Multiple edges between the same vertices
    const std::vector<Edge> edges = {{0, 1, 10},
                                     {0, 1, 5}, // Better edge
                                     {0, 1, 15},
                                     {1, 2, 3}};

    const auto [mstEdges, weight] = mst(edges, 3);

    EXPECT_EQ(mstEdges.size(), 2);
    EXPECT_EQ(weight, 8); // Should use 5 and 3
}

TEST_F(MSTTest, NegativeWeights) {
    const std::vector<Edge> edges = {{0, 1, -5}, {1, 2, -3}, {0, 2, 10}};

    const auto [mstEdges, weight] = mst(edges, 3);

    EXPECT_EQ(mstEdges.size(), 2);
    EXPECT_EQ(weight, -8);
}

TEST_F(MSTTest, EqualWeights) {
    const std::vector<Edge> edges = {{0, 1, 5}, {1, 2, 5}, {2, 3, 5}, {0, 3, 5}};

    const auto [mstEdges, weight] = mst(edges, 4);

    EXPECT_EQ(mstEdges.size(), 3);
    EXPECT_EQ(weight, 15);
}

TEST_F(MSTTest, LargeWeights) {
    const std::vector<Edge> edges = {
        {0, 1, 1000000000LL}, {1, 2, 2000000000LL}, {0, 2, 5000000000LL}};

    const auto [mstEdges, weight] = mst(edges, 3);

    EXPECT_EQ(mstEdges.size(), 2);
    EXPECT_EQ(weight, 3000000000LL);
}

TEST_F(MSTTest, SelfLoops) {
    // Self loops should be ignored
    const std::vector<Edge> edges = {{0, 0, 100}, // Self loop
                                     {0, 1, 5},
                                     {1, 1, 50}, // Self loop
                                     {1, 2, 3}};

    const auto [mstEdges, weight] = mst(edges, 3);

    EXPECT_EQ(mstEdges.size(), 2);
    EXPECT_EQ(weight, 8);
}

TEST_F(MSTTest, AlreadySorted) {
    const std::vector<Edge> edges = {{0, 1, 1}, {1, 2, 2}, {2, 3, 3}, {0, 3, 10}};

    const auto [mstEdges, weight] = mst(edges, 4);

    EXPECT_EQ(mstEdges.size(), 3);
    EXPECT_EQ(weight, 6);
}

TEST_F(MSTTest, ReverseSorted) {
    const std::vector<Edge> edges = {{0, 3, 10}, {2, 3, 3}, {1, 2, 2}, {0, 1, 1}};

    const auto [mstEdges, weight] = mst(edges, 4);

    EXPECT_EQ(mstEdges.size(), 3);
    EXPECT_EQ(weight, 6);
}

TEST_F(MSTTest, StarGraph) {
    // Central node 0 connected to all other nodes
    const std::vector<Edge> edges = {{0, 1, 1}, {0, 2, 2}, {0, 3, 3}, {0, 4, 4}};

    const auto [mstEdges, weight] = mst(edges, 5);

    EXPECT_EQ(mstEdges.size(), 4);
    EXPECT_EQ(weight, 10);
}

TEST_F(MSTTest, CycleGraph) {
    // Cycle: 0-1-2-3-0 with increasing weights
    const std::vector<Edge> edges = {
        {0, 1, 1}, {1, 2, 2}, {2, 3, 3}, {3, 0, 4} // This edge should be excluded
    };

    const auto [mstEdges, weight] = mst(edges, 4);

    EXPECT_EQ(mstEdges.size(), 3);
    EXPECT_EQ(weight, 6);
}

TEST_F(MSTTest, DisconnectedComponents) {
    const std::vector<Edge> edges = {{0, 1, 5}, {2, 3, 7}};

    // This creates a forest, not a tree
    EXPECT_THROW({ mst(edges, 4); }, std::runtime_error);
}

TEST_F(MSTTest, ZeroWeightEdges) {
    const std::vector<Edge> edges = {{0, 1, 0}, {1, 2, 0}, {2, 3, 5}};

    const auto [mstEdges, weight] = mst(edges, 4);

    EXPECT_EQ(mstEdges.size(), 3);
    EXPECT_EQ(weight, 5);
}

TEST_F(MSTTest, BidirectionalEdges) {
    // Same edge added in both directions
    const std::vector<Edge> edges = {{0, 1, 5},
                                     {1, 0, 5}, // Same edge, reverse direction
                                     {1, 2, 3}};

    const auto [mstEdges, weight] = mst(edges, 3);

    // Should only use one direction of the 0-1 edge
    EXPECT_EQ(mstEdges.size(), 2);
    EXPECT_EQ(weight, 8);
}

TEST_F(MSTTest, EdgeOrderingPreserved) {
    const std::vector<Edge> edges = {{2, 3, 1}, {0, 1, 2}, {1, 2, 3}};

    const auto [mstEdges, weight] = mst(edges, 4);

    ASSERT_EQ(mstEdges.size(), 3);
    EXPECT_EQ(weight, 6);

    // Edges should be in sorted order by weight
    EXPECT_LE(mstEdges[0].w, mstEdges[1].w);
    EXPECT_LE(mstEdges[1].w, mstEdges[2].w);
}

TEST_F(MSTTest, EmptyGraph) {
    const std::vector<Edge> edges = {};

    const auto [mstEdges, weight] = mst(edges, 0);

    EXPECT_EQ(mstEdges.size(), 0);
    EXPECT_EQ(weight, 0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
