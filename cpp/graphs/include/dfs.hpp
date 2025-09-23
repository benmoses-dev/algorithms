#pragma once

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <stack>
#include <vector>

namespace algo::dfs {

using Bridge = std::pair<int, int>;
using ul = unsigned long;

template <typename T> constexpr ul to_ul(T x) {
    assert(x >= 0);
    return static_cast<ul>(x);
}

template <typename T> constexpr int to_int(T x) { return static_cast<int>(x); }

/**
 * Store the results of the DFS.
 * This allows us to solve multiple different types of graph/tree traversal problems.
 */
struct DFSResult {
    bool hasCycle;               // Detect cycles in undirected or directed graphs.
    std::vector<int> parent;     // Used to recreate the path through the graph.
    std::vector<int> components; // Used for connectivity checks.
    std::vector<int> entry;      // Track the order of processing.
    std::vector<int> exit;       // Used to track ancestors.
    std::vector<int> low;        // Used to find the critical components of a graph.
    std::vector<ul> articulationPoints; // Critical nodes in an undirected graph.
    std::vector<Bridge> bridges;        // Critical edges in an undirected graph.
    std::vector<std::vector<ul>>
        sccs;                     // Strongly-connected components in a directed graph.
    std::stack<ul> sccStack;      // Track current component stack.
    std::vector<uint8_t> onStack; // Tarjan's SCC algorithm.
    std::vector<int> sccIndex;    // Component ID for each node in Tarjan's.
    std::vector<ul> postOrder;    // Track post order of traversal.
    std::vector<ul> topoOrder;    // Only useful in a DAG.
};

class DFSDriver {
  private:
    int timer;
    int currentComponent;
    int currSCC;

    void undirectedDfs(std::vector<std::vector<ul>> &adj, std::vector<uint8_t> &visited,
                       DFSResult &res, ul u, int p = -1) {
        /**
         * 0 = not visited
         * 1 = visiting
         */
        visited[u] = 1;

        res.parent[u] = p;
        res.components[u] = currentComponent; // Flood fill all nodes in the same DFS.

        res.entry[u] = res.low[u] = timer++;
        ul children = 0;
        bool isArticulation = false;

        for (ul v : adj[u]) {
            if (p < 0 || v == to_ul(p)) // Don't go back up the edge.
                continue;

            if (visited[v] == 0) {
                undirectedDfs(adj, visited, res, v, static_cast<int>(u));

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

                if (p >= 0 && v != to_ul(p)) {
                    // Back edge => cycle
                    res.hasCycle = true;
                }
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

    void directedDfs(std::vector<std::vector<ul>> &adj, std::vector<uint8_t> &visited,
                     DFSResult &res, ul u, int p = -1) {
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

        for (ul v : adj[u]) {
            if (visited[v] == 0) {
                // Recurse and then update the low-link.
                directedDfs(adj, visited, res, v, static_cast<int>(u));
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
            std::vector<ul> components;
            while (!res.sccStack.empty()) {
                ul v = res.sccStack.top();
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
    DFSResult runDFS(std::vector<std::vector<ul>> &adj, bool isDirected = false) {
        ul n = adj.size();

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

        for (ul u = 0; u < n; u++) {
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
    std::vector<ul> getPath(int u, DFSResult &res) {
        std::vector<ul> path;
        while (u >= 0) {
            path.push_back(to_ul(u));
            u = res.parent[to_ul(u)];
        }
        reverse(path.begin(), path.end());
        return path;
    }

    /**
     * Is u an ancestor of v?
     */
    bool isAncestor(ul u, ul v, const DFSResult &res) {
        return (res.entry[u] <= res.entry[v]) && (res.exit[v] <= res.exit[u]);
    }
};

} // namespace algo::dfs
