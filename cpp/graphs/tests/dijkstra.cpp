#include "dijkstra.hpp"
#include <cassert>
#include <climits>
#include <iostream>
#include <vector>

using namespace algo::bfs;

int main() {
    /**
     * Graph (directed, with a cycle):
     *
     *   0 --2--> 1
     *   0 --500--> 2
     *   0 --1000--> 6
     *   1 --50--> 2
     *   1 --200--> 3
     *   2 --100--> 3
     *   2 --600--> 4
     *   3 --1--> 4
     *   3 --300--> 5
     *   4 --2--> 5
     *   4 --10--> 2   <-- cycle edge
     *   5 --1000--> 6
     *
     * Node 7 is disconnected.
     *
     * Shortest paths from 0:
     *   dist[0] = 0
     *   dist[1] = 2                   (0 -> 1)
     *   dist[2] = 52                  (0 -> 1 -> 2)
     *   dist[3] = 152                 (0 -> 1 -> 2 -> 3)
     *   dist[4] = 153                 (0 -> 1 -> 2 -> 3 -> 4)
     *   dist[5] = 155                 (0 -> 1 -> 2 -> 3 -> 4 -> 5)
     *   dist[6] = 1000                (0 -> 6)
     *   dist[7] = UINT_MAX            (disconnected)
     */

    std::vector<std::vector<edge>> graph(8);
    graph[0].push_back({1, 2});
    graph[0].push_back({2, 500});
    graph[0].push_back({6, 1000});
    graph[1].push_back({2, 50});
    graph[1].push_back({3, 200});
    graph[2].push_back({3, 100});
    graph[2].push_back({4, 600});
    graph[3].push_back({4, 1});
    graph[3].push_back({5, 300});
    graph[4].push_back({5, 2});
    graph[4].push_back({2, 10});
    graph[5].push_back({6, 1000});
    // node 7: disconnected

    std::vector<int> prev(8, -1);
    auto dist = dijkstra(graph, prev, 0);

    // Distances
    assert(dist[0] == 0);
    assert(dist[1] == 2);
    assert(dist[2] == 52);
    assert(dist[3] == 152);
    assert(dist[4] == 153);
    assert(dist[5] == 155);
    assert(dist[6] == 1000);
    assert(dist[7] == UINT_MAX);

    // Predecessors
    assert(prev[0] == -1); // start
    assert(prev[1] == 0);  // 0 -> 1
    assert(prev[2] == 1);  // 0 -> 1 -> 2
    assert(prev[3] == 2);  // 0 -> 1 -> 2 -> 3
    assert(prev[4] == 3);  // 0 -> 1 -> 2 -> 3 -> 4
    assert(prev[5] == 4);  // 0 -> 1 -> 2 -> 3 -> 4 -> 5
    assert(prev[6] == 0);  // 0 -> 6
    assert(prev[7] == -1); // disconnected

    // Path reconstruction assertions
    auto pathTo5 = reconstructPath(5, prev);
    std::vector<ul> expected5 = {0, 1, 2, 3, 4, 5};
    assert(pathTo5 == expected5);

    auto pathTo6 = reconstructPath(6, prev);
    std::vector<ul> expected6 = {0, 6};
    assert(pathTo6 == expected6);

    auto pathTo2 = reconstructPath(2, prev);
    std::vector<ul> expected2 = {0, 1, 2};
    assert(pathTo2 == expected2);

    auto pathTo7 = reconstructPath(7, prev);
    std::vector<ul> expected7 = {7}; // Unreachable, only itself
    assert(pathTo7 == expected7);

    std::cout << "All tests passed!\n";
    return 0;
}
