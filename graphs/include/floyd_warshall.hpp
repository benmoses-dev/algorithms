#pragma once

#include <limits>
#include <optional>
#include <stdexcept>
#include <vector>

namespace algo::graph {

using ul = std::size_t;
using ll = long long;
using edge = std::pair<ul, ll>;

/**
 * Floyd–Warshall: All-pairs shortest paths.
 *
 * - Supports negative weights (no negative cycles)
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

    void compute() {
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

        for (ul i = 0; i < n; i++) {
            if (dist[i][i] < 0) {
                throw std::runtime_error("Negative cycle detected");
            }
        }
    }

    const std::vector<std::vector<ll>> &getDistances() const { return dist; }

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
