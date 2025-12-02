#pragma once

#include <algorithm>
#include <cstdint>
#include <limits>
#include <queue>
#include <stdexcept>
#include <vector>

namespace algo::flow {

using ll = long long;

const ll INF = std::numeric_limits<ll>::max();

struct Edge {
    std::size_t to;
    ll cap, flow;
    std::size_t rev;
};

struct Flow {
    std::size_t from, to;
    ll cap, flow;
};

class Dinic {
  private:
    std::size_t n;
    std::vector<std::vector<Edge>> adj;
    std::vector<ll> level;
    std::vector<std::size_t> iter;

    bool bfs(const std::size_t source, const std::size_t sink) {
        fill(level.begin(), level.end(), -1);
        std::queue<std::size_t> q;
        level[source] = 0;
        q.push(source);
        while (!q.empty()) {
            const std::size_t u = q.front();
            q.pop();
            for (const auto &e : adj[u]) {
                if (level[e.to] < 0 && e.cap > e.flow) {
                    level[e.to] = level[u] + 1;
                    q.push(e.to);
                }
            }
        }
        return level[sink] >= 0;
    }

    ll dfs(const std::size_t u, const std::size_t sink, const ll pushed) {
        if (u == sink || pushed == 0)
            return pushed;
        for (std::size_t &i = iter[u]; i < adj[u].size(); i++) {
            Edge &e = adj[u][i];
            const ll res = e.cap - e.flow;
            if (level[u] + 1 != level[e.to] || res <= 0) {
                continue;
            }
            const ll flow = dfs(e.to, sink, std::min(pushed, res));
            if (flow > 0) {
                e.flow += flow;
                adj[e.to][e.rev].flow -= flow;
                return flow;
            }
        }
        return 0;
    }

    std::vector<uint8_t> getReachable(const std::size_t source) const {
        std::vector<uint8_t> reachable(n, false);
        std::queue<std::size_t> q;
        q.push(source);
        reachable[source] = true;
        while (!q.empty()) {
            const std::size_t u = q.front();
            q.pop();
            for (const auto &e : adj[u]) {
                const ll res = e.cap - e.flow;
                if (!reachable[e.to] && res > 0) {
                    reachable[e.to] = true;
                    q.push(e.to);
                }
            }
        }
        return reachable;
    }

    bool isValidFlow() const {
        for (std::size_t u = 0; u < n; u++) {
            for (const auto &e : adj[u]) {
                if (e.flow > e.cap || e.flow < 0) {
                    return false;
                }
            }
        }
        return true;
    }

    bool checkFlowConservation(const std::size_t v, const std::size_t source,
                               const std::size_t sink) const {
        if (v == source || v == sink) {
            return true;
        }
        ll inFlow = 0, outFlow = 0;
        for (std::size_t u = 0; u < n; u++) {
            for (const auto &e : adj[u]) {
                if (e.to == v && e.cap > 0) {
                    inFlow += e.flow;
                }
            }
        }
        for (const auto &e : adj[v]) {
            if (e.cap > 0) {
                outFlow += e.flow;
            }
        }
        return inFlow == outFlow;
    }

  public:
    Dinic(const std::size_t N) : n(N), adj(N), level(N), iter(N) {}

    void addEdge(const std::size_t u, const std::size_t v, const ll cap,
                 bool directed = true) {
        adj[u].emplace_back(v, cap, 0, adj[v].size());
        adj[v].emplace_back(u, directed ? 0 : cap, 0, adj[u].size() - 1);
    }

    ll maxFlow(const std::size_t source, const std::size_t sink) {
        if (source >= n || sink >= n) {
            throw std::out_of_range("Invalid source or sink");
        }
        if (source == sink) {
            return 0;
        }
        ll total = 0;
        while (bfs(source, sink)) {
            fill(iter.begin(), iter.end(), 0);
            ll pushed;
            while ((pushed = dfs(source, sink, INF)) > 0) {
                if (total > (INF - pushed)) {
                    throw std::runtime_error("Flow exceeds maximum");
                }
                total += pushed;
            }
        }
        return total;
    }

    std::vector<Flow> getFlows() const {
        std::vector<Flow> result;
        for (std::size_t u = 0; u < n; u++) {
            for (const auto &e : adj[u]) {
                if (e.cap > 0) {
                    result.emplace_back(u, e.to, e.cap, e.flow);
                }
            }
        }
        return result;
    }

    std::pair<std::vector<std::size_t>, std::vector<std::size_t>>
    getMinCut(const std::size_t source) const {
        const std::vector<uint8_t> reachable = getReachable(source);
        std::vector<std::size_t> sourceSide, sinkSide;
        for (std::size_t i = 0; i < n; i++) {
            if (reachable[i])
                sourceSide.emplace_back(i);
            else
                sinkSide.emplace_back(i);
        }
        return {sourceSide, sinkSide};
    }

    std::vector<std::pair<std::size_t, std::size_t>>
    getMinCutEdges(const std::size_t source) const {
        const std::vector<uint8_t> reachable = getReachable(source);
        std::vector<std::pair<std::size_t, std::size_t>> cutEdges;
        for (std::size_t u = 0; u < n; u++) {
            if (reachable[u]) {
                for (const auto &e : adj[u]) {
                    if (!reachable[e.to] && e.cap > 0) {
                        cutEdges.emplace_back(u, e.to);
                    }
                }
            }
        }
        return cutEdges;
    }

    ll getMinCutCapacity(const std::size_t source) const {
        const auto cutEdges = getMinCutEdges(source);
        ll total = 0;
        for (const auto &[u, v] : cutEdges) {
            for (const auto &e : adj[u]) {
                if (e.to == v && e.cap > 0) {
                    total += e.cap;
                    break;
                }
            }
        }
        return total;
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
