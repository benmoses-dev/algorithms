#pragma once

#include "bfs.hpp"
#include <mutex>
#include <optional>
#include <thread>

namespace algo::bfs {

struct ThreadJoiner {
    std::vector<std::thread> &threads;
    ~ThreadJoiner() {
        for (auto &t : threads) {
            if (t.joinable()) {
                t.join();
            }
        }
    }
};

/**
 * Multi-threaded wrapper for BFS.
 * Each start point runs BFS independently in a separate thread.
 */
inline vvll multiBFS(const vvi &adj, const vi &starts,
                     std::optional<size_t> maxThreads = std::nullopt) {
    const size_t n = starts.size();
    vvll results(n);

    const size_t max_threads =
        maxThreads ? std::max(stcast(1), *maxThreads)
                   : std::max(stcast(1), stcast(std::thread::hardware_concurrency()));
    std::queue<size_t> q;
    for (size_t i = 0; i < n; i++) {
        q.push(i);
    }
    std::mutex qMut;
    std::vector<std::thread> threads;
    threads.reserve(max_threads);

    {
        ThreadJoiner joiner{threads};
        for (size_t i = 0; i < max_threads; i++) {
            threads.emplace_back([&adj, &results, &qMut, &q, &starts]() {
                while (true) {
                    size_t at;
                    {
                        std::lock_guard lock(qMut);
                        if (q.empty()) {
                            break;
                        }
                        at = q.front();
                        q.pop();
                    }
                    auto start = starts[at];
                    results[at] = adjBFS(adj, stcast(start));
                }
            });
        }
    }

    return results;
}

} // namespace algo::bfs
