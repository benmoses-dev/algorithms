#pragma once

#include <algorithm>
#include <climits>
#include <cstdint>
#include <queue>
#include <stdexcept>
#include <vector>

namespace algo::bfs {

using edge = std::pair<std::uint64_t, std::uint64_t>;

template <typename T> constexpr std::uint64_t to_uint(T x) {
    if (x < 0) {
        throw std::runtime_error("Cannot cast a negative to unsigned!");
    }
    return static_cast<std::uint64_t>(x);
}

template <typename T> constexpr std::int64_t to_int(T x) {
    return static_cast<std::int64_t>(x);
}

inline std::vector<std::uint64_t> dijkstra(std::vector<std::vector<edge>> &adjacencyList,
                                           std::vector<std::int64_t> &prev,
                                           std::uint64_t start) {
    std::size_t n = adjacencyList.size();
    std::vector<std::uint64_t> distances(n, UINT64_MAX); // Non-negative weights

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
            std::uint64_t currDist = distances[index];
            std::uint64_t newDist = d + weight;
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

inline std::vector<std::uint64_t> reconstructPath(std::uint64_t end,
                                                  const std::vector<std::int64_t> &prev) {
    std::vector<std::uint64_t> path;
    std::uint64_t curr = end;
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
