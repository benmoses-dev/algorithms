#pragma once

#include <algorithm>
#include <mutex>
#include <optional>
#include <queue>
#include <thread>
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
 * Generic BFS algorithm.
 * Tracks whether the graph has cycles or is bipartite and generates the shortest path
 * from the start to all other nodes.
 * Thread safe, so can be called concurrently to do a multi-source BFS.
 */
inline vi adjBFS(const vvi &adj, const int start) {
    size_t n = adj.size();

    bool hasCycle = false;
    bool isBipartite = true;
    vi prev(n, -1);
    vi colours(n, -1);
    vi distances(n, -1);

    std::queue<int> q;
    q.push(start);
    colours[start] = 0;
    distances[start] = 0;

    while (!q.empty()) {
        int next = q.front();
        q.pop();

        for (const int &neighbour : adj[next]) {
            if (colours[neighbour] == -1) {
                colours[neighbour] = colours[next] ^ 1;
                distances[neighbour] = distances[next] + 1;
                prev[neighbour] = next;
                q.push(neighbour);
            } else {
                if (prev[next] != neighbour) {
                    hasCycle = true;
                    if (colours[next] == colours[neighbour]) {
                        isBipartite = false;
                    }
                }
            }
        }
    }

    return distances; // Adjust the return value as necessary
}

inline vvi gridBFS(const vvi &grid, const int startRow, const int startCol) {
    int r = (int)grid.size();
    int c = (int)grid[0].size();

    vvpii prev(r, vpii(c));

    vvi colours(r, vi(c, -1));
    vvi distances(r, vi(c, -1));

    int dr[] = {-1, 1, 0, 0};
    int dc[] = {0, 0, -1, 1};

    prev[startRow][startCol] = {-1, -1};

    std::queue<pii> q;
    q.push({startRow, startCol});
    colours[startRow][startCol] = 0;
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
            if (colours[nr][nc] == -1) {
                colours[nr][nc] = colours[row][col] ^ 1;
                distances[nr][nc] = distances[row][col] + 1;
                prev[nr][nc] = {row, col};
                q.push({nr, nc});
            }
        }
    }

    return distances;
}

struct ThreadJoiner {
    std::vector<std::thread> &threads;
    ~ThreadJoiner() {
        for (auto &t : threads) {
            if (t.joinable()) {
                t.join();
            }
        }
    }
};

/**
 * Multi-threaded wrapper for BFS.
 * Each start point runs BFS independently in a separate thread.
 */
template <typename T, typename U, typename V>
inline T threadWrapper(const U &adj, const V &starts, std::optional<size_t> maxThreads) {
    size_t n = starts.size();
    T results(n);

    const size_t max_threads =
        maxThreads ? std::max((size_t)1, *maxThreads) // So we don't nuke our RAM usage
                   : std::max((size_t)1, (size_t)std::thread::hardware_concurrency());
    std::queue<size_t> q;
    for (size_t i = 0; i < n; i++) {
        q.push(i);
    }
    std::mutex qMut;
    std::vector<std::thread> threads;
    threads.reserve(max_threads);

    {
        ThreadJoiner joiner{threads};
        for (size_t i = 0; i < max_threads; i++) {
            threads.emplace_back([&adj, &results, &qMut, &q, &starts]() {
                while (true) {
                    size_t at;
                    {
                        std::lock_guard lock(qMut);
                        if (q.empty()) {
                            break;
                        }
                        at = q.front();
                        q.pop();
                    }
                    if constexpr (std::is_same_v<typename V::value_type, pii>) {
                        auto [row, col] = starts[at];
                        results[at] = gridBFS(adj, row, col);
                    } else {
                        auto start = starts[at];
                        results[at] = adjBFS(adj, start);
                    }
                }
            });
        }
    }

    return results;
}

/**
 * Wrappers to avoid template params.
 */
inline vvi multiBFS(const vvi &adj, const vi &starts, std::optional<size_t> maxThreads) {
    return threadWrapper<vvi, vvi, vi>(adj, starts, maxThreads);
}
inline vvvi multiGridBFS(const vvi &grid, const vpii &starts,
                         std::optional<size_t> maxThreads) {
    return threadWrapper<vvvi, vvi, vpii>(grid, starts, maxThreads);
}

} // namespace algo::bfs
