#pragma once

#include <cstdint>
#include <limits>
#include <queue>
#include <vector>

namespace algo::graph {

using ul = std::size_t;
using ll = long long;
using Edge = std::pair<ul, ll>;

constexpr ll INF = std::numeric_limits<ll>::max();

/**
 * Bellman–Ford: single-source shortest paths with negative weights.
 *
 * - Handles negative edges
 * - Detects negative cycles reachable from the source
 * - O(VE)
 */
inline std::vector<ll> BF(const std::vector<std::vector<Edge>> &adj, const ul start) {
    const ul n = adj.size();
    std::vector<ll> dist(n, INF);
    dist[start] = 0;
    for (ul i = 0; i + 1 < n; i++) {
        bool relaxed = false;
        for (ul u = 0; u < n; u++) {
            if (dist[u] == INF) {
                continue;
            }
            for (const auto &[v, w] : adj[u]) {
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
    std::queue<ul> neg;
    for (ul u = 0; u < n; u++) {
        if (dist[u] == INF) {
            continue;
        }
        for (const auto &[v, w] : adj[u]) {
            if (dist[u] + w < dist[v]) {
                neg.push(v);
                dist[v] = -INF;
            }
        }
    }
    while (!neg.empty()) {
        const ul at = neg.front();
        neg.pop();
        for (const auto &[next, w] : adj[at]) {
            if (dist[next] != -INF) {
                dist[next] = -INF;
                neg.push(next);
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
    std::vector<ll> dist(n, INF);
    std::vector<uint8_t> inQ(n, 0);
    std::vector<ul> count(n, 0);
    std::queue<ul> q;
    dist[source] = 0;
    q.push(source);
    inQ[source] = 1;
    std::queue<ul> neg;
    while (!q.empty()) {
        const ul u = q.front();
        q.pop();
        inQ[u] = 0;
        for (const auto &[v, w] : adj[u]) {
            if (count[v] > n) {
                dist[v] = -INF;
                neg.push(v);
                continue;
            }
            const ll nd = dist[u] + w;
            if (nd < dist[v]) {
                dist[v] = nd;
                if (!inQ[v]) {
                    q.push(v);
                    inQ[v] = 1;
                    count[v]++;
                }
            }
        }
    }
    while (!neg.empty()) {
        const ul at = neg.front();
        neg.pop();
        for (const auto &[next, w] : adj[at]) {
            if (dist[next] != -INF) {
                dist[next] = -INF;
                neg.push(next);
            }
        }
    }
    return dist;
}

} // namespace algo::graph
