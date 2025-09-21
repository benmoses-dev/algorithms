#pragma once

#include <cassert>
#include <cstdint>
#include <queue>
#include <stdexcept>
#include <vector>

namespace algo::bfs {

using ll = long long;
using ul = unsigned long;

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
 * Graph-transformation BFS algorithm to calculate the transformation orbit over the
 * finite field Z/ZM. Thread safe, so can be called concurrently to do a multi-source BFS.
 */
inline std::vector<ll> adjBFS(const std::vector<std::vector<ul>> &adj, const ul start) {
    const ul n = adj.size();

    // Track distance and whether a node has been visited (to prevent infinite loops)
    std::vector<uint8_t> visited(n, 0);
    std::vector<ll> transformation(n, 0);

    std::queue<ul> q;
    q.push(start);
    visited[start] = 1;        // Visit the start
    transformation[start] = 0; // Back to itself = 0

    while (!q.empty()) {
        const ul next = q.front();
        q.pop();

        for (const ul &neighbour : adj[next]) {
            const ul idx = neighbour;
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
