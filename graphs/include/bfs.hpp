#pragma once

#include <queue>
#include <stdexcept>
#include <vector>

namespace algo::graph {

using ll = long long;

/**
 * Generic BFS algorithm.
 * Tracks whether the graph has cycles or is bipartite, components, and shortest paths.
 */
inline bool adjBFS(const std::vector<std::vector<std::size_t>> &adj) {
    const std::size_t n = adj.size();

    // Track state
    bool hasCycle = false;
    bool isBipartite = true;
    std::vector<int> prev(n, -1);
    std::vector<int> colours(n, -1);
    std::vector<int> distances(n, -1);

    // Assign component IDs
    int componentId = 0;
    std::vector<int> components(n, -1);

    /**
     * Remove the loop and pass in an index to only carry out a BFS from one starting node
     * instead of all nodes.
     */
    for (std::size_t i = 0; i < n; i++) {
        if (colours[i] != -1) {
            continue;
        }

        std::queue<std::size_t> q;
        q.push(i);
        colours[i] = 0;
        components[i] = componentId;
        distances[i] = 0;

        while (!q.empty()) {
            const std::size_t next = q.front();
            q.pop();

            /**
             * Start a BFS through all neighbours.
             * If we get back to a node that has already been visited, we have a cycle.
             */
            for (const std::size_t &neighbour : adj[next]) {
                if (colours[neighbour] == -1) {
                    colours[neighbour] = colours[next] ^ 1;
                    components[neighbour] = componentId;
                    distances[neighbour] = distances[next] + 1;
                    prev[neighbour] = static_cast<int>(next);
                    q.push(neighbour);
                } else {
                    if (prev[next] != static_cast<int>(neighbour)) {
                        hasCycle = true;
                        if (colours[next] == colours[neighbour]) {
                            isBipartite = false;
                        }
                    }
                }
            }
        }
        componentId++;
    }

    return hasCycle && isBipartite; // Alter this as necessary.
}

/**
 * BFS on a grid. No need to track cycles.
 */
inline std::vector<std::vector<std::pair<int, int>>>
gridBFS(const std::vector<std::vector<int>> &grid) {
    const std::size_t r = grid.size();
    const std::size_t c = grid[0].size();

    std::vector<std::vector<std::pair<int, int>>> prev(
        r, std::vector<std::pair<int, int>>(c));

    // Track state
    std::vector<std::vector<int>> colours(r, std::vector<int>(c, -1));
    std::vector<std::vector<int>> distances(r, std::vector<int>(c, -1));

    // Assign component IDs
    int componentId = 0;
    std::vector<std::vector<int>> components(r, std::vector<int>(c, -1));

    for (std::size_t i = 0; i < r; i++) {
        for (std::size_t j = 0; j < c; j++) {
            if (colours[i][j] != -1) {
                continue;
            }

            const int dr[] = {-1, 1, 0, 0};
            const int dc[] = {0, 0, -1, 1};

            prev[i][j] = {-1, -1};

            std::queue<std::pair<std::size_t, std::size_t>> q;
            q.push({i, j});
            colours[i][j] = 0;
            components[i][j] = componentId;
            distances[i][j] = 0;

            while (!q.empty()) {
                const auto &[row, col] = q.front();
                q.pop();
                for (std::size_t d = 0; d < 4; d++) {
                    const int nr = dr[d] + static_cast<int>(row);
                    const int nc = dc[d] + static_cast<int>(col);
                    if (nr < 0 || nr >= static_cast<int>(r)) {
                        continue;
                    }
                    if (nc < 0 || nc >= static_cast<int>(c)) {
                        continue;
                    }
                    const std::size_t rIdx = static_cast<std::size_t>(nr);
                    const std::size_t cIdx = static_cast<std::size_t>(nc);
                    if (colours[rIdx][cIdx] == -1) {
                        colours[rIdx][cIdx] = colours[row][col] ^ 1;
                        components[rIdx][cIdx] = componentId;
                        distances[rIdx][cIdx] = distances[row][col] + 1;
                        prev[rIdx][cIdx] = {row, col};
                        q.push({nr, nc});
                    }
                }
            }
            componentId++;
        }
    }

    return prev;
}

/**
 * Kahn's algorithm for topological sort in a DAG.
 * This is an alternative to using DFS, and it can be used to compute build dependencies
 * or path lengths.
 */
inline std::vector<std::size_t>
kahn(const std::vector<std::vector<std::pair<std::size_t, ll>>> &adj) {
    const std::size_t n = adj.size();
    std::vector<std::size_t> indeg(n, 0);
    for (std::size_t u = 0; u < n; u++) {
        for (const auto &[v, w] : adj[u]) {
            indeg[v]++;
        }
    }
    // Use a priority queue here to force ordering if necessary.
    std::queue<std::size_t> q;
    for (std::size_t u = 0; u < n; u++) {
        if (indeg[u] == 0) {
            q.push(u);
        }
    }
    std::vector<std::size_t> topo;
    while (!q.empty()) {
        const std::size_t u = q.front();
        q.pop();
        topo.emplace_back(u);
        for (const auto &[v, w] : adj[u]) {
            indeg[v]--;
            if (indeg[v] == 0) {
                q.push(v);
            }
        }
    }
    if (topo.size() != n) {
        throw std::runtime_error("Graph is not a DAG");
    }
    return topo;
}

/**
 * Find the weighted longest path in a DAG using dynamic programming and BFS.
 * This assumes an empty path is allowed and any start node is allowed. Otherwise,
 * initialise dp to -LINF and set dp[source] = 0.
 *
 * For shortest path, see the DFS header (implementation will be identical whether
 * topological order is generated using Kahn's BFS or DFS).
 */
inline ll longestPath(const std::vector<std::vector<std::pair<std::size_t, ll>>> &adj,
                      const std::vector<std::size_t> &topo) {
    const std::size_t n = adj.size();
    std::vector<ll> dp(n, 0);
    ll maxLen = 0;
    for (const std::size_t u : topo) {
        for (const auto &[v, w] : adj[u]) {
            dp[v] = std::max(dp[v], dp[u] + w);
            maxLen = std::max(dp[v], maxLen);
        }
    }
    return maxLen;
}

} // namespace algo::graph
