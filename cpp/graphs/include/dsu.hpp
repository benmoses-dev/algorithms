#include <numeric>
#include <vector>

namespace algo::dsu {

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

} // namespace algo::dsu
