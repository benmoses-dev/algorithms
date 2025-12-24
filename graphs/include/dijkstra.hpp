#pragma once

#include <algorithm>
#include <climits>
#include <cstdint>
#include <limits>
#include <queue>
#include <stdexcept>
#include <vector>

namespace algo::graph {

using ul = std::size_t;
using ll = long long;
using edge = std::pair<ul, ll>;

template <typename T> constexpr ul to_uint(T x) {
    if (x < 0) {
        throw std::runtime_error("Cannot cast a negative to unsigned!");
    }
    return static_cast<ul>(x);
}

template <typename T> constexpr ll to_int(T x) { return static_cast<ll>(x); }

/**
 * Shortest path algorithms:
 *
 * - Single-source, non-negative weights:
 *     use dijkstra(start)
 *
 * - All-pairs, possibly negative weights (no negative cycles):
 *     use johnson()
 *
 * DAG shortest paths and Bellman-Ford are intentionally excluded
 * from this header.
 */
class ShortestPaths {
  private:
    const std::vector<std::vector<edge>> &adj;
    const ul n;
    std::vector<ll> dist;
    std::vector<ll> prev;
    std::vector<ll> potential;
    bool usePotentials;

    void computePotentials() {
        std::fill(potential.begin(), potential.end(), 0);
        std::queue<ul> q;
        std::vector<uint8_t> inQueue(n, 1);
        std::vector<ul> relaxCount(n, 0);
        for (ul i = 0; i < n; i++) {
            q.push(i);
        }
        while (!q.empty()) {
            const ul u = q.front();
            q.pop();
            inQueue[u] = 0;
            for (const auto &[v, w] : adj[u]) {
                const ll nd = potential[u] + w;
                if (nd < potential[v]) {
                    potential[v] = nd;
                    if (!inQueue[v]) {
                        q.push(v);
                        inQueue[v] = 1;
                        if (++relaxCount[v] >= n) {
                            throw std::runtime_error("Negative cycle detected");
                        }
                    }
                }
            }
        }
    }

    void dijkstraInternal(const ul start) {
        const ll INF = std::numeric_limits<ll>::max();
        std::fill(dist.begin(), dist.end(), INF);
        std::fill(prev.begin(), prev.end(), -1);
        std::priority_queue<std::pair<ll, ul>, std::vector<std::pair<ll, ul>>,
                            std::greater<std::pair<ll, ul>>>
            pq;
        dist[start] = 0;
        pq.push({0, start});
        while (!pq.empty()) {
            const auto [d, u] = pq.top();
            pq.pop();
            if (d > dist[u]) {
                continue;
            }
            for (const auto &[v, w] : adj[u]) {
                const ll cost = w + (usePotentials ? potential[u] - potential[v] : 0);
                const ll nd = d + cost;
                if (nd < dist[v]) {
                    dist[v] = nd;
                    prev[v] = to_int(u);
                    pq.push({nd, v});
                }
            }
        }
    }

  public:
    explicit ShortestPaths(const std::vector<std::vector<edge>> &graph)
        : adj(graph), n(graph.size()), dist(n), prev(n), potential(n),
          usePotentials(false) {}

    /**
     * Single-source shortest path.
     *
     * Preconditions:
     * - All edge weights must be non-negative.
     */
    std::vector<ll> dijkstra(const ul start) {
        usePotentials = false;
        dijkstraInternal(start);
        return dist;
    }

    /**
     * All-pairs shortest paths using Johnson's algorithm.
     *
     * Preconditions:
     * - Graph may contain negative weights
     * - No negative cycles
     */
    std::vector<std::vector<ll>> johnson() {
        computePotentials();
        usePotentials = true;
        std::vector<std::vector<ll>> result(n, std::vector<ll>(n));
        const ll INF = std::numeric_limits<ll>::max();
        for (ul s = 0; s < n; s++) {
            dijkstraInternal(s);
            for (ul v = 0; v < n; v++) {
                if (dist[v] != INF) {
                    result[s][v] = dist[v] + potential[v] - potential[s];
                }
            }
        }
        return result;
    }

    /**
     * Path reconstruction helper (after Dijkstra).
     */
    std::vector<ul> reconstructPath(const ul end) const {
        std::vector<ul> path;
        ul curr = end;
        while (true) {
            path.emplace_back(curr);
            if (prev[curr] < 0) {
                break;
            }
            curr = to_uint(prev[curr]);
        }
        std::reverse(path.begin(), path.end());
        return path;
    }
};

} // namespace algo::graph
