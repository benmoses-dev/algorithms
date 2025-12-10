#pragma once
#include <cassert>
#include <cstddef>
#include <vector>

using ll = long long;

namespace algo::ds {

/**
 * 1D Fenwick Tree (Binary Indexed Tree)
 * Supports point updates and prefix sum queries in O(log n) time.
 * Space: O(n), 0-based indexing.
 */
class FenwickTree {
  private:
    const std::size_t n;
    std::vector<ll> bit; // 1-indexed internally

    static inline int lowbit(const int i) { return i & -i; }

  public:
    explicit FenwickTree(const std::size_t size) : n(size), bit(size + 1, 0) {}

    explicit FenwickTree(const std::vector<ll> &data)
        : n(data.size()), bit(data.size() + 1, 0) {
        for (std::size_t i = 0; i < n; i++) {
            bit[i + 1] = data[i];
        }
        for (int i = 1; i <= static_cast<int>(n); i++) {
            const int j = i + lowbit(i);
            if (j <= static_cast<int>(n)) {
                bit[j] += bit[i];
            }
        }
    }

    std::size_t size() const { return n; }

    void add(const std::size_t index, const ll delta) {
        assert(index < n && "Index out of bounds");
        for (int i = static_cast<int>(index) + 1; i <= static_cast<int>(n);
             i += lowbit(i)) {
            bit[i] += delta;
        }
    }

    void set(const std::size_t index, const ll value) { add(index, value - get(index)); }

    ll sum(const std::size_t index) const {
        assert(index < n && "Index out of bounds");
        ll total = 0;
        for (int i = static_cast<int>(index) + 1; i > 0; i -= lowbit(i)) {
            total += bit[i];
        }
        return total;
    }

    ll rangeSum(const std::size_t l, const std::size_t r) const {
        if (l > r) {
            return 0;
        }
        assert(r < n && "Index out of bounds");
        if (l == 0) {
            return sum(r);
        }
        return sum(r) - sum(l - 1);
    }

    ll get(const std::size_t index) const { return rangeSum(index, index); }
};

/**
 * 2D Fenwick Tree (Binary Indexed Tree)
 * Supports point updates and rectangle sum queries in O(log n * log m) time.
 * Space: O(n * m), 0-based indexing.
 */
class FenwickTree2D {
  private:
    const std::size_t n;
    const std::size_t m;
    std::vector<std::vector<ll>> bit; // 1-indexed internally

    static inline int lowbit(const int i) { return i & -i; }

  public:
    explicit FenwickTree2D(const std::size_t rows, const std::size_t cols)
        : n(rows), m(cols), bit(rows + 1, std::vector<ll>(cols + 1, 0)) {}

    explicit FenwickTree2D(const std::vector<std::vector<ll>> &data)
        : n(data.size()), m(data.empty() ? 0 : data[0].size()),
          bit(data.size() + 1, std::vector<ll>(m + 1, 0)) {
        for (std::size_t i = 0; i < n; ++i) {
            for (std::size_t j = 0; j < m; ++j) {
                bit[i + 1][j + 1] = data[i][j];
            }
        }
        for (int i = 1; i <= static_cast<int>(n); ++i) {
            for (int j = 1; j <= static_cast<int>(m); ++j) {
                const int pi = i + lowbit(i);
                const int pj = j + lowbit(j);
                if (pi <= static_cast<int>(n)) {
                    bit[pi][j] += bit[i][j];
                }
                if (pj <= static_cast<int>(m)) {
                    bit[i][pj] += bit[i][j];
                }
                if (pi <= static_cast<int>(n) && pj <= static_cast<int>(m)) {
                    bit[pi][pj] -= bit[i][j]; // Avoid double-counting
                }
            }
        }
    }

    std::size_t rows() const { return n; }
    std::size_t cols() const { return m; }

    void add(const std::size_t x, const std::size_t y, const ll delta) {
        assert(x < n && y < m && "Coordinates out of bounds");
        for (int i = static_cast<int>(x) + 1; i <= static_cast<int>(n); i += lowbit(i)) {
            for (int j = static_cast<int>(y) + 1; j <= static_cast<int>(m);
                 j += lowbit(j)) {
                bit[i][j] += delta;
            }
        }
    }

    void set(const std::size_t x, const std::size_t y, const ll value) {
        add(x, y, value - get(x, y));
    }

    // Sum from [0,0] to [x,y]
    ll sum(const std::size_t x, const std::size_t y) const {
        assert(x < n && y < m && "Coordinates out of bounds");
        ll total = 0;
        for (int i = static_cast<int>(x) + 1; i > 0; i -= lowbit(i)) {
            for (int j = static_cast<int>(y) + 1; j > 0; j -= lowbit(j)) {
                total += bit[i][j];
            }
        }
        return total;
    }

    // Sum of rectangle [x1,y1] to [x2,y2]
    ll rangeSum(const std::size_t x1, const std::size_t y1, const std::size_t x2,
                const std::size_t y2) const {
        if (x1 > x2 || y1 > y2) {
            return 0;
        }
        assert(x2 < n && y2 < m && "Coordinates out of bounds");
        const ll A = sum(x2, y2);
        const ll B = (x1 > 0) ? sum(x1 - 1, y2) : 0;
        const ll C = (y1 > 0) ? sum(x2, y1 - 1) : 0;
        const ll D = (x1 > 0 && y1 > 0) ? sum(x1 - 1, y1 - 1) : 0;
        return A - B - C + D;
    }

    ll get(const std::size_t x, const std::size_t y) const {
        return rangeSum(x, y, x, y);
    }
};

} // namespace algo::ds
