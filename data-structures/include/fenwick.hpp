#pragma once

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
