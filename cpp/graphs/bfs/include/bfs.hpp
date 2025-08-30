#pragma once

#include <cstddef>
#include <queue>
#include <utility>
#include <vector>

namespace algo::bfs {

using vi = std::vector<int>;
using vvi = std::vector<vi>;
using vvvi = std::vector<vvi>;
using pii = std::pair<int, int>;
using vpii = std::vector<pii>;
using vvpii = std::vector<vpii>;

/**
 * Generic BFS algorithm to calculate the shortest path between the start and all other
 * nodes.
 * Thread safe, so can be called concurrently to do a multi-source BFS.
 */
inline vi adjBFS(const vvi &adj, const int start) {
    size_t n = adj.size();

    // Track distance and whether a node has been visited (to prevent infinite loops)
    vi visited(n, 0);
    vi distances(n, -1);

    std::queue<int> q;
    q.push(start);
    visited[start] = 1;   // Visit the start
    distances[start] = 0; // Back to itself = 0

    while (!q.empty()) {
        int next = q.front();
        q.pop();

        for (const int &neighbour : adj[next]) {
            if (visited[neighbour] == 0) { // Only process if this is the first time
                visited[neighbour] = 1;    // We are now visiting this node
                distances[neighbour] = distances[next] + 1; // One more hop
                q.push(neighbour);
            }
        }
    }

    return distances; // Adjust the return value as necessary
}

/**
 * So this is pretty much the same idea as above, but we are moving in set directions,
 * instead of using an adjacency list of neighbours.
 */
inline vvi gridBFS(const vvi &grid, const int startRow, const int startCol) {
    int r = (int)grid.size();
    int c = (int)grid[0].size();

    vvi visited(r, vi(c, 0));
    vvi distances(r, vi(c, -1));

    int dr[] = {-1, 1, 0, 0};
    int dc[] = {0, 0, -1, 1};

    std::queue<pii> q;
    q.push({startRow, startCol});
    visited[startRow][startCol] = 1;
    distances[startRow][startCol] = 0;

    while (!q.empty()) {
        auto [row, col] = q.front();
        q.pop();
        for (int d = 0; d < 4; d++) {
            int nr = dr[d] + row;
            int nc = dc[d] + col;
            if (nr < 0 || nr >= r)
                continue;
            if (nc < 0 || nc >= c)
                continue;
            if (visited[nr][nc] == 0) {
                visited[nr][nc] = 1;
                distances[nr][nc] = distances[row][col] + 1;
                q.push({nr, nc});
            }
        }
    }

    return distances;
}

} // namespace algo::bfs
