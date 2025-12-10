#pragma once

#include <algorithm>
#include <climits>
#include <cstdint>
#include <queue>
#include <stdexcept>
#include <vector>

namespace algo::graph {

using u64 = std::uint64_t;
using i64 = std::int64_t;
using edge = std::pair<u64, u64>;

template <typename T> constexpr u64 to_uint(T x) {
    if (x < 0) {
        throw std::runtime_error("Cannot cast a negative to unsigned!");
    }
    return static_cast<u64>(x);
}

template <typename T> constexpr i64 to_int(T x) { return static_cast<i64>(x); }

inline std::vector<u64> dijkstra(std::vector<std::vector<edge>> &adjacencyList,
                                 std::vector<i64> &prev, u64 start) {
    std::size_t n = adjacencyList.size();
    std::vector<u64> distances(n, UINT64_MAX); // Non-negative weights

    // Queue of pair<weight, index> - sorted by smallest distance (min heap)
    std::priority_queue<edge, std::vector<edge>, std::greater<edge>> pq;
    pq.push({0, start});

    distances[start] = 0;
    prev[start] = -1;

    while (!pq.empty()) {
        auto [d, i] = pq.top();
        pq.pop();
        if (d > distances[i])
            continue; // Stale value on queue

        for (const auto &[index, weight] : adjacencyList[i]) {
            u64 currDist = distances[index];
            u64 newDist = d + weight;
            if (newDist < currDist) {
                /**
                 * Going to the neighbour through the current node is better
                 * than the current route to that node.
                 */
                distances[index] = newDist;
                prev[index] = to_int(i);
                pq.push({newDist, index});
            }
        }
    }

    return distances;
}

inline std::vector<u64> reconstructPath(u64 end, const std::vector<i64> &prev) {
    std::vector<u64> path;
    u64 curr = end;
    while (true) {
        path.push_back(curr);
        if (prev[curr] < 0)
            break;
        curr = to_uint(prev[curr]);
    }
    reverse(path.begin(), path.end());
    return path;
}

} // namespace algo::bfs
