#pragma once

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <stack>
#include <vector>

namespace algo::dfs {

using Bridge = std::pair<std::uint64_t, std::uint64_t>;

template <typename T> constexpr std::uint64_t to_uint(T x) {
    assert(x >= 0);
    return static_cast<std::uint64_t>(x);
}

template <typename T> constexpr std::int64_t to_int(T x) {
    return static_cast<std::int64_t>(x);
}

/**
 * Store the results of the DFS.
 * This allows us to solve multiple different types of graph/tree traversal problems.
 */
struct DFSResult {
    bool hasCycle;                    // Detect cycles in undirected or directed graphs.
    std::vector<std::int64_t> parent; // Used to recreate the path through the graph.
    std::vector<std::int64_t> components; // Used for connectivity checks.
    std::vector<std::int64_t> entry;      // Track the order of processing.
    std::vector<std::int64_t> exit;       // Used to track ancestors.
    std::vector<std::int64_t> low; // Used to find the critical components of a graph.
    std::vector<std::uint64_t>
        articulationPoints;      // Critical nodes in an undirected graph.
    std::vector<Bridge> bridges; // Critical edges in an undirected graph.
    std::vector<std::vector<std::uint64_t>>
        sccs; // Strongly-connected components in a directed graph.
    std::stack<std::uint64_t> sccStack;   // Track current component stack.
    std::vector<uint8_t> onStack;         // Tarjan's SCC algorithm.
    std::vector<std::int64_t> sccIndex;   // Component ID for each node in Tarjan's.
    std::vector<std::uint64_t> postOrder; // Track post order of traversal.
    std::vector<std::uint64_t> topoOrder; // Only useful in a DAG.
};

class DFSDriver {
  private:
    std::int64_t timer;
    std::int64_t currentComponent;
    std::int64_t currSCC;

    void undirectedDfs(std::vector<std::vector<std::uint64_t>> &adj,
                       std::vector<uint8_t> &visited, DFSResult &res, std::uint64_t u,
                       std::int64_t p = -1) {
        /**
         * 0 = not visited
         * 1 = visiting
         */
        visited[u] = 1;

        res.parent[u] = p;
        res.components[u] = currentComponent; // Flood fill all nodes in the same DFS.

        res.entry[u] = res.low[u] = timer++;
        std::uint64_t children = 0;
        bool isArticulation = false;

        for (std::uint64_t v : adj[u]) {
            if (p >= 0 && v == to_uint(p)) // Don't go back up the edge.
                continue;

            if (visited[v] == 0) {
                undirectedDfs(adj, visited, res, v, to_int(u));

                // v has now been processed (and is a descendant) - use its low-link
                // value.
                res.low[u] = std::min(res.low[u], res.low[v]);

                /**
                 * If v cannot get back up to u (or above) via another path, this is a
                 * bridge. Remove this edge disconnects the graph.
                 */
                if (res.low[v] > res.entry[u]) {
                    res.bridges.emplace_back(u, v);
                }

                /**
                 * If v cannot get back above u, this is an articulation point (even if it
                 * can get back to u). Removing this node disconnects the graph.
                 */
                if (res.low[v] >= res.entry[u] && p != -1) {
                    isArticulation = true;
                }

                children++;
            } else {
                /**
                 * v is being processed (we are still recursing, so it doesn't have a
                 * low-link value yet). Use its entry time as the low-link for everything
                 * on this route.
                 */
                res.low[u] = std::min(res.low[u], res.entry[v]);

                // Back edge => cycle
                res.hasCycle = true;
            }
        }

        // Root base case
        if (p == -1 && children > 1) {
            isArticulation = true;
        }

        if (isArticulation) {
            res.articulationPoints.push_back(u);
        }

        res.exit[u] = timer++;
        res.postOrder.push_back(u);
    }

    void directedDfs(std::vector<std::vector<std::uint64_t>> &adj,
                     std::vector<uint8_t> &visited, DFSResult &res, std::uint64_t u,
                     std::int64_t p = -1) {
        /**
         * 0 = not visited
         * 1 = visiting
         * We can track cycles using the stack.
         */
        visited[u] = 1;

        res.parent[u] = p;
        res.components[u] = currentComponent; // Track weakly connected components.

        // Detect bridges and articulation points
        res.entry[u] = res.low[u] = timer++;

        res.sccStack.push(u);
        res.onStack[u] = 1;

        for (std::uint64_t v : adj[u]) {
            if (visited[v] == 0) {
                // Recurse and then update the low-link.
                directedDfs(adj, visited, res, v, to_int(u));
                res.low[u] = std::min(res.low[u], res.low[v]);
            } else if (res.onStack[v] == 1) { // Currently being processed.
                res.low[u] = std::min(res.low[u], res.entry[v]);
                // Back edge => cycle
                res.hasCycle = true;
            }
            // Otherwise, it's a forward/cross edge
        }

        if (res.low[u] == res.entry[u]) {
            // Root of SCC - process all strongly connected components in this group.
            std::vector<std::uint64_t> components;
            while (!res.sccStack.empty()) {
                std::uint64_t v = res.sccStack.top();
                res.sccStack.pop();
                res.onStack[v] = 0;
                components.push_back(v);
                res.sccIndex[v] = currSCC; // Track strongly connected component IDs.
                if (v == u)                // We have reached the root.
                    break;
            }
            res.sccs.push_back(components);
            currSCC++;
        }

        res.exit[u] = timer++;
        res.postOrder.push_back(u);
    }

  public:
    DFSDriver() : timer(0), currentComponent(0), currSCC(0) {}

    /**
     * Pass isDirected depending on whether the graph is directed or
     * not.
     */
    DFSResult runDFS(std::vector<std::vector<std::uint64_t>> &adj,
                     bool isDirected = false) {
        std::size_t n = adj.size();

        DFSResult res;
        res.parent.assign(n, -1);
        res.components.assign(n, -1);
        res.sccIndex.assign(n, -1);
        res.entry.resize(n, -1);
        res.exit.resize(n, -1);
        res.low.resize(n, -1);
        res.hasCycle = false;
        res.onStack.assign(n, 0);
        std::vector<uint8_t> visited(n, 0);

        for (std::size_t u = 0; u < n; u++) {
            if (visited[u] == 0) {
                if (isDirected) {
                    directedDfs(adj, visited, res, u, -1);
                } else {
                    undirectedDfs(adj, visited, res, u, -1);
                }
                currentComponent++;
            }
        }

        // If the graph is a DAG, we can get the topological order.
        if (isDirected && !res.hasCycle) {
            res.topoOrder = res.postOrder;
            reverse(res.topoOrder.begin(), res.topoOrder.end());
        }

        return res;
    }

    /**
     * Re-construct the recursion path using the parent vector.
     */
    std::vector<std::uint64_t> getPath(std::int64_t u, DFSResult &res) {
        std::vector<std::uint64_t> path;
        while (u >= 0) {
            path.push_back(to_uint(u));
            u = res.parent[to_uint(u)];
        }
        reverse(path.begin(), path.end());
        return path;
    }

    /**
     * Is u an ancestor of v?
     */
    bool isAncestor(std::uint64_t u, std::uint64_t v, const DFSResult &res) {
        return (res.entry[u] <= res.entry[v]) && (res.exit[v] <= res.exit[u]);
    }
};

} // namespace algo::dfs
