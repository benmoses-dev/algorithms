#include "threads.hpp"
#include <iostream>

int main() {
    using namespace algo::bfs;

    vvi adj = {
        {1, 2},    // Node 0 connects to 1, 2
        {0, 3},    // Node 1 connects to 0, 3
        {0, 3},    // Node 2 connects to 0, 3
        {1, 2, 4}, // Node 3 connects to 1, 2, 4
        {3}        // Node 4 connects to 3
    };
    vi starts = {0, 4};

    std::cout << "multiBFS results:" << std::endl;
    vvi bfsResults = multiBFS(adj, starts);
    for (size_t i = 0; i < bfsResults.size(); ++i) {
        std::cout << "From start " << starts[i] << ": ";
        for (int d : bfsResults[i])
            std::cout << d << " ";
        std::cout << std::endl;
    }

    vvi grid = {
        {0, 0, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 0},
        {0, 1, 0, 0}
    };
    vpii gridStarts = {{0, 0}, {2, 2}};

    std::cout << std::endl << "multiGridBFS results:" << std::endl;
    vvvi gridResults = multiGridBFS(grid, gridStarts);
    for (size_t i = 0; i < gridResults.size(); ++i) {
        std::cout << "From start (" << gridStarts[i].first << ", " << gridStarts[i].second
                  << "):" << std::endl;
        for (auto &row : gridResults[i]) {
            for (int d : row)
                std::cout << d << "\t";
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    return 0;
}
