#include "dfs.hpp"
#include <algorithm>
#include <gtest/gtest.h>
#include <vector>

using namespace algo::graph;

class DFSTest : public ::testing::Test {
  protected:
    DFSDriver dfs;

    void SetUp() override {
        // Fresh DFSDriver for each test
    }

    bool hasBridge(const std::vector<std::pair<u64, u64>> &bridges, u64 u, u64 v) {
        for (const auto &br : bridges) {
            if ((br.first == u && br.second == v) || (br.first == v && br.second == u)) {
                return true;
            }
        }
        return false;
    }
};

TEST_F(DFSTest, UndirectedGraphConnectivity) {
    // Graph: 0 -- 1 -- 2
    //             |
    //             3
    std::vector<std::vector<u64>> adj(4);
    adj[0] = {1};
    adj[1] = {0, 2, 3};
    adj[2] = {1};
    adj[3] = {1};

    DFSResult res = dfs.runDFS(adj, false);

    // All nodes should be in the same component
    EXPECT_EQ(res.components[0], res.components[1])
        << "Nodes 0 and 1 should be in same component";
    EXPECT_EQ(res.components[1], res.components[2])
        << "Nodes 1 and 2 should be in same component";
    EXPECT_EQ(res.components[1], res.components[3])
        << "Nodes 1 and 3 should be in same component";
}

TEST_F(DFSTest, UndirectedGraphNoCycle) {
    // Graph: 0 -- 1 -- 2
    //             |
    //             3
    std::vector<std::vector<u64>> adj(4);
    adj[0] = {1};
    adj[1] = {0, 2, 3};
    adj[2] = {1};
    adj[3] = {1};

    DFSResult res = dfs.runDFS(adj, false);

    EXPECT_FALSE(res.hasCycle) << "Tree structure should not have cycles";
}

TEST_F(DFSTest, UndirectedGraphArticulationPoints) {
    // Graph: 0 -- 1 -- 2
    //             |
    //             3
    std::vector<std::vector<u64>> adj(4);
    adj[0] = {1};
    adj[1] = {0, 2, 3};
    adj[2] = {1};
    adj[3] = {1};

    DFSResult res = dfs.runDFS(adj, false);

    // Node 1 is the only articulation point
    ASSERT_EQ(res.articulationPoints.size(), 1)
        << "Should have exactly one articulation point";
    EXPECT_EQ(res.articulationPoints[0], 1) << "Node 1 should be the articulation point";
}

TEST_F(DFSTest, UndirectedGraphBridges) {
    // Graph: 0 -- 1 -- 2
    //             |
    //             3
    std::vector<std::vector<u64>> adj(4);
    adj[0] = {1};
    adj[1] = {0, 2, 3};
    adj[2] = {1};
    adj[3] = {1};

    DFSResult res = dfs.runDFS(adj, false);

    // Should have 3 bridges: (0,1), (1,2), (1,3)
    ASSERT_EQ(res.bridges.size(), 3) << "Should have exactly 3 bridges";

    EXPECT_TRUE(hasBridge(res.bridges, 0, 1)) << "Should have bridge between 0 and 1";
    EXPECT_TRUE(hasBridge(res.bridges, 1, 2)) << "Should have bridge between 1 and 2";
    EXPECT_TRUE(hasBridge(res.bridges, 1, 3)) << "Should have bridge between 1 and 3";
}

TEST_F(DFSTest, UndirectedGraphPathReconstruction) {
    // Graph: 0 -- 1 -- 2
    //             |
    //             3
    std::vector<std::vector<u64>> adj(4);
    adj[0] = {1};
    adj[1] = {0, 2, 3};
    adj[2] = {1};
    adj[3] = {1};

    DFSResult res = dfs.runDFS(adj, false);

    auto path = dfs.getPath(2, res);
    std::vector<u64> expected = {0, 1, 2};

    EXPECT_EQ(path, expected) << "Path from 0 to 2 should be [0, 1, 2]";
}

TEST_F(DFSTest, UndirectedGraphWithCycle) {
    // Triangle: 0-1-2-0
    std::vector<std::vector<u64>> adj(3);
    adj[0] = {1, 2};
    adj[1] = {0, 2};
    adj[2] = {0, 1};

    DFSResult res = dfs.runDFS(adj, false);

    EXPECT_TRUE(res.hasCycle) << "Triangle should contain a cycle";
    EXPECT_TRUE(res.articulationPoints.empty())
        << "Triangle should have no articulation points";
    EXPECT_TRUE(res.bridges.empty()) << "Triangle should have no bridges";
}

TEST_F(DFSTest, DirectedDAGNoCycle) {
    // DAG: 0 -> 1 -> 2 -> 3
    std::vector<std::vector<u64>> adj(4);
    adj[0] = {1};
    adj[1] = {2};
    adj[2] = {3};
    adj[3] = {};

    DFSResult res = dfs.runDFS(adj, true);

    EXPECT_FALSE(res.hasCycle) << "DAG should not have cycles";
}

TEST_F(DFSTest, DirectedDAGSCCs) {
    // DAG: 0 -> 1 -> 2 -> 3
    std::vector<std::vector<u64>> adj(4);
    adj[0] = {1};
    adj[1] = {2};
    adj[2] = {3};
    adj[3] = {};

    DFSResult res = dfs.runDFS(adj, true);

    // Each node should be in its own SCC
    ASSERT_EQ(res.sccs.size(), 4) << "DAG should have 4 SCCs (one per node)";
}

TEST_F(DFSTest, DirectedDAGTopologicalOrder) {
    // DAG: 0 -> 1 -> 2 -> 3
    std::vector<std::vector<u64>> adj(4);
    adj[0] = {1};
    adj[1] = {2};
    adj[2] = {3};
    adj[3] = {};

    DFSResult res = dfs.runDFS(adj, true);

    std::vector<u64> expected = {0, 1, 2, 3};
    EXPECT_EQ(res.topoOrder, expected) << "Topological order should be [0, 1, 2, 3]";
}

TEST_F(DFSTest, DirectedDAGPathReconstruction) {
    // DAG: 0 -> 1 -> 2 -> 3
    std::vector<std::vector<u64>> adj(4);
    adj[0] = {1};
    adj[1] = {2};
    adj[2] = {3};
    adj[3] = {};

    DFSResult res = dfs.runDFS(adj, true);

    auto path = dfs.getPath(3, res);
    std::vector<u64> expected = {0, 1, 2, 3};

    EXPECT_EQ(path, expected) << "Path from 0 to 3 should be [0, 1, 2, 3]";
}

TEST_F(DFSTest, DirectedGraphWithCycle) {
    // Cycle: 0 -> 1 -> 2 -> 0
    //        2 -> 3
    std::vector<std::vector<u64>> adj(4);
    adj[0] = {1};
    adj[1] = {2};
    adj[2] = {0, 3};
    adj[3] = {};

    DFSResult res = dfs.runDFS(adj, true);

    EXPECT_TRUE(res.hasCycle) << "Graph should contain a cycle";
}

TEST_F(DFSTest, DirectedGraphSCCStructure) {
    // Cycle: 0 -> 1 -> 2 -> 0
    //        2 -> 3
    std::vector<std::vector<u64>> adj(4);
    adj[0] = {1};
    adj[1] = {2};
    adj[2] = {0, 3};
    adj[3] = {};

    DFSResult res = dfs.runDFS(adj, true);

    // SCCs: {0,1,2}, {3}
    ASSERT_EQ(res.sccs.size(), 2) << "Should have 2 SCCs";

    bool foundBigSCC = false;
    bool foundSingleSCC = false;

    for (const auto &scc : res.sccs) {
        if (scc.size() == 3) {
            foundBigSCC = true;
            // Verify it contains 0, 1, 2
            std::vector<u64> sorted = scc;
            std::sort(sorted.begin(), sorted.end());
            EXPECT_EQ(sorted, (std::vector<u64>{0, 1, 2}))
                << "Large SCC should contain nodes 0, 1, 2";
        }
        if (scc.size() == 1 && scc[0] == 3) {
            foundSingleSCC = true;
        }
    }

    EXPECT_TRUE(foundBigSCC) << "Should find SCC with 3 nodes";
    EXPECT_TRUE(foundSingleSCC) << "Should find SCC with node 3";
}

TEST_F(DFSTest, DirectedGraphWithCycleNoTopoOrder) {
    // Cycle: 0 -> 1 -> 2 -> 0
    //        2 -> 3
    std::vector<std::vector<u64>> adj(4);
    adj[0] = {1};
    adj[1] = {2};
    adj[2] = {0, 3};
    adj[3] = {};

    DFSResult res = dfs.runDFS(adj, true);

    EXPECT_TRUE(res.topoOrder.empty())
        << "Graph with cycle should not have topological order";
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
