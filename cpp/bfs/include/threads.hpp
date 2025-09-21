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

template <typename T> constexpr ul stcast(T x) {
    assert(x >= 0);
    return static_cast<ul>(x);
}

/**
 * Multi-threaded wrapper for BFS.
 * Each start point runs BFS independently in a separate thread.
 */
inline std::vector<std::vector<ll>>
multiBFS(const std::vector<std::vector<ul>> &adj, const std::vector<ul> &starts,
         std::optional<ul> maxThreads = std::nullopt) {
    const ul n = starts.size();
    std::vector<std::vector<ll>> results(n);

    const ul max_threads =
        maxThreads ? std::max(stcast(1), *maxThreads)
                   : std::max(stcast(1), stcast(std::thread::hardware_concurrency()));
    std::queue<ul> q;
    for (ul i = 0; i < n; i++) {
        q.push(i);
    }
    std::mutex qMut;
    std::vector<std::thread> threads;
    threads.reserve(max_threads);

    {
        ThreadJoiner joiner{threads};
        for (ul i = 0; i < max_threads; i++) {
            threads.emplace_back([&adj, &results, &qMut, &q, &starts]() {
                while (true) {
                    ul at;
                    {
                        std::lock_guard lock(qMut);
                        if (q.empty()) {
                            break;
                        }
                        at = q.front();
                        q.pop();
                    }
                    auto start = starts[at];
                    results[at] = adjBFS(adj, start);
                }
            });
        }
    }

    return results;
}

} // namespace algo::bfs
