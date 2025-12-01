#include <algorithm>
#include <numeric>
#include <stdexcept>
#include <vector>

namespace algo::mst {

using ll = long long;

struct Edge {
    std::size_t u, v;
    ll w;
};

class DSU {
  private:
    std::vector<std::size_t> parents;
    std::vector<std::size_t> size;

  public:
    DSU(const std::size_t n) {
        parents.resize(n);
        iota(parents.begin(), parents.end(), 0);
        size.assign(n, 1);
    }
    std::size_t find(const std::size_t x) {
        if (parents[x] != x)
            parents[x] = find(parents[x]);
        return parents[x];
    }
    bool unite(const std::size_t a, const std::size_t b) {
        std::size_t rootA = find(a);
        std::size_t rootB = find(b);
        if (rootA == rootB)
            return false;
        if (size[rootA] < size[rootB])
            std::swap(rootA, rootB);
        parents[rootB] = rootA;
        size[rootA] += size[rootB];
        return true;
    }
};

inline std::pair<std::vector<Edge>, ll> mst(std::vector<Edge> edges,
                                            const std::size_t n) {
    if (edges.size() == 0) {
        if (n == 0 || n == 1) {
            return {{}, 0};
        }
        if (n > 1) {
            throw std::runtime_error("Graph is disconnected");
        }
    }
    DSU dsu(n);
    std::vector<Edge> mst;
    std::sort(edges.begin(), edges.end(),
              [](const Edge &a, const Edge &b) { return a.w < b.w; });
    ll weight = 0;
    for (const auto &edge : edges) {
        if (dsu.unite(edge.u, edge.v)) {
            mst.emplace_back(edge);
            weight += edge.w;
            if (mst.size() == (n - 1)) {
                break;
            }
        }
    }
    if (mst.size() != (n - 1)) {
        throw std::runtime_error("Graph is disconnected");
    }
    return {mst, weight};
}

} // namespace algo::mst
