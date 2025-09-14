#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <queue>
#include <stdexcept>
#include <vector>

namespace algo::bfs {

using ll = long long;
using vll = std::vector<ll>;
using vvll = std::vector<vll>;
using vi = std::vector<int>;
using vui = std::vector<uint8_t>;
using vvi = std::vector<vi>;

template <typename T> constexpr size_t stcast(T x) {
    assert(x >= 0);
    return static_cast<size_t>(x);
}

// You can tweak this to increase or decrease the CPU load:
const ll M = 1'000'003; // Prime. Probably best if it is less than 1e9+7.

inline ll normalise(ll base, ll m) { return (base % m + m) % m; }

inline ll modPow(ll base, ll exp, ll m) {
    if (m <= 1) {
        throw std::invalid_argument("Modulus must be greater than 1");
    }
    base = normalise(base, m);
    ll result = 1;
    while (exp > 0) {
        if (exp & 1) { // If odd, multiply by the base
            result = (result * base) % m;
        }
        base = (base * base) % m; // Square the base and halve the exponent
        exp >>= 1;
    }
    return result;
}

inline ll modInv(ll base, ll m) {
    base = normalise(base, m);
    ll exp = m - 2; // b^-1 is modular congruent with b^m-2 mod m
    return modPow(base, exp, m);
}

inline ll stressCPU(ll x) {
    ll temp = (x % (M - 1)) + 1; // Ensure values are in the range [1...M-1]
    ll inverse = modInv(temp, M);
    return inverse % M;
}

/**
 * Generic BFS algorithm to calculate the shortest path between the start and all other
 * nodes.
 * Thread safe, so can be called concurrently to do a multi-source BFS.
 */
inline vll adjBFS(const vvi &adj, const size_t start) {
    const size_t n = adj.size();

    // Track distance and whether a node has been visited (to prevent infinite loops)
    vui visited(n, 0);
    vll transformation(n, 0); // Transformation orbit over the finite field Z/ZM

    std::queue<size_t> q;
    q.push(start);
    visited[start] = 1;        // Visit the start
    transformation[start] = 0; // Back to itself = 0

    while (!q.empty()) {
        const size_t next = q.front();
        q.pop();

        for (const int &neighbour : adj[next]) {
            const size_t idx = stcast(neighbour);
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

} // namespace algo::bfs
