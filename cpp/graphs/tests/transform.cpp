#include "transform.hpp"
#include <iostream>

using namespace algo::bfs;

int main() {
    const std::vector<std::vector<ul>> adj = {
        {1, 2},   // Node 0 (start)
        {0, 3},   // Node 1
        {0, 4},   // Node 2
        {1, 5},   // Node 3
        {2, 6},   // Node 4
        {3, 7},   // Node 5
        {4, 8},   // Node 6
        {5, 9},   // Node 7
        {6, 10},  // Node 8
        {7, 11},  // Node 9
        {8, 12},  // Node 10
        {9, 13},  // Node 11
        {10, 14}, // Node 12
        {11, 15}, // Node 13
        {12, 16}, // Node 14
        {13, 17}, // Node 15
        {14, 18}, // Node 16
        {15, 19}, // Node 17
        {16, 20}, // Node 18
        {17, 21}, // Node 19
        {18, 22}, // Node 20
        {19, 23}, // Node 21
        {20, 24}, // Node 22
        {21, 25}, // Node 23
        {22, 26}, // Node 24
        {23, 27}, // Node 25
        {24, 28}, // Node 26
        {25, 29}, // Node 27
        {26, 30}, // Node 28
        {27, 31}, // Node 29
        {28, 32}, // Node 30
        {29, 33}, // Node 31
        {30, 34}, // Node 32
        {31, 35}, // Node 33
        {32},     // Node 34
        {33},     // Node 35
    };

    // Multiple spines (starts) at nodes 0, 4, 8, 12, 16
    const std::vector<ul> starts = {0, 4, 8, 12, 16};

    std::cout << "multiBFS results:" << std::endl;
    const std::vector<std::vector<ll>> bfsResults = multiBFS(adj, starts);
    for (ul i = 0; i < bfsResults.size(); ++i) {
        std::cout << "From start " << starts[i] << ": ";
        for (ll d : bfsResults[i])
            std::cout << d << " ";
        std::cout << "\n";
    }

    return 0;
}
