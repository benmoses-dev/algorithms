#pragma once

#include <cassert>
#include <cstdint>
#include <mutex>
#include <optional>
#include <queue>
#include <stdexcept>
#include <thread>
#include <vector>

namespace algo::bfs {

using u64 = std::uint64_t;
using i64 = std::int64_t;

template <typename T> constexpr u64 to_uint(T x) {
    if (x < 0) {
        throw std::runtime_error("Cannot cast a negative to unsigned!");
    }
    return static_cast<u64>(x);
}

// You can tweak this to increase or decrease the CPU load:
const i64 M = 1'000'003; // Prime. Probably best if it is less than 1e9+7.

inline i64 normalise(i64 base, i64 m) { return (base % m + m) % m; }

inline i64 modPow(i64 base, i64 exp, i64 m) {
    if (m <= 1) {
        throw std::invalid_argument("Modulus must be greater than 1");
    }
    base = normalise(base, m);
    i64 result = 1;
    while (exp > 0) {
        if (exp & 1) { // If odd, multiply by the base
            result = (result * base) % m;
        }
        base = (base * base) % m; // Square the base and halve the exponent
        exp >>= 1;
    }
    return result;
}

inline i64 modInv(i64 base, i64 m) {
    base = normalise(base, m);
    i64 exp = m - 2; // b^-1 is modular congruent with b^m-2 mod m
    return modPow(base, exp, m);
}

inline i64 stressCPU(i64 x) {
    i64 temp = (x % (M - 1)) + 1; // Ensure values are in the range [1...M-1]
    i64 inverse = modInv(temp, M);
    return inverse % M;
}

/**
 * Graph-transformation BFS algorithm to calculate the transformation orbit over the
 * finite field Z/ZM. Thread safe, so can be called concurrently to do a multi-source BFS.
 */
inline std::vector<i64> adjBFS(const std::vector<std::vector<u64>> &adj,
                               const u64 start) {
    const std::size_t n = adj.size();

    // Track distance and whether a node has been visited (to prevent infinite loops)
    std::vector<uint8_t> visited(n, 0);
    std::vector<i64> transformation(n, 0);

    std::queue<u64> q;
    q.push(start);
    visited[start] = 1;        // Visit the start
    transformation[start] = 0; // Back to itself = 0

    while (!q.empty()) {
        const u64 next = q.front();
        q.pop();

        for (const u64 &neighbour : adj[next]) {
            const u64 idx = neighbour;
            if (visited[idx] == 1) {
                continue; // Only process if this is the first time
            }
            visited[idx] = 1; // We are now visiting this node
            transformation[idx] = stressCPU(transformation[next]);
            q.push(idx);
        }
    }

    return transformation;
}

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
inline std::vector<std::vector<i64>>
multiBFS(const std::vector<std::vector<u64>> &adj, const std::vector<u64> &starts,
         std::optional<u64> maxThreads = std::nullopt) {
    const std::size_t n = starts.size();
    std::vector<std::vector<i64>> results(n);

    const u64 max_threads =
        maxThreads ? std::max(to_uint(1), *maxThreads)
                   : std::max(to_uint(1), to_uint(std::thread::hardware_concurrency()));
    std::queue<std::size_t> q;
    for (std::size_t i = 0; i < n; i++) {
        q.push(i);
    }
    std::mutex qMut;
    std::vector<std::thread> threads;
    threads.reserve(max_threads);

    {
        ThreadJoiner joiner{threads};
        for (std::size_t i = 0; i < max_threads; i++) {
            threads.emplace_back([&adj, &results, &qMut, &q, &starts]() {
                while (true) {
                    u64 at;
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
