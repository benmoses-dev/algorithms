#pragma once

#include <numeric>
#include <stack>
#include <vector>

namespace algo::graph {

using ll = long long;

class DSU {
  private:
    std::vector<std::size_t> parents;
    std::vector<std::size_t> size;
    std::vector<int> maxVal;
    std::vector<ll> sums;
    std::size_t numComponents;

  public:
    DSU(const std::vector<int> &values) {
        const std::size_t n = values.size();
        parents.resize(n);
        iota(parents.begin(), parents.end(), 0);
        size.assign(n, 1);
        maxVal.resize(n);
        sums.resize(n);
        for (std::size_t i = 0; i < n; i++) {
            maxVal[i] = values[i];
            sums[i] = values[i];
        }
        numComponents = n;
    }

    std::size_t find(const std::size_t x) {
        if (parents[x] != x)
            parents[x] = find(parents[x]);
        return parents[x];
    }

    void unite(const std::size_t a, const std::size_t b) {
        std::size_t rootA = find(a);
        std::size_t rootB = find(b);
        if (rootA == rootB)
            return;
        if (size[rootA] < size[rootB]) {
            std::swap(rootA, rootB);
        }
        parents[rootB] = rootA;
        size[rootA] += size[rootB];
        sums[rootA] += sums[rootB];
        maxVal[rootA] = std::max(maxVal[rootA], maxVal[rootB]);
        maxVal[rootB] = 0;
        numComponents--;
    }

    std::size_t getSize(const std::size_t i) { return size[find(i)]; }

    int getMaxVal(const std::size_t i) { return maxVal[find(i)]; }

    ll getSum(const std::size_t i) { return sums[find(i)]; }

    std::size_t getComponents() const { return numComponents; }
};

class DSURollback {
  private:
    std::vector<std::size_t> parent, rank;
    std::stack<std::pair<std::size_t *, std::size_t>> history; // (pointer, old value)

    void save(std::size_t *ptr) { history.push({ptr, *ptr}); }

  public:
    DSURollback(const std::size_t n) : parent(n), rank(n, 0) {
        iota(parent.begin(), parent.end(), 0);
    }

    std::size_t find(const std::size_t x) const {
        // No path compression (but can be const)
        return parent[x] == x ? x : find(parent[x]);
    }

    bool unite(std::size_t x, std::size_t y) {
        x = find(x);
        y = find(y);
        if (x == y) {
            return false;
        }
        if (rank[x] < rank[y]) {
            std::swap(x, y);
        }
        save(&parent[y]);
        parent[y] = x;
        if (rank[x] == rank[y]) {
            save(&rank[x]);
            rank[x]++;
        }
        return true;
    }

    void rollback(const std::size_t checkpointSize) {
        // Undo operations until history.size() == checkpointSize
        while (history.size() > checkpointSize) {
            auto [ptr, oldVal] = history.top();
            history.pop();
            *ptr = oldVal;
        }
    }

    std::size_t checkpoint() const { return history.size(); }
};

} // namespace algo::graph
