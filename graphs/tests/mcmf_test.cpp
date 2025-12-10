#include "mcmf.hpp"
#include <gtest/gtest.h>

using namespace algo::graph;

class MCMFTest : public ::testing::Test {
  protected:
    void SetUp() override {}
};

// Basic Functionality Tests

TEST_F(MCMFTest, SimplePath) {
    // s --[cap=10, cost=2]--> t
    MinCostMaxFlow mcmf(2);
    mcmf.addEdge(0, 1, 10, 2);

    const auto [flow, cost] = mcmf.solve(0, 1);

    EXPECT_EQ(flow, 10);
    EXPECT_EQ(cost, 20);
}

TEST_F(MCMFTest, TwoPathsChooseCheaper) {
    // s --[cap=5, cost=1]--> t
    //  \--[cap=5, cost=3]--/
    MinCostMaxFlow mcmf(2);
    mcmf.addEdge(0, 1, 5, 1);
    mcmf.addEdge(0, 1, 5, 3);

    const auto [flow, cost] = mcmf.solve(0, 1);

    EXPECT_EQ(flow, 10);
    EXPECT_EQ(cost, 20); // 5*1 + 5*3
}

TEST_F(MCMFTest, DiamondGraph) {
    /**
     *      1
     *    /   \
     *   s     t
     *    \   /
     *      2
     */
    MinCostMaxFlow mcmf(4);
    mcmf.addEdge(0, 1, 10, 1);
    mcmf.addEdge(0, 2, 10, 2);
    mcmf.addEdge(1, 3, 10, 2);
    mcmf.addEdge(2, 3, 10, 1);

    const auto [flow, cost] = mcmf.solve(0, 3);

    EXPECT_EQ(flow, 20);
    EXPECT_EQ(cost, 60);
}

TEST_F(MCMFTest, NoPath) {
    MinCostMaxFlow mcmf(3);
    mcmf.addEdge(0, 1, 10, 1);

    const auto [flow, cost] = mcmf.solve(0, 2);

    EXPECT_EQ(flow, 0);
    EXPECT_EQ(cost, 0);
}

TEST_F(MCMFTest, Bottleneck) {
    // s --[cap=10, cost=1]--> a --[cap=5, cost=1]--> t
    MinCostMaxFlow mcmf(3);
    mcmf.addEdge(0, 1, 10, 1);
    mcmf.addEdge(1, 2, 5, 1);

    const auto [flow, cost] = mcmf.solve(0, 2);

    EXPECT_EQ(flow, 5);
    EXPECT_EQ(cost, 10);
}

TEST_F(MCMFTest, MultiplePaths) {
    MinCostMaxFlow mcmf(4);
    mcmf.addEdge(0, 1, 10, 1);
    mcmf.addEdge(1, 3, 10, 1);
    mcmf.addEdge(0, 2, 5, 2);
    mcmf.addEdge(2, 3, 5, 2);

    const auto [flow, cost] = mcmf.solve(0, 3);

    EXPECT_EQ(flow, 15);
    EXPECT_EQ(cost, 40);
}

TEST_F(MCMFTest, SelfLoop) {
    MinCostMaxFlow mcmf(2);
    mcmf.addEdge(0, 0, 10, 1);
    mcmf.addEdge(0, 1, 5, 2);

    const auto [flow, cost] = mcmf.solve(0, 1);

    EXPECT_EQ(flow, 5);
    EXPECT_EQ(cost, 10);
}

TEST_F(MCMFTest, ZeroCapacity) {
    MinCostMaxFlow mcmf(2);
    mcmf.addEdge(0, 1, 0, 1);

    const auto [flow, cost] = mcmf.solve(0, 1);

    EXPECT_EQ(flow, 0);
    EXPECT_EQ(cost, 0);
}

TEST_F(MCMFTest, LargeCapacity) {
    MinCostMaxFlow mcmf(2);
    const ll large_cap = 1000000000LL;
    mcmf.addEdge(0, 1, large_cap, 1);

    const auto [flow, cost] = mcmf.solve(0, 1);

    EXPECT_EQ(flow, large_cap);
    EXPECT_EQ(cost, large_cap);
}

TEST_F(MCMFTest, ZeroCost) {
    MinCostMaxFlow mcmf(2);
    mcmf.addEdge(0, 1, 10, 0);

    const auto [flow, cost] = mcmf.solve(0, 1);

    EXPECT_EQ(flow, 10);
    EXPECT_EQ(cost, 0);
}

TEST_F(MCMFTest, ComplexNetwork) {
    MinCostMaxFlow mcmf(5);
    mcmf.addEdge(0, 1, 5, 2);
    mcmf.addEdge(0, 2, 5, 3);
    mcmf.addEdge(1, 3, 10, 1);
    mcmf.addEdge(2, 3, 10, 1);
    mcmf.addEdge(3, 4, 10, 2);

    const auto [flow, cost] = mcmf.solve(0, 4);

    EXPECT_EQ(flow, 10);
    EXPECT_EQ(cost, 55);
}

TEST_F(MCMFTest, SourceEqualsSink) {
    MinCostMaxFlow mcmf(3);
    mcmf.addEdge(0, 1, 10, 1);

    const auto [flow, cost] = mcmf.solve(0, 0);

    EXPECT_EQ(flow, 0);
    EXPECT_EQ(cost, 0);
}

// API Tests

TEST_F(MCMFTest, Reset) {
    MinCostMaxFlow mcmf(2);
    mcmf.addEdge(0, 1, 10, 2);

    const auto [flow1, cost1] = mcmf.solve(0, 1);
    EXPECT_EQ(flow1, 10);
    EXPECT_EQ(cost1, 20);

    mcmf.reset();
    const auto [flow2, cost2] = mcmf.solve(0, 1);
    EXPECT_EQ(flow2, 10);
    EXPECT_EQ(cost2, 20);
}

TEST_F(MCMFTest, GetFlows) {
    MinCostMaxFlow mcmf(3);
    mcmf.addEdge(0, 1, 10, 1);
    mcmf.addEdge(1, 2, 5, 2);

    mcmf.solve(0, 2);

    const auto flows = mcmf.getFlows();

    std::size_t positive_flows = 0;
    for (const auto &f : flows) {
        if (f.flow > 0)
            positive_flows++;
    }

    EXPECT_EQ(positive_flows, 2);
}

TEST_F(MCMFTest, NegativeCosts) {
    MinCostMaxFlow mcmf(3);
    mcmf.addEdge(0, 1, 10, -5);
    mcmf.addEdge(1, 2, 10, 3);

    mcmf.initPotentials(0);
    const auto [flow, cost] = mcmf.solve(0, 2);

    EXPECT_EQ(flow, 10);
    EXPECT_EQ(cost, -20);
}

TEST_F(MCMFTest, MultipleResets) {
    MinCostMaxFlow mcmf(3);
    mcmf.addEdge(0, 1, 10, 1);
    mcmf.addEdge(1, 2, 10, 2);

    for (std::size_t i = 0; i < 10; i++) {
        auto [flow, cost] = mcmf.solve(0, 2);
        EXPECT_EQ(flow, 10);
        EXPECT_EQ(cost, 30);
        mcmf.reset();
    }
}

// Real-World Applications

TEST_F(MCMFTest, AssignmentProblem) {
    // 2 workers, 2 jobs
    // Optimal: W0->J0 (5) + W1->J1 (3) = 8

    const std::size_t source = 0;
    const std::size_t w0 = 1, w1 = 2;
    const std::size_t j0 = 3, j1 = 4;
    const std::size_t sink = 5;

    MinCostMaxFlow mcmf(6);

    mcmf.addEdge(source, w0, 1, 0);
    mcmf.addEdge(source, w1, 1, 0);

    mcmf.addEdge(w0, j0, 1, 5);
    mcmf.addEdge(w0, j1, 1, 10);
    mcmf.addEdge(w1, j0, 1, 8);
    mcmf.addEdge(w1, j1, 1, 3);

    mcmf.addEdge(j0, sink, 1, 0);
    mcmf.addEdge(j1, sink, 1, 0);

    const auto [flow, cost] = mcmf.solve(source, sink);

    EXPECT_EQ(flow, 2);
    EXPECT_EQ(cost, 8);
}

TEST_F(MCMFTest, CapacitatedAssignment) {
    // Workers can take multiple jobs

    const std::size_t source = 0;
    const std::size_t w0 = 1, w1 = 2;
    const std::size_t j0 = 3, j1 = 4, j2 = 5;
    const std::size_t sink = 6;

    MinCostMaxFlow mcmf(7);

    mcmf.addEdge(source, w0, 2, 0);
    mcmf.addEdge(source, w1, 1, 0);

    mcmf.addEdge(w0, j0, 1, 5);
    mcmf.addEdge(w0, j1, 1, 3);
    mcmf.addEdge(w0, j2, 1, 7);
    mcmf.addEdge(w1, j0, 1, 2);
    mcmf.addEdge(w1, j1, 1, 4);
    mcmf.addEdge(w1, j2, 1, 6);

    mcmf.addEdge(j0, sink, 1, 0);
    mcmf.addEdge(j1, sink, 1, 0);
    mcmf.addEdge(j2, sink, 1, 0);

    const auto [flow, cost] = mcmf.solve(source, sink);

    EXPECT_EQ(flow, 3);
    EXPECT_EQ(cost, 12);
}

TEST_F(MCMFTest, ParallelEdges) {
    MinCostMaxFlow mcmf(2);
    mcmf.addEdge(0, 1, 5, 1);
    mcmf.addEdge(0, 1, 5, 2);
    mcmf.addEdge(0, 1, 5, 3);

    const auto [flow, cost] = mcmf.solve(0, 1);

    EXPECT_EQ(flow, 15);
    EXPECT_EQ(cost, 30);
}

TEST_F(MCMFTest, LongPath) {
    MinCostMaxFlow mcmf(6);
    for (std::size_t i = 0; i < 5; i++) {
        mcmf.addEdge(i, i + 1, 10, 1);
    }

    const auto [flow, cost] = mcmf.solve(0, 5);

    EXPECT_EQ(flow, 10);
    EXPECT_EQ(cost, 50);
}

TEST_F(MCMFTest, WideGraph) {
    const std::size_t n = 102;
    MinCostMaxFlow mcmf(n);

    for (std::size_t i = 1; i < n - 1; i++) {
        mcmf.addEdge(0, i, 1, static_cast<ll>(i));
        mcmf.addEdge(i, n - 1, 1, 1);
    }

    auto [flow, cost] = mcmf.solve(0, n - 1);

    EXPECT_EQ(flow, 100);
    EXPECT_EQ(cost, 5150);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
