#pragma once

#include <cstdint>
#include <limits>
#include <queue>
#include <stdexcept>
#include <vector>

namespace algo::graph {

using ul = std::size_t;
using ll = long long;
using Edge = std::pair<ul, ll>;

/**
 * Bellman–Ford: single-source shortest paths with negative weights.
 *
 * - Handles negative edges
 * - Detects negative cycles reachable from the source
 * - O(VE)
 */
inline std::vector<ll> BF(const std::vector<std::vector<Edge>> &adjacencyList,
                          const ul start) {
    const ul n = adjacencyList.size();
    constexpr ll INF = std::numeric_limits<ll>::max();
    std::vector<ll> dist(n, INF);
    dist[start] = 0;
    for (ul iter = 0; iter + 1 < n; iter++) {
        bool relaxed = false;
        for (ul u = 0; u < n; u++) {
            if (dist[u] == INF) {
                continue;
            }
            for (const auto &[v, w] : adjacencyList[u]) {
                const ll nd = dist[u] + w;
                if (nd < dist[v]) {
                    dist[v] = nd;
                    relaxed = true;
                }
            }
        }
        if (!relaxed) {
            break;
        }
    }
    for (ul u = 0; u < n; u++) {
        if (dist[u] == INF) {
            continue;
        }
        for (const auto &[v, w] : adjacencyList[u]) {
            if (dist[u] + w < dist[v]) {
                throw std::runtime_error("Negative cycle detected");
            }
        }
    }
    return dist;
}

/**
 * SPFA: Optimisation of Bellman–Ford.
 *
 * - Same guarantees as BF
 * - Faster in typical cases
 * - Worst-case O(VE)
 */
inline std::vector<ll> spfa(const std::vector<std::vector<Edge>> &adj, const ul source) {
    const ul n = adj.size();
    constexpr ll INF = std::numeric_limits<ll>::max();
    std::vector<ll> dist(n, INF);
    std::vector<uint8_t> inQueue(n, 0);
    std::vector<ul> relaxCount(n, 0);
    std::queue<ul> q;
    dist[source] = 0;
    q.push(source);
    inQueue[source] = 1;
    while (!q.empty()) {
        const ul u = q.front();
        q.pop();
        inQueue[u] = 0;
        for (const auto &[v, w] : adj[u]) {
            const ll nd = dist[u] + w;
            if (nd < dist[v]) {
                dist[v] = nd;
                if (!inQueue[v]) {
                    q.push(v);
                    inQueue[v] = 1;
                    if (++relaxCount[v] > n) {
                        throw std::runtime_error("Negative cycle detected");
                    }
                }
            }
        }
    }
    return dist;
}

} // namespace algo::graph
