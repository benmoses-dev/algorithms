#pragma once

#include <algorithm>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <vector>

namespace algo::ds {

using ll = long long;

enum class Operation { SUM, MIN, MAX, GCD };

/**
 * Segment Tree with Lazy Propagation
 *
 * Supports:
 * - SUM: Range sum queries + range addition updates
 * - MIN: Range min queries + range set updates
 * - MAX: Range max queries + range set updates
 * - GCD: Range GCD queries + point updates only
 *
 * Time: O(log n) per query/update
 * Space: O(n)
 */
class SegmentTree {
  private:
    std::vector<ll> tree;
    std::vector<ll> lazy;
    std::vector<bool> lazySet; // Track if lazy is a "set" operation
    const int n;
    const Operation op;

    static int validateSize(const int size) {
        if (size <= 0) {
            throw std::invalid_argument("Segment tree size must be positive");
        }
        return size;
    }

    ll identity() const noexcept {
        switch (op) {
        case Operation::SUM:
            return 0;
        case Operation::MIN:
            return std::numeric_limits<ll>::max();
        case Operation::MAX:
            return std::numeric_limits<ll>::min();
        case Operation::GCD:
            return 0;
        }
        return 0;
    }

    ll merge(const ll a, const ll b) const noexcept {
        switch (op) {
        case Operation::SUM:
            return a + b;
        case Operation::MIN:
            return std::min(a, b);
        case Operation::MAX:
            return std::max(a, b);
        case Operation::GCD:
            return std::gcd(a, b);
        }
        return 0;
    }

    void applyLazy(const int node, const int start, const int end, const ll value) {
        if (op == Operation::SUM) {
            tree[node] += value * (end - start + 1);
        } else {
            // MIN/MAX/GCD: set operation
            tree[node] = value;
        }
    }

    void propagate(const int node, const int start, const int end) {
        if (lazy[node] == 0 && !lazySet[node]) {
            return;
        }
        applyLazy(node, start, end, lazy[node]);
        if (start != end) {
            if (op == Operation::SUM) {
                lazy[2 * node] += lazy[node];
                lazy[2 * node + 1] += lazy[node];
            } else {
                lazy[2 * node] = lazy[node];
                lazy[2 * node + 1] = lazy[node];
                lazySet[2 * node] = true;
                lazySet[2 * node + 1] = true;
            }
        }
        lazy[node] = 0;
        lazySet[node] = false;
    }

    void build(const std::vector<ll> &arr, const int node, const int start,
               const int end) {
        if (start == end) {
            tree[node] = arr[start];
        } else {
            const int mid = (start + end) / 2;
            build(arr, 2 * node, start, mid);
            build(arr, 2 * node + 1, mid + 1, end);
            tree[node] = merge(tree[2 * node], tree[2 * node + 1]);
        }
    }

    void updateRange(const int node, const int start, const int end, const int L,
                     const int R, const ll val) {
        propagate(node, start, end);
        if (R < start || end < L) {
            return;
        }
        if (L <= start && end <= R) {
            lazy[node] = val;
            if (op != Operation::SUM) {
                lazySet[node] = true;
            }
            propagate(node, start, end);
            return;
        }
        const int mid = (start + end) / 2;
        updateRange(2 * node, start, mid, L, R, val);
        updateRange(2 * node + 1, mid + 1, end, L, R, val);
        tree[node] = merge(tree[2 * node], tree[2 * node + 1]);
    }

    void updatePoint(const int node, const int start, const int end, const int idx,
                     const ll val) {
        if (start == end) {
            tree[node] = val;
            return;
        }
        const int mid = (start + end) / 2;
        if (idx <= mid) {
            updatePoint(2 * node, start, mid, idx, val);
        } else {
            updatePoint(2 * node + 1, mid + 1, end, idx, val);
        }
        tree[node] = merge(tree[2 * node], tree[2 * node + 1]);
    }

    ll queryRange(const int node, const int start, const int end, const int L,
                  const int R) {
        if (R < start || end < L) {
            return identity();
        }
        propagate(node, start, end);
        if (L <= start && end <= R) {
            return tree[node];
        }
        const int mid = (start + end) / 2;
        const ll leftQuery = queryRange(2 * node, start, mid, L, R);
        const ll rightQuery = queryRange(2 * node + 1, mid + 1, end, L, R);
        return merge(leftQuery, rightQuery);
    }

  public:
    explicit SegmentTree(const std::vector<ll> &arr,
                         const Operation oper = Operation::SUM)
        : n(validateSize(static_cast<int>(arr.size()))), op(oper) {
        tree.assign(4 * n, identity());
        lazy.assign(4 * n, 0);
        lazySet.assign(4 * n, false);
        if (n > 0) {
            build(arr, 1, 0, n - 1);
        }
    }

    int size() const { return n; }

    Operation operation() const { return op; }

    void update(const int L, const int R, const ll val) {
        if (L < 0 || R >= n || L > R) {
            throw std::out_of_range("Update range out of bounds");
        }
        if (op == Operation::GCD) {
            throw std::logic_error("GCD segment tree only supports point updates");
        }
        updateRange(1, 0, n - 1, L, R, val);
    }

    void updatePoint(const int idx, const ll val) {
        if (idx < 0 || idx >= n) {
            throw std::out_of_range("Index out of bounds");
        }
        updatePoint(1, 0, n - 1, idx, val);
    }

    ll query(const int L, const int R) {
        if (L < 0 || R >= n || L > R) {
            throw std::out_of_range("Query range out of bounds");
        }
        return queryRange(1, 0, n - 1, L, R);
    }

    ll queryPoint(const int idx) { return query(idx, idx); }
};

} // namespace algo::ds
