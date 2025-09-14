#include "threads.hpp"
#include <iostream>

int main() {
    using namespace algo::bfs;

    const vvi adj = {
        {1, 2},    // Node 0 connects to 1, 2
        {0, 3},    // Node 1 connects to 0, 3
        {0, 3},    // Node 2 connects to 0, 3
        {1, 2, 4}, // Node 3 connects to 1, 2, 4
        {3}        // Node 4 connects to 3
    };
    const vi starts = {0, 4};

    std::cout << "multiBFS results:" << std::endl;
    const vvll bfsResults = multiBFS(adj, starts);
    for (size_t i = 0; i < bfsResults.size(); ++i) {
        std::cout << "From start " << starts[i] << ": ";
        for (ll d : bfsResults[i])
            std::cout << d << " ";
        std::cout << "\n";
    }

    return 0;
}
