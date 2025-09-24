#pragma once

#include <algorithm>
#include <cstdint>
#include <stack>
#include <stdexcept>
#include <vector>

namespace algo::dfs {

using u64 = std::uint64_t;
using i64 = std::int64_t;
using Bridge = std::pair<u64, u64>;

template <typename T> constexpr u64 to_uint(T x) {
    if (x < 0) {
        throw std::runtime_error("Cannot cast a negative to unsigned!");
    }
    return static_cast<u64>(x);
}

template <typename T> constexpr i64 to_int(T x) { return static_cast<i64>(x); }

/**
 * Store the results of the DFS.
 * This allows us to solve multiple different types of graph/tree traversal problems.
 */
struct DFSResult {
    bool hasCycle;               // Detect cycles in undirected or directed graphs.
    std::vector<i64> parent;     // Used to recreate the path through the graph.
    std::vector<i64> components; // Used for connectivity checks.
    std::vector<i64> entry;      // Track the order of processing.
    std::vector<i64> exit;       // Used to track ancestors.
    std::vector<i64> low;        // Used to find the critical components of a graph.
    std::vector<u64> articulationPoints; // Critical nodes in an undirected graph.
    std::vector<Bridge> bridges;         // Critical edges in an undirected graph.
    std::vector<std::vector<u64>>
        sccs;                     // Strongly-connected components in a directed graph.
    std::stack<u64> sccStack;     // Track current component stack.
    std::vector<uint8_t> onStack; // Tarjan's SCC algorithm.
    std::vector<i64> sccIndex;    // Component ID for each node in Tarjan's.
    std::vector<u64> postOrder;   // Track post order of traversal.
    std::vector<u64> topoOrder;   // Only useful in a DAG.
};

class DFSDriver {
  private:
    i64 timer;
    i64 currentComponent;
    i64 currSCC;

    void undirectedDfs(std::vector<std::vector<u64>> &adj, std::vector<uint8_t> &visited,
                       DFSResult &res, u64 u, i64 p = -1) {
        /**
         * 0 = not visited
         * 1 = visiting
         */
        visited[u] = 1;

        res.parent[u] = p;
        res.components[u] = currentComponent; // Flood fill all nodes in the same DFS.

        res.entry[u] = res.low[u] = timer++;
        u64 children = 0;
        bool isArticulation = false;

        for (u64 v : adj[u]) {
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

    void directedDfs(std::vector<std::vector<u64>> &adj, std::vector<uint8_t> &visited,
                     DFSResult &res, u64 u, i64 p = -1) {
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

        for (u64 v : adj[u]) {
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
            std::vector<u64> components;
            while (!res.sccStack.empty()) {
                u64 v = res.sccStack.top();
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
    DFSResult runDFS(std::vector<std::vector<u64>> &adj, bool isDirected = false) {
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
    std::vector<u64> getPath(i64 u, DFSResult &res) {
        std::vector<u64> path;
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
    bool isAncestor(u64 u, u64 v, const DFSResult &res) {
        return (res.entry[u] <= res.entry[v]) && (res.exit[v] <= res.exit[u]);
    }
};

} // namespace algo::dfs
