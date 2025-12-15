#pragma once

#include <algorithm>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <vector>

namespace algo::ds {

using ll = long long;

/**
 * Segment Tree for Range Sum Queries
 *
 * Supports:
 * - Range addition updates: a[i] += v
 * - Range sum queries
 *
 * Time: O(log n) per operation
 * Space: O(n)
 */
class SegmentTreeSum {
  private:
    std::vector<ll> tree;
    std::vector<ll> lazy;
    ll n;

    static ll validateSize(const ll size) {
        if (size <= 0) {
            throw std::invalid_argument("Segment tree size must be positive");
        }
        return size;
    }

    void build(const std::vector<ll> &arr, const ll node, const ll start, const ll end) {
        if (start == end) {
            tree[node] = arr[start];
        } else {
            const ll mid = (start + end) / 2;
            build(arr, 2 * node, start, mid);
            build(arr, 2 * node + 1, mid + 1, end);
            tree[node] = tree[2 * node] + tree[2 * node + 1];
        }
    }

    void applyLazy(const ll node, const ll start, const ll end, const ll value) {
        tree[node] += value * (end - start + 1);
        lazy[node] += value;
    }

    void propagate(const ll node, const ll start, const ll end) {
        if (lazy[node] == 0 || start == end) {
            return;
        }
        const ll mid = (start + end) / 2;
        applyLazy(2 * node, start, mid, lazy[node]);
        applyLazy(2 * node + 1, mid + 1, end, lazy[node]);
        lazy[node] = 0;
    }

    void updateRange(const ll node, const ll start, const ll end, const ll L, const ll R,
                     const ll val) {
        if (R < start || end < L) {
            return;
        }
        if (L <= start && end <= R) {
            applyLazy(node, start, end, val);
            return;
        }
        propagate(node, start, end);
        const ll mid = (start + end) / 2;
        updateRange(2 * node, start, mid, L, R, val);
        updateRange(2 * node + 1, mid + 1, end, L, R, val);
        tree[node] = tree[2 * node] + tree[2 * node + 1];
    }

    ll queryRange(const ll node, const ll start, const ll end, const ll L, const ll R) {
        if (R < start || end < L) {
            return 0;
        }
        if (L <= start && end <= R) {
            return tree[node];
        }
        propagate(node, start, end);
        const ll mid = (start + end) / 2;
        return queryRange(2 * node, start, mid, L, R) +
               queryRange(2 * node + 1, mid + 1, end, L, R);
    }

  public:
    explicit SegmentTreeSum(const std::vector<ll> &arr)
        : n(validateSize(static_cast<ll>(arr.size()))) {
        tree.assign(4 * n, 0);
        lazy.assign(4 * n, 0);
        build(arr, 1, 0, n - 1);
    }

    void update(const ll L, const ll R, const ll val) {
        if (L < 0 || R >= n || L > R) {
            throw std::out_of_range("Update range out of bounds");
        }
        updateRange(1, 0, n - 1, L, R, val);
    }

    ll query(const ll L, const ll R) {
        if (L < 0 || R >= n || L > R) {
            throw std::out_of_range("Query range out of bounds");
        }
        return queryRange(1, 0, n - 1, L, R);
    }
};

/**
 * Segment Tree for Min / Max / GCD Queries
 *
 * Supports:
 * - Point updates: a[i] = v
 * - Range queries:
 *     - minimum
 *     - maximum
 *     - gcd
 *
 * No lazy propagation.
 *
 * Time: O(log n) per operation
 * Space: O(n)
 */
class SegmentTreeStats {
  private:
    struct Node {
        ll minVal;
        ll maxVal;
        ll gcdVal;
    };

    std::vector<Node> tree;
    const ll n;

    static ll validateSize(const ll size) {
        if (size <= 0) {
            throw std::invalid_argument("Segment tree size must be positive");
        }
        return size;
    }

    static Node identity() noexcept {
        return {std::numeric_limits<ll>::max(), std::numeric_limits<ll>::min(), 0};
    }

    static Node merge(const Node &a, const Node &b) noexcept {
        return {std::min(a.minVal, b.minVal), std::max(a.maxVal, b.maxVal),
                std::gcd(a.gcdVal, b.gcdVal)};
    }

    void build(const std::vector<ll> &arr, const ll node, const ll start, const ll end) {
        if (start == end) {
            tree[node] = {arr[start], arr[start], arr[start]};
        } else {
            const ll mid = (start + end) / 2;
            build(arr, 2 * node, start, mid);
            build(arr, 2 * node + 1, mid + 1, end);
            tree[node] = merge(tree[2 * node], tree[2 * node + 1]);
        }
    }

    void updatePoint(const ll node, const ll start, const ll end, const ll idx,
                     const ll val) {
        if (start == end) {
            tree[node] = {val, val, val};
            return;
        }
        const ll mid = (start + end) / 2;
        if (idx <= mid) {
            updatePoint(2 * node, start, mid, idx, val);
        } else {
            updatePoint(2 * node + 1, mid + 1, end, idx, val);
        }
        tree[node] = merge(tree[2 * node], tree[2 * node + 1]);
    }

    Node queryRange(const ll node, const ll start, const ll end, const ll L, const ll R) {
        if (R < start || end < L) {
            return identity();
        }
        if (L <= start && end <= R) {
            return tree[node];
        }
        const ll mid = (start + end) / 2;
        return merge(queryRange(2 * node, start, mid, L, R),
                     queryRange(2 * node + 1, mid + 1, end, L, R));
    }

  public:
    explicit SegmentTreeStats(const std::vector<ll> &arr)
        : n(validateSize(static_cast<ll>(arr.size()))) {
        tree.assign(4 * n, identity());
        build(arr, 1, 0, n - 1);
    }

    void updatePoint(const ll idx, const ll val) {
        if (idx < 0 || idx >= n) {
            throw std::out_of_range("Index out of bounds");
        }
        updatePoint(1, 0, n - 1, idx, val);
    }

    ll queryMin(const ll L, const ll R) {
        if (L < 0 || R >= n || L > R) {
            throw std::out_of_range("Query range out of bounds");
        }
        return queryRange(1, 0, n - 1, L, R).minVal;
    }

    ll queryMax(const ll L, const ll R) {
        if (L < 0 || R >= n || L > R) {
            throw std::out_of_range("Query range out of bounds");
        }
        return queryRange(1, 0, n - 1, L, R).maxVal;
    }

    ll queryGCD(const ll L, const ll R) {
        if (L < 0 || R >= n || L > R) {
            throw std::out_of_range("Query range out of bounds");
        }
        return queryRange(1, 0, n - 1, L, R).gcdVal;
    }
};

} // namespace algo::ds
