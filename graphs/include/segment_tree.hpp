#pragma once

#include <vector>

namespace algo::segment_tree {

class SegmentTree {
    std::vector<std::size_t> tree;
    std::size_t n;

    void build(const std::vector<std::size_t> &arr, const std::size_t node,
               const std::size_t start, std::size_t end) {
        if (start == end) {
            tree[node] = arr[start];
        } else {
            const std::size_t mid = (start + end) / 2;
            build(arr, 2 * node, start, mid);
            build(arr, 2 * node + 1, mid + 1, end);
            tree[node] = tree[2 * node] + tree[2 * node + 1]; // Combine
        }
    }

    void update(const std::size_t node, const std::size_t start, const std::size_t end,
                const std::size_t idx, const std::size_t val) {
        if (start == end) {
            tree[node] = val;
        } else {
            const std::size_t mid = (start + end) / 2;
            if (idx <= mid) {
                update(2 * node, start, mid, idx, val);
            } else {
                update(2 * node + 1, mid + 1, end, idx, val);
            }
            tree[node] = tree[2 * node] + tree[2 * node + 1];
        }
    }

    std::size_t query(const std::size_t node, const std::size_t start,
                      const std::size_t end, const std::size_t L, const std::size_t R) {
        if (R < start || end < L) {
            return 0;
        }
        if (L <= start && end <= R) {
            return tree[node];
        }
        const std::size_t mid = (start + end) / 2;
        const std::size_t p1 = query(2 * node, start, mid, L, R);
        const std::size_t p2 = query(2 * node + 1, mid + 1, end, L, R);
        return p1 + p2;
    }

  public:
    SegmentTree(std::vector<std::size_t> &arr) {
        n = arr.size();
        tree.assign(4 * n, 0);
        build(arr, 1, 0, n - 1);
    }

    void update(const std::size_t idx, const std::size_t val) {
        update(1, 0, n - 1, idx, val);
    }

    std::size_t query(const std::size_t L, const std::size_t R) {
        return query(1, 0, n - 1, L, R);
    }
};

} // namespace algo::segment_tree
