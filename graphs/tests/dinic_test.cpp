#include "dinic.hpp"
#include <algorithm>
#include <gtest/gtest.h>

using namespace algo::flow;

class DinicTest : public ::testing::Test {
  protected:
    void SetUp() override {}
};

TEST_F(DinicTest, SimpleFlow) {
    Dinic dinic(2);
    dinic.addEdge(0, 1, 10);

    ll flow = dinic.maxFlow(0, 1);
    EXPECT_EQ(flow, 10);
}

TEST_F(DinicTest, MultiplePaths) {
    Dinic dinic(4);
    dinic.addEdge(0, 1, 10);
    dinic.addEdge(0, 2, 10);
    dinic.addEdge(1, 3, 10);
    dinic.addEdge(2, 3, 10);

    ll flow = dinic.maxFlow(0, 3);
    EXPECT_EQ(flow, 20);
}

TEST_F(DinicTest, Bottleneck) {
    // Graph with bottleneck in the middle
    //   0 --100--> 1 --5--> 2 --100--> 3
    Dinic dinic(4);
    dinic.addEdge(0, 1, 100);
    dinic.addEdge(1, 2, 5);
    dinic.addEdge(2, 3, 100);

    ll flow = dinic.maxFlow(0, 3);
    EXPECT_EQ(flow, 5);
}

TEST_F(DinicTest, NoPath) {
    Dinic dinic(4);
    dinic.addEdge(0, 1, 10);
    dinic.addEdge(2, 3, 10);

    ll flow = dinic.maxFlow(0, 3);
    EXPECT_EQ(flow, 0);
}

TEST_F(DinicTest, ComplexNetwork) {
    Dinic dinic(5);
    dinic.addEdge(0, 1, 10);
    dinic.addEdge(0, 2, 10);
    dinic.addEdge(1, 3, 10);
    dinic.addEdge(1, 2, 2);
    dinic.addEdge(2, 4, 9);
    dinic.addEdge(4, 3, 9);

    ll flow = dinic.maxFlow(0, 3);
    EXPECT_EQ(flow, 19);
}

TEST_F(DinicTest, UndirectedEdge) {
    // Undirected edge should allow flow in both directions
    Dinic dinic(3);
    dinic.addEdge(0, 1, 10, false);
    dinic.addEdge(1, 2, 5);

    ll flow = dinic.maxFlow(0, 2);
    EXPECT_EQ(flow, 5);
}

TEST_F(DinicTest, SelfLoop) {
    // Self loops shouldn't affect flow
    Dinic dinic(3);
    dinic.addEdge(0, 1, 10);
    dinic.addEdge(1, 1, 100);
    dinic.addEdge(1, 2, 10);

    ll flow = dinic.maxFlow(0, 2);
    EXPECT_EQ(flow, 10);
}

TEST_F(DinicTest, ParallelEdges) {
    Dinic dinic(2);
    dinic.addEdge(0, 1, 5);
    dinic.addEdge(0, 1, 7);
    dinic.addEdge(0, 1, 3);

    ll flow = dinic.maxFlow(0, 1);
    EXPECT_EQ(flow, 15);
}

TEST_F(DinicTest, Reset) {
    Dinic dinic(3);
    dinic.addEdge(0, 1, 10);
    dinic.addEdge(1, 2, 10);

    ll flow1 = dinic.maxFlow(0, 2);
    EXPECT_EQ(flow1, 10);

    dinic.reset();

    ll flow2 = dinic.maxFlow(0, 2);
    EXPECT_EQ(flow2, 10);
}

TEST_F(DinicTest, GetFlows) {
    Dinic dinic(3);
    dinic.addEdge(0, 1, 10);
    dinic.addEdge(1, 2, 10);

    dinic.maxFlow(0, 2);

    auto flows = dinic.getFlows();

    // Should have 2 forward edges (reverse edges have cap = 0, so they're not included)
    ASSERT_EQ(flows.size(), 2);

    // Check that forward edges have flow
    bool found_0_1 = false, found_1_2 = false;
    for (const auto &f : flows) {
        if (f.from == 0 && f.to == 1 && f.cap == 10) {
            EXPECT_EQ(f.flow, 10);
            found_0_1 = true;
        }
        if (f.from == 1 && f.to == 2 && f.cap == 10) {
            EXPECT_EQ(f.flow, 10);
            found_1_2 = true;
        }
    }
    EXPECT_TRUE(found_0_1 && found_1_2);
}

TEST_F(DinicTest, MinCut) {
    Dinic dinic(4);
    dinic.addEdge(0, 1, 10);
    dinic.addEdge(0, 3, 10);
    dinic.addEdge(1, 2, 10);
    dinic.addEdge(3, 2, 10);

    dinic.maxFlow(0, 2);

    auto [sourceSide, sinkSide] = dinic.getMinCut(0);

    // Source side should contain 0, sink side should contain 2
    EXPECT_NE(std::find(sourceSide.begin(), sourceSide.end(), 0), sourceSide.end());
    EXPECT_NE(std::find(sinkSide.begin(), sinkSide.end(), 2), sinkSide.end());

    // All vertices should be accounted for
    EXPECT_EQ(sourceSide.size() + sinkSide.size(), 4);
}

TEST_F(DinicTest, MinCutEdges) {
    // 0 --5--> 1 --10--> 2
    Dinic dinic(3);
    dinic.addEdge(0, 1, 5);
    dinic.addEdge(1, 2, 10);

    ll flow = dinic.maxFlow(0, 2);
    EXPECT_EQ(flow, 5);

    auto cutEdges = dinic.getMinCutEdges(0);

    // The cut should be at edge 0->1 (capacity 5)
    ASSERT_EQ(cutEdges.size(), 1);
    EXPECT_EQ(cutEdges[0].first, 0);
    EXPECT_EQ(cutEdges[0].second, 1);
}

TEST_F(DinicTest, MinCutCapacity) {
    Dinic dinic(4);
    dinic.addEdge(0, 1, 10);
    dinic.addEdge(0, 2, 10);
    dinic.addEdge(1, 3, 10);
    dinic.addEdge(2, 3, 10);

    ll flow = dinic.maxFlow(0, 3);
    ll cutCapacity = dinic.getMinCutCapacity(0);

    // Max-flow min-cut theorem: flow should equal the cut capacity
    EXPECT_EQ(flow, cutCapacity);
    EXPECT_EQ(cutCapacity, 20);
}

TEST_F(DinicTest, LargeCapacity) {
    Dinic dinic(2);
    ll large_cap = 1000000000LL;
    dinic.addEdge(0, 1, large_cap);

    ll flow = dinic.maxFlow(0, 1);
    EXPECT_EQ(flow, large_cap);
}

TEST_F(DinicTest, ZeroCapacity) {
    Dinic dinic(2);
    dinic.addEdge(0, 1, 0);

    ll flow = dinic.maxFlow(0, 1);
    EXPECT_EQ(flow, 0);
}

TEST_F(DinicTest, SourceEqualsSink) {
    Dinic dinic(3);
    dinic.addEdge(0, 1, 10);
    dinic.addEdge(1, 2, 10);

    ll flow = dinic.maxFlow(1, 1);
    EXPECT_EQ(flow, 0);
}

TEST_F(DinicTest, BipartiteMatching) {
    Dinic dinic(8);

    // Source to left side
    dinic.addEdge(6, 0, 1);
    dinic.addEdge(6, 1, 1);
    dinic.addEdge(6, 2, 1);

    // Matching edges
    dinic.addEdge(0, 3, 1);
    dinic.addEdge(0, 4, 1);
    dinic.addEdge(1, 3, 1);
    dinic.addEdge(1, 5, 1);
    dinic.addEdge(2, 4, 1);

    // Right side to sink
    dinic.addEdge(3, 7, 1);
    dinic.addEdge(4, 7, 1);
    dinic.addEdge(5, 7, 1);

    ll matching = dinic.maxFlow(6, 7);
    EXPECT_EQ(matching, 3);
}

TEST_F(DinicTest, MultipleRuns) {
    Dinic dinic(4);
    dinic.addEdge(0, 1, 10);
    dinic.addEdge(1, 2, 10);
    dinic.addEdge(2, 3, 10);

    ll flow1 = dinic.maxFlow(0, 2);
    EXPECT_EQ(flow1, 10);

    // Running maxFlow again without a reset should return 0
    ll flow2 = dinic.maxFlow(0, 2);
    EXPECT_EQ(flow2, 0);

    // After reset, it should work again
    dinic.reset();
    ll flow3 = dinic.maxFlow(0, 2);
    EXPECT_EQ(flow3, 10);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
