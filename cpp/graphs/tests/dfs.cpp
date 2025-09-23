#include "dfs.hpp"
#include <cassert>
#include <iostream>
#include <vector>

using namespace algo::dfs;

int main() {
    DFSDriver dfs;

    // Undirected Graph Tests
    {
        // 0 -- 1 -- 2
        //      |
        //      3
        std::vector<std::vector<std::uint64_t>> adj(4);
        adj[0] = {1};
        adj[1] = {0, 2, 3};
        adj[2] = {1};
        adj[3] = {1};

        DFSResult res = dfs.runDFS(adj, false);

        // Connectivity
        assert(res.components[0] == res.components[1]);
        assert(res.components[1] == res.components[2]);
        assert(res.components[1] == res.components[3]);

        // No cycles
        assert(!res.hasCycle);

        // Articulation point is node 1
        assert(res.articulationPoints.size() == 1);
        assert(res.articulationPoints[0] == 1);

        // Bridges: (0,1), (1,2), (1,3)
        assert(res.bridges.size() == 3);
        // Order might vary, so check presence
        bool b01 = false, b12 = false, b13 = false;
        for (auto &br : res.bridges) {
            if ((br.first == 0 && br.second == 1) || (br.first == 1 && br.second == 0))
                b01 = true;
            if ((br.first == 1 && br.second == 2) || (br.first == 2 && br.second == 1))
                b12 = true;
            if ((br.first == 1 && br.second == 3) || (br.first == 3 && br.second == 1))
                b13 = true;
        }
        assert(b01 && b12 && b13);

        // Path reconstruction: 0 -> 1 -> 2
        auto path = dfs.getPath(2, res);
        assert((path == std::vector<std::uint64_t>{0, 1, 2}));

        std::cout << "Undirected graph tests passed!\n";
    }

    //  Undirected Graph with Cycle
    {
        // Triangle: 0-1-2-0
        std::vector<std::vector<std::uint64_t>> adj(3);
        adj[0] = {1, 2};
        adj[1] = {0, 2};
        adj[2] = {0, 1};

        DFSResult res = dfs.runDFS(adj, false);

        // Should detect a cycle
        assert(res.hasCycle);

        // No articulation points in a triangle
        assert(res.articulationPoints.empty());

        // No bridges in a cycle
        assert(res.bridges.empty());

        std::cout << "Undirected cycle graph tests passed!\n";
    }

    //  Directed Graph Tests (DAG)
    {
        // 0 -> 1 -> 2 -> 3
        std::vector<std::vector<std::uint64_t>> adj(4);
        adj[0] = {1};
        adj[1] = {2};
        adj[2] = {3};
        adj[3] = {};

        DFSResult res = dfs.runDFS(adj, true);

        // No cycles
        assert(!res.hasCycle);

        // One SCC per node (since it’s a DAG)
        assert(res.sccs.size() == 4);

        // Topological order must be [0,1,2,3]
        assert((res.topoOrder == std::vector<std::uint64_t>{0, 1, 2, 3}));

        // Path reconstruction: 0 -> 1 -> 2 -> 3
        auto path = dfs.getPath(3, res);
        assert((path == std::vector<std::uint64_t>{0, 1, 2, 3}));

        std::cout << "Directed DAG tests passed!\n";
    }

    //  Directed Graph with Cycle (SCCs)
    {
        // 0 -> 1 -> 2 -> 0  (cycle)
        // 2 -> 3
        std::vector<std::vector<std::uint64_t>> adj(4);
        adj[0] = {1};
        adj[1] = {2};
        adj[2] = {0, 3};
        adj[3] = {};

        DFSResult res = dfs.runDFS(adj, true);

        // Should detect a cycle
        assert(res.hasCycle);

        // SCCs: {0,1,2}, {3}
        assert(res.sccs.size() == 2);
        bool foundBig = false, foundSingle = false;
        for (auto &scc : res.sccs) {
            if (scc.size() == 3)
                foundBig = true;
            if (scc.size() == 1 && scc[0] == 3)
                foundSingle = true;
        }
        assert(foundBig && foundSingle);

        // No topo order because graph has cycle
        assert(res.topoOrder.empty());

        std::cout << "Directed cycle graph (SCC) tests passed!\n";
    }

    std::cout << "All DFS tests passed!\n";
    return 0;
}
