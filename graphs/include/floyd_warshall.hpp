#pragma once

#include <cstdint>
#include <limits>
#include <optional>
#include <queue>
#include <vector>

namespace algo::graph {

using ul = std::size_t;
using ll = long long;
using edge = std::pair<ul, ll>;

/**
 * Floyd–Warshall: All-pairs shortest paths.
 *
 * - Supports negative weights (also catches negative cycles)
 * - O(V^3)
 */
class FloydWarshall {
  private:
    const ul n;
    std::vector<std::vector<ll>> dist;
    std::vector<std::vector<std::optional<ul>>> next;

  public:
    explicit FloydWarshall(const ul vertices)
        : n(vertices), dist(vertices, std::vector<ll>(vertices)),
          next(vertices, std::vector<std::optional<ul>>(vertices, std::nullopt)) {
        const ll INF = std::numeric_limits<ll>::max();
        for (ul i = 0; i < n; i++) {
            for (ul j = 0; j < n; j++) {
                dist[i][j] = (i == j ? 0 : INF);
            }
        }
    }

    void addEdge(const ul u, const ul v, const ll w) {
        if (dist[u][v] > w) {
            dist[u][v] = w;
            next[u][v] = v;
        }
    }

    std::vector<std::vector<ll>> compute() {
        const ll INF = std::numeric_limits<ll>::max();
        for (ul k = 0; k < n; k++) {
            for (ul i = 0; i < n; i++) {
                if (dist[i][k] == INF) {
                    continue;
                }
                for (ul j = 0; j < n; j++) {
                    if (dist[k][j] == INF) {
                        continue;
                    }
                    const ll nd = dist[i][k] + dist[k][j];
                    if (nd < dist[i][j]) {
                        dist[i][j] = nd;
                        next[i][j] = next[i][k];
                    }
                }
            }
        }
        std::queue<ul> neg;
        std::vector<std::uint8_t> inQ(n, 0);
        for (ul i = 0; i < n; i++) {
            if (dist[i][i] < 0) {
                neg.push(i);
                inQ[i] = 1;
            }
        }
        while (!neg.empty()) {
            const ul at = neg.front();
            neg.pop();
            for (ul i = 0; i < n; i++) {
                for (ul j = 0; j < n; j++) {
                    if (dist[i][at] != INF && dist[at][j] != INF) {
                        dist[i][j] = -INF;
                        if (!inQ[j]) {
                            inQ[j] = 1;
                            neg.push(j);
                        }
                    }
                }
            }
        }
        return dist;
    }

    std::vector<ul> reconstructPath(ul u, const ul v) const {
        const ll INF = std::numeric_limits<ll>::max();
        std::vector<ul> path;
        if (dist[u][v] == INF || !next[u][v].has_value()) {
            return path;
        }
        ul curr = u;
        while (curr != v) {
            path.emplace_back(curr);
            curr = next[u][v].value();
            u = curr;
        }
        path.emplace_back(v);
        return path;
    }
};

} // namespace algo::graph
