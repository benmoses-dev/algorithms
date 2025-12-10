#include "dijkstra.hpp"
#include <climits>
#include <gtest/gtest.h>
#include <vector>

using namespace algo::graph;

class DijkstraTest : public ::testing::Test {
  protected:
    std::vector<std::vector<edge>> graph;
    std::vector<i64> prev;
    std::vector<uint64_t> dist;

    void SetUp() override {
        /**
         * Graph (directed, with a cycle):
         *
         *   0 --2--> 1
         *   0 --500--> 2
         *   0 --1000--> 6
         *   1 --50--> 2
         *   1 --200--> 3
         *   2 --100--> 3
         *   2 --600--> 4
         *   3 --1--> 4
         *   3 --300--> 5
         *   4 --2--> 5
         *   4 --10--> 2   <-- cycle edge
         *   5 --1000--> 6
         *
         * Node 7 is disconnected.
         *
         * Shortest paths from 0:
         *   dist[0] = 0
         *   dist[1] = 2                   (0 -> 1)
         *   dist[2] = 52                  (0 -> 1 -> 2)
         *   dist[3] = 152                 (0 -> 1 -> 2 -> 3)
         *   dist[4] = 153                 (0 -> 1 -> 2 -> 3 -> 4)
         *   dist[5] = 155                 (0 -> 1 -> 2 -> 3 -> 4 -> 5)
         *   dist[6] = 1000                (0 -> 6)
         *   dist[7] = UINT_MAX            (disconnected)
         */
        graph.resize(8);
        graph[0].push_back({1, 2});
        graph[0].push_back({2, 500});
        graph[0].push_back({6, 1000});
        graph[1].push_back({2, 50});
        graph[1].push_back({3, 200});
        graph[2].push_back({3, 100});
        graph[2].push_back({4, 600});
        graph[3].push_back({4, 1});
        graph[3].push_back({5, 300});
        graph[4].push_back({5, 2});
        graph[4].push_back({2, 10}); // cycle edge
        graph[5].push_back({6, 1000});
        // node 7: disconnected

        prev.resize(8, -1);
        dist = dijkstra(graph, prev, 0);
    }
};

TEST_F(DijkstraTest, ShortestDistances) {
    EXPECT_EQ(dist[0], 0) << "Distance to source should be 0";
    EXPECT_EQ(dist[1], 2) << "Shortest path: 0 -> 1";
    EXPECT_EQ(dist[2], 52) << "Shortest path: 0 -> 1 -> 2";
    EXPECT_EQ(dist[3], 152) << "Shortest path: 0 -> 1 -> 2 -> 3";
    EXPECT_EQ(dist[4], 153) << "Shortest path: 0 -> 1 -> 2 -> 3 -> 4";
    EXPECT_EQ(dist[5], 155) << "Shortest path: 0 -> 1 -> 2 -> 3 -> 4 -> 5";
    EXPECT_EQ(dist[6], 1000) << "Shortest path: 0 -> 6";
    EXPECT_EQ(dist[7], UINT64_MAX) << "Node 7 is disconnected";
}

TEST_F(DijkstraTest, PredecessorChain) {
    EXPECT_EQ(prev[0], -1) << "Source has no predecessor";
    EXPECT_EQ(prev[1], 0) << "Path: 0 -> 1";
    EXPECT_EQ(prev[2], 1) << "Path: 0 -> 1 -> 2";
    EXPECT_EQ(prev[3], 2) << "Path: 0 -> 1 -> 2 -> 3";
    EXPECT_EQ(prev[4], 3) << "Path: 0 -> 1 -> 2 -> 3 -> 4";
    EXPECT_EQ(prev[5], 4) << "Path: 0 -> 1 -> 2 -> 3 -> 4 -> 5";
    EXPECT_EQ(prev[6], 0) << "Path: 0 -> 6";
    EXPECT_EQ(prev[7], -1) << "Node 7 is disconnected";
}

TEST_F(DijkstraTest, PathReconstructionToNode5) {
    auto path = reconstructPath(5, prev);
    std::vector<uint64_t> expected = {0, 1, 2, 3, 4, 5};

    ASSERT_EQ(path.size(), expected.size()) << "Path to node 5 should have 6 nodes";

    for (size_t i = 0; i < expected.size(); i++) {
        EXPECT_EQ(path[i], expected[i]) << "Path mismatch at position " << i;
    }
}

TEST_F(DijkstraTest, PathReconstructionToNode6) {
    auto path = reconstructPath(6, prev);
    std::vector<uint64_t> expected = {0, 6};

    ASSERT_EQ(path.size(), expected.size()) << "Path to node 6 should have 2 nodes";

    EXPECT_EQ(path, expected) << "Direct path 0 -> 6";
}

TEST_F(DijkstraTest, PathReconstructionToNode2) {
    auto path = reconstructPath(2, prev);
    std::vector<uint64_t> expected = {0, 1, 2};

    ASSERT_EQ(path.size(), expected.size()) << "Path to node 2 should have 3 nodes";

    EXPECT_EQ(path, expected) << "Path: 0 -> 1 -> 2";
}

TEST_F(DijkstraTest, PathReconstructionToDisconnectedNode) {
    auto path = reconstructPath(7, prev);
    std::vector<uint64_t> expected = {7};

    ASSERT_EQ(path.size(), 1) << "Unreachable node should only contain itself";

    EXPECT_EQ(path, expected)
        << "Disconnected node 7 should return path containing only itself";
}

TEST_F(DijkstraTest, CycleHandling) {
    // Verify the algorithm handles the cycle correctly
    // The cycle 2 -> 4 -> 2 should not cause infinite loops
    // and should not provide a shorter path than the optimal one

    // Node 2 should still be reached optimally via 0 -> 1 -> 2
    EXPECT_EQ(dist[2], 52);
    EXPECT_EQ(prev[2], 1);

    // The cycle edge 4 -> 2 should not be used in the shortest path
    // because going 0 -> 1 -> 2 (cost 52) is better than
    // 0 -> 1 -> 2 -> 3 -> 4 -> 2 (cost 163)
}

TEST_F(DijkstraTest, GraphStructure) {
    EXPECT_EQ(graph.size(), 8) << "Graph should have 8 nodes";
    EXPECT_EQ(graph[0].size(), 3) << "Node 0 should have 3 outgoing edges";
    EXPECT_EQ(graph[7].size(), 0) << "Node 7 should have no outgoing edges";
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
