#include "transform.hpp"
#include <chrono>
#include <iostream>

using namespace algo::bfs;

int main() {
    std::optional<u64> maxThreads;
    std::string line;

    std::cout << "Enter max threads (or leave blank to use max hardware concurrency): ";
    std::getline(std::cin, line);

    if (!line.empty()) {
        try {
            u64 val = std::stoul(line);
            maxThreads = val;
        } catch (...) {
            std::cerr << "Invalid number!" << std::endl;
        }
    }

    if (!maxThreads) {
        std::cout << "Using hardware concurrency to determine thread count" << std::endl;
        maxThreads = std::thread::hardware_concurrency();
    }

    std::cout << "Using max threads = " << *maxThreads << std::endl;
    std::cout << "Starting now..." << std::endl;

    {
        // Adjacency list stress test
        const u64 n = 1'000'000; // 1 million nodes
        std::vector<std::vector<u64>> adj(n);

        // Sparse chain + some extra edges to make it interesting
        for (u64 i = 0; i < n - 1; i++) {
            adj[i].push_back(i + 1);
            adj[i + 1].push_back(i);
            if (i % 100 == 0 && i + 100 < n) {
                adj[i].push_back(i + 100);
                adj[i + 100].push_back(i);
            }
        }

        // 1,000 start points spread across the graph
        u64 starts = 1'000;
        std::vector<u64> adjStarts(starts, 0);
        for (u64 i = 0; i < starts; i++) {
            adjStarts[i] = i * starts;
        }

        auto t1 = std::chrono::high_resolution_clock::now();
        std::vector<std::vector<i64>> adjResults = multiBFS(adj, adjStarts, maxThreads);
        auto t2 = std::chrono::high_resolution_clock::now();

        std::cout << "multiBFS (" << adjStarts.size() << " starts, " << n
                  << " nodes) took " << std::chrono::duration<double>(t2 - t1).count()
                  << " seconds" << std::endl;
    }

    return 0;
}
