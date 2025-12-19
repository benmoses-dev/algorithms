#pragma once

#include <algorithm>
#include <stdexcept>
#include <vector>

namespace algo::ds {

using ll = long long;

/**
 * 1D Fenwick Tree (Binary Indexed Tree)
 * Supports point updates and prefix prefixSum queries in O(log n) time.
 * Space: O(n), 0-based indexing.
 */
class FenwickTree {
  private:
    const int n;
    std::vector<ll> bit;

    static inline int lowbit(const int i) { return i & -i; }

    static int validateSize(const int size) {
        if (size < 0) {
            throw std::invalid_argument("Fenwick tree size must be non-negative");
        }
        return size;
    }

  public:
    explicit FenwickTree(const int size) : n(validateSize(size)), bit(size + 1, 0) {}

    explicit FenwickTree(const std::vector<ll> &data)
        : n(static_cast<int>(data.size())), bit(data.size() + 1, 0) {
        for (int i = 0; i < n; i++) {
            bit[i + 1] = data[i];
        }
        for (int i = 1; i <= n; i++) {
            const int j = i + lowbit(i);
            if (j <= n) {
                bit[j] += bit[i];
            }
        }
    }

    int size() const { return n; }

    void add(const int index, const ll delta) {
        if (index < 0 || index >= n) {
            throw std::out_of_range("Index out of bounds");
        }
        for (int i = index + 1; i <= n; i += lowbit(i)) {
            bit[i] += delta;
        }
    }

    void set(const int index, const ll value) { add(index, value - get(index)); }

    ll prefixSum(const int index) const {
        if (index < 0 || index >= n) {
            throw std::out_of_range("Index out of bounds");
        }
        ll total = 0;
        for (int i = index + 1; i > 0; i -= lowbit(i)) {
            total += bit[i];
        }
        return total;
    }

    ll rangeSum(const int l, const int r) const {
        if (l > r) {
            return 0;
        }
        if (r < 0 || r >= n) {
            throw std::out_of_range("Index out of bounds");
        }
        if (l == 0) {
            return prefixSum(r);
        }
        return prefixSum(r) - prefixSum(l - 1);
    }

    ll get(const int index) const { return rangeSum(index, index); }
};

/**
 * Node for storing both count and sum in compressed Fenwick tree
 */
struct FenwickNode {
    int count;
    ll sum;

    FenwickNode() : count(0), sum(0) {}
    FenwickNode(const int c, const ll s) : count(c), sum(s) {}
};

/**
 * Compressed Fenwick Tree
 *
 * Handles values up to 10^9 by using coordinate compression.
 * Maps arbitrary values to compressed ranks [0, k-1] where k is the number of
 * unique values. The tree stores both count and sum at each compressed position.
 *
 * Use case: Dynamic order statistics with range sums
 * - Find k-th smallest element
 * - Sum of smallest k elements
 * - Update values efficiently
 *
 * Time: O(log k) per operation where k = number of unique values
 * Space: O(k)
 */
class CompressedFenwickTree {
  private:
    int n;
    std::vector<FenwickNode> bit;
    std::vector<std::pair<ll, int>> sorted;
    std::vector<int> compressed;

    static inline int lowbit(const int i) { return i & -i; }

    void addInternal(const int rank, const ll value, const int delta) {
        for (int i = rank + 1; i <= n; i += lowbit(i)) {
            bit[i].count += delta;
            bit[i].sum += value * delta;
        }
    }

  public:
    /**
     * Construct from a vector of values (possibly with duplicates).
     */
    explicit CompressedFenwickTree(const std::vector<ll> &values)
        : n(values.size()), bit(n + 1), sorted(n), compressed(n) {
        // Create pairs of (value, original_index)
        for (int i = 0; i < n; i++) {
            sorted[i] = {values[i], i};
        }
        std::sort(sorted.begin(), sorted.end());
        // Map each original index to its compressed rank
        for (int rank = 0; rank < n; rank++) {
            compressed[sorted[rank].second] = rank;
        }
        // Initialise tree with all values
        for (int i = 0; i < n; i++) {
            const int rank = compressed[i];
            addInternal(rank, values[i], 1);
        }
    }

    int size() const { return n; }

    /**
     * Add delta instances of the value at origIdx.
     * origIdx refers to position in the original input array.
     */
    void add(const int origIdx, const int delta = 1) {
        if (origIdx < 0 || origIdx >= static_cast<int>(compressed.size())) {
            throw std::out_of_range("Index out of bounds");
        }
        const int rank = getRank(origIdx);
        const ll value = getOriginalValue(rank);
        addInternal(rank, value, delta);
    }

    /**
     * Remove delta instance of the value at origIdx.
     */
    void remove(const int origIdx, const int delta = 1) { add(origIdx, -delta); }

    /**
     * Get sum of the smallest k elements.
     * Walks the Fenwick tree structure to find k smallest elements in O(log n).
     *
     * Returns the sum of their ORIGINAL values (not compressed ranks).
     */
    ll sumK(const int k) const {
        if (k <= 0) {
            return 0;
        }
        int totalCount = 0;
        ll totalSum = 0;
        int pos = 0;
        for (int b = 19; b >= 0; --b) {
            const int nextPos = pos | (1 << b);
            if (nextPos <= n && totalCount + bit[nextPos].count <= k) {
                totalCount += bit[nextPos].count;
                totalSum += bit[nextPos].sum;
                pos = nextPos;
            }
        }
        return totalSum;
    }

    /**
     * Get sum from the smallest L to the smallest R.
     * Useful for getting e.g. the sum of the middle third of a range:
     * rangeSum(n / 3, 2 * n / 3)
     */
    ll rangeSum(const int L, const int R) const {
        if (L < 0) {
            throw std::out_of_range("Left out of bounds");
        }
        if (L <= 1) {
            return sumK(R);
        }
        return sumK(R) - sumK(L - 1);
    }

    ll totalSum() const {
        ll total = 0;
        int pos = 0;
        for (int b = 19; b >= 0; --b) {
            const int next = pos | (1 << b);
            if (next <= n) {
                total += bit[next].sum;
                pos = next;
            }
        }
        return total;
    }

    /**
     * Get total count of all elements in the tree.
     */
    int totalCount() const {
        int total = 0;
        int pos = 0;
        for (int b = 19; b >= 0; --b) {
            const int next = pos | (1 << b);
            if (next <= n) {
                total += bit[next].count;
                pos = next;
            }
        }
        return total;
    }

    /**
     * Get the original value at a compressed rank.
     */
    ll getOriginalValue(const int rank) const {
        if (rank < 0 || rank >= n) {
            throw std::out_of_range("Rank out of bounds");
        }
        return sorted[rank].first;
    }

    /**
     * Get compressed rank for an original index.
     */
    int getRank(const int origIdx) const {
        if (origIdx < 0 || origIdx >= static_cast<int>(compressed.size())) {
            throw std::out_of_range("Index out of bounds");
        }
        return compressed[origIdx];
    }
};

/**
 * 2D Fenwick Tree (Binary Indexed Tree)
 * Supports point updates and rectangle prefixSum queries in O(log n * log m) time.
 * Space: O(n * m), 0-based indexing.
 */
class FenwickTree2D {
  private:
    const int n;
    const int m;
    std::vector<std::vector<ll>> bit;

    static inline int lowbit(const int i) { return i & -i; }

    static int validateRows(const int rows, const int cols) {
        if (rows < 0 || cols < 0) {
            throw std::invalid_argument("Fenwick tree dimensions must be non-negative");
        }
        return rows;
    }

  public:
    explicit FenwickTree2D(const int rows, const int cols)
        : n(validateRows(rows, cols)), m(cols),
          bit(rows + 1, std::vector<ll>(cols + 1, 0)) {}

    explicit FenwickTree2D(const std::vector<std::vector<ll>> &data)
        : n(static_cast<int>(data.size())),
          m(data.empty() ? 0 : static_cast<int>(data[0].size())),
          bit(data.size() + 1, std::vector<ll>(m + 1, 0)) {
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                bit[i + 1][j + 1] = data[i][j];
            }
        }
        for (int i = 1; i <= n; ++i) {
            for (int j = 1; j <= m; ++j) {
                const int pi = i + lowbit(i);
                const int pj = j + lowbit(j);
                if (pi <= n) {
                    bit[pi][j] += bit[i][j];
                }
                if (pj <= m) {
                    bit[i][pj] += bit[i][j];
                }
                if (pi <= n && pj <= m) {
                    bit[pi][pj] -= bit[i][j];
                }
            }
        }
    }

    int rows() const { return n; }
    int cols() const { return m; }

    void add(const int x, const int y, const ll delta) {
        if (x < 0 || x >= n || y < 0 || y >= m) {
            throw std::out_of_range("Coordinates out of bounds");
        }
        for (int i = x + 1; i <= n; i += lowbit(i)) {
            for (int j = y + 1; j <= m; j += lowbit(j)) {
                bit[i][j] += delta;
            }
        }
    }

    void set(const int x, const int y, const ll value) { add(x, y, value - get(x, y)); }

    // Sum from [0,0] to [x,y]
    ll prefixSum(const int x, const int y) const {
        if (x < 0 || x >= n || y < 0 || y >= m) {
            throw std::out_of_range("Coordinates out of bounds");
        }
        ll total = 0;
        for (int i = x + 1; i > 0; i -= lowbit(i)) {
            for (int j = y + 1; j > 0; j -= lowbit(j)) {
                total += bit[i][j];
            }
        }
        return total;
    }

    // Sum of rectangle [x1,y1] to [x2,y2]
    ll rangeSum(const int x1, const int y1, const int x2, const int y2) const {
        if (x1 > x2 || y1 > y2) {
            return 0;
        }
        if (x2 < 0 || x2 >= n || y2 < 0 || y2 >= m) {
            throw std::out_of_range("Coordinates out of bounds");
        }
        const ll A = prefixSum(x2, y2);
        const ll B = (x1 > 0) ? prefixSum(x1 - 1, y2) : 0;
        const ll C = (y1 > 0) ? prefixSum(x2, y1 - 1) : 0;
        const ll D = (x1 > 0 && y1 > 0) ? prefixSum(x1 - 1, y1 - 1) : 0;
        return A - B - C + D;
    }

    ll get(const int x, const int y) const { return rangeSum(x, y, x, y); }
};

} // namespace algo::ds
