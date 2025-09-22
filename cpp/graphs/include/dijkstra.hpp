#include <climits>
#include <queue>
#include <vector>

namespace algo::bfs {

using ul = unsigned long;
using edge = std::pair<ul, ul>;

inline std::vector<ul> dijkstra(std::vector<std::vector<edge>> &adjacencyList,
                                std::vector<int> &prev, ul start) {
    ul n = adjacencyList.size();
    std::vector<ul> distances(n, UINT_MAX); // Non-negative weights

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
            ul currDist = distances[index];
            ul newDist = d + weight;
            if (newDist < currDist) {
                /**
                 * Going to the neighbour through the current node is better
                 * than the current route to that node.
                 */
                distances[index] = newDist;
                prev[index] = static_cast<int>(i);
                pq.push({newDist, index});
            }
        }
    }

    return distances;
}

} // namespace algo::bfs
