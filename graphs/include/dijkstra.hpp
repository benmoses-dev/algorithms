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

const ll INF = std::numeric_limits<ll>::max();

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
    std::vector<ll> p;
    bool usePotentials;
    std::vector<std::uint8_t> neg;

    void SPFA() {
        std::fill(p.begin(), p.end(), 0);
        std::vector<uint8_t> inQ(n, 1);
        std::vector<ul> count(n, 0);
        std::queue<ul> q;
        for (ul i = 0; i < n; i++) {
            q.push(i);
        }
        std::queue<ul> negQ;
        while (!q.empty()) {
            const ul u = q.front();
            q.pop();
            inQ[u] = 0;
            for (const auto &[v, w] : adj[u]) {
                if (count[v] > n) {
                    neg[v] = 1;
                    negQ.push(v);
                    continue;
                }
                const ll nd = p[u] + w;
                if (nd < p[v]) {
                    p[v] = nd;
                    if (!inQ[v]) {
                        q.push(v);
                        inQ[v] = 1;
                        count[v]++;
                    }
                }
            }
        }
        std::vector<std::uint8_t> visited(n, 0);
        while (!negQ.empty()) {
            const ul at = negQ.front();
            negQ.pop();
            if (visited[at]) {
                continue;
            }
            visited[at] = 1;
            for (const auto &[next, w] : adj[at]) {
                if (!neg[next]) {
                    neg[next] = 1;
                    negQ.push(next);
                }
            }
        }
    }

    void BF() {
        std::fill(p.begin(), p.end(), 0);
        for (ul i = 0; i + 1 < n; i++) {
            bool relaxed = false;
            for (ul u = 0; u < n; u++) {
                for (const auto &[v, w] : adj[u]) {
                    const ll nd = p[u] + w;
                    if (nd < p[v]) {
                        p[v] = nd;
                        relaxed = true;
                    }
                }
            }
            if (!relaxed) {
                break;
            }
        }
        std::queue<ul> negQ;
        for (ul u = 0; u < n; u++) {
            for (const auto &[v, w] : adj[u]) {
                if (p[u] + w < p[v]) {
                    if (!neg[v]) {
                        neg[v] = 1;
                        negQ.push(v);
                    }
                }
            }
        }
        std::vector<std::uint8_t> visited(n, 0);
        while (!negQ.empty()) {
            const ul at = negQ.front();
            negQ.pop();
            if (visited[at]) {
                continue;
            }
            visited[at] = 1;
            for (const auto &[next, w] : adj[at]) {
                if (!neg[next]) {
                    neg[next] = 1;
                    negQ.push(next);
                }
            }
        }
    }

    void dijkstraInternal(const ul start) {
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
                const ll cost = w + (usePotentials ? p[u] - p[v] : 0);
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
        : adj(graph), n(graph.size()), dist(n), prev(n), p(n), usePotentials(false),
          neg(n, 0) {}

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
     * - Negative cycles are marked as -INF
     */
    std::vector<std::vector<ll>> johnson() {
        SPFA();
        // BF();
        usePotentials = true;
        std::vector<std::vector<ll>> result(n, std::vector<ll>(n));
        for (ul s = 0; s < n; s++) {
            if (neg[s]) {
                std::fill(result[s].begin(), result[s].end(), -INF);
                continue;
            }
            dijkstraInternal(s);
            for (ul v = 0; v < n; v++) {
                if (neg[v]) {
                    result[s][v] = -INF;
                } else if (dist[v] != INF) {
                    result[s][v] = dist[v] + p[v] - p[s];
                } else {
                    result[s][v] = INF;
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
