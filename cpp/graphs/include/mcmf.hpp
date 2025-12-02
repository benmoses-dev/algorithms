#include <algorithm>
#include <cstdint>
#include <limits>
#include <queue>
#include <vector>

namespace algo::flow {

using ll = long long;

const ll INF = std::numeric_limits<ll>::max();

struct Edge {
    std::size_t to;
    ll cap, flow, cost;
    std::size_t rev;
};

struct Flow {
    std::size_t from, to;
    ll cost, flow;
};

class MinCostMaxFlow {
  private:
    std::size_t n;
    std::vector<std::vector<Edge>> adj;
    std::vector<ll> dist;
    std::vector<int> parent;
    std::vector<std::size_t> parentEdge;
    std::vector<ll> potential;

    bool dijkstra(const std::size_t source, const std::size_t sink) {
        fill(dist.begin(), dist.end(), INF);
        fill(parent.begin(), parent.end(), -1);
        std::priority_queue<std::pair<ll, std::size_t>,
                            std::vector<std::pair<ll, std::size_t>>,
                            std::greater<std::pair<ll, std::size_t>>>
            pq;
        dist[source] = 0;
        pq.push({0, source});
        while (!pq.empty()) {
            auto [d, u] = pq.top();
            pq.pop();
            if (d > dist[u]) {
                continue;
            }
            for (std::size_t i = 0; i < adj[u].size(); i++) {
                const Edge &e = adj[u][i];
                if (e.cap > e.flow) {
                    const ll reduced_cost = e.cost + potential[u] - potential[e.to];
                    const ll new_dist = dist[u] + reduced_cost;
                    if (new_dist < dist[e.to]) {
                        dist[e.to] = new_dist;
                        parent[e.to] = (int)u;
                        parentEdge[e.to] = i;
                        pq.push({new_dist, e.to});
                    }
                }
            }
        }
        return dist[sink] != INF;
    }

  public:
    MinCostMaxFlow(const std::size_t N)
        : n(N), adj(N), dist(N), parent(N), parentEdge(N), potential(N) {}

    void addEdge(const std::size_t u, const std::size_t v, const ll cap, const ll cost) {
        adj[u].emplace_back(v, cap, 0, cost, adj[v].size());
        adj[v].emplace_back(u, 0, 0, -cost, adj[u].size() - 1);
    }

    std::pair<ll, ll> solve(const std::size_t source, const std::size_t sink) {
        ll totalFlow = 0;
        ll totalCost = 0;
        while (dijkstra(source, sink)) {
            for (std::size_t i = 0; i < n; i++) {
                if (dist[i] < INF) {
                    potential[i] += dist[i];
                }
            }
            ll pathFlow = INF;
            std::size_t v = sink;
            while (v != source) {
                std::size_t u = static_cast<std::size_t>(parent[v]);
                std::size_t i = parentEdge[v];
                const Edge &e = adj[u][i];
                const ll res = e.cap - e.flow;
                pathFlow = std::min(pathFlow, res);
                v = u;
            }
            v = sink;
            while (v != source) {
                const std::size_t u = static_cast<std::size_t>(parent[v]);
                const std::size_t i = parentEdge[v];
                Edge &e = adj[u][i];
                e.flow += pathFlow;
                adj[e.to][e.rev].flow -= pathFlow;
                totalCost += pathFlow * e.cost;
                v = u;
            }
            totalFlow += pathFlow;
        }
        return {totalFlow, totalCost};
    }

    std::vector<Flow> getFlows() const {
        std::vector<Flow> result;
        for (std::size_t u = 0; u < n; u++) {
            for (const auto &e : adj[u]) {
                if (e.cap > 0 && e.flow > 0) {
                    result.emplace_back(u, e.to, e.cost, e.flow);
                }
            }
        }
        return result;
    }

    void initPotentials(const std::size_t source) { // Only if negative
        fill(potential.begin(), potential.end(), INF);
        potential[source] = 0;
        std::queue<std::size_t> q;
        std::vector<uint8_t> inQueue(n, 0);
        q.push(source);
        inQueue[source] = 1;
        while (!q.empty()) {
            std::size_t u = q.front();
            q.pop();
            inQueue[u] = 0;
            for (const auto &e : adj[u]) {
                if (e.cap > 0 && potential[u] + e.cost < potential[e.to]) {
                    potential[e.to] = potential[u] + e.cost;
                    if (!inQueue[e.to]) {
                        q.push(e.to);
                        inQueue[e.to] = 1;
                    }
                }
            }
        }
    }

    void reset() {
        for (std::size_t u = 0; u < n; u++) {
            for (auto &e : adj[u]) {
                e.flow = 0;
            }
        }
    }
};

} // namespace algo::flow
