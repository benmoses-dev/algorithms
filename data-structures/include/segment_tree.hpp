#pragma once

#include <algorithm>
#include <cstddef>
#include <numeric>
#include <vector>

using ll = long long;

namespace algo::ds {

enum class Operation { SUM, MIN, MAX, GCD };

class SegmentTree {
  private:
    std::vector<ll> tree, lazy;
    std::size_t n;
    Operation op;

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

    void propagate(const std::size_t node, const std::size_t start,
                   const std::size_t end) {
        if (lazy[node] != 0) {
            if (op == Operation::SUM) {
                tree[node] += (end - start + 1) * lazy[node];
            } else {
                tree[node] += lazy[node];
            }
            if (start != end) {
                lazy[2 * node] += lazy[node];
                lazy[2 * node + 1] += lazy[node];
            }
            lazy[node] = 0;
        }
    }

    void build(const std::vector<ll> &arr, const std::size_t node,
               const std::size_t start, const std::size_t end) {
        if (start == end) {
            tree[node] = arr[start];
        } else {
            const std::size_t mid = (start + end) / 2;
            build(arr, 2 * node, start, mid);
            build(arr, 2 * node + 1, mid + 1, end);
            tree[node] = merge(tree[2 * node], tree[2 * node + 1]);
        }
    }

    void update(const std::size_t node, const std::size_t start, const std::size_t end,
                const std::size_t L, const std::size_t R, const ll val) {
        propagate(node, start, end);
        if (R < start || end < L) {
            return;
        }
        if (L <= start && end <= R) {
            lazy[node] += val;
            propagate(node, start, end);
            return;
        }
        const std::size_t mid = (start + end) / 2;
        update(2 * node, start, mid, L, R, val);
        update(2 * node + 1, mid + 1, end, L, R, val);
        tree[node] = merge(tree[2 * node], tree[2 * node + 1]);
    }

    ll query(const std::size_t node, const std::size_t start, const std::size_t end,
             const std::size_t L, const std::size_t R) {
        propagate(node, start, end);
        if (R < start || end < L) {
            return 0;
        }
        if (L <= start && end <= R) {
            return tree[node];
        }
        const std::size_t mid = (start + end) / 2;
        const ll p1 = query(2 * node, start, mid, L, R);
        const ll p2 = query(2 * node + 1, mid + 1, end, L, R);
        return merge(p1, p2);
    }

  public:
    SegmentTree(const std::vector<ll> &arr, const Operation oper = Operation::SUM)
        : n(arr.size()), op(oper) {
        tree.assign(4 * n, 0);
        lazy.assign(4 * n, 0);
        build(arr, 1, 0, n - 1);
    }

    void update(const std::size_t L, const std::size_t R, const ll val) {
        update(1, 0, n - 1, L, R, val);
    }

    ll query(const std::size_t L, const std::size_t R) {
        return query(1, 0, n - 1, L, R);
    }
};

} // namespace algo::ds
