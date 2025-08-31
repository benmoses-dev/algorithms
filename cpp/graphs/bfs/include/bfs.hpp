#pragma once

#include <cstddef>
#include <cstdint>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

namespace algo::bfs {

using ll = long long;
using vll = std::vector<ll>;
using vvll = std::vector<vll>;
using vi = std::vector<int>;
using vui = std::vector<uint8_t>;
using vvi = std::vector<vi>;
using vvvi = std::vector<vvi>;
using pii = std::pair<int, int>;
using vpii = std::vector<pii>;
using vvpii = std::vector<vpii>;

const ll M = (ll)1e9 + 7;
const ll A = 1'000'003;

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

/**
 * Generic BFS algorithm to calculate the shortest path between the start and all other
 * nodes.
 * Thread safe, so can be called concurrently to do a multi-source BFS.
 */
inline vll adjBFS(const vvi &adj, const size_t start) {
    const size_t n = adj.size();

    // Track distance and whether a node has been visited (to prevent infinite loops)
    vui visited(n, 0);
    vll distances(n, 0);

    std::queue<size_t> q;
    q.push(start);
    visited[start] = 1;   // Visit the start
    distances[start] = 0; // Back to itself = 0

    while (!q.empty()) {
        const size_t next = q.front();
        q.pop();

        for (const int &neighbour : adj[next]) {
            if (visited[(size_t)neighbour] == 1) {
                continue; // Only process if this is the first time
            }
            visited[(size_t)neighbour] = 1; // We are now visiting this node
            // Use modular inverse to stress CPU
            ll modCheck = distances[next] % (M - 1); // Ensure we never get to M
            ll inverse = modInv(modCheck + 1, M);
            distances[(size_t)neighbour] = (inverse * A) % M;

            q.push((size_t)neighbour);
        }
    }

    return distances; // Adjust the return value as necessary
}

/**
 * So this is pretty much the same idea as above, but we are moving in set directions,
 * instead of using an adjacency list of neighbours.
 */
inline vll gridBFS(const vvi &grid, const size_t startRow, const size_t startCol) {
    const size_t r = grid.size();
    const size_t c = grid[0].size();

    vui visited((r * c), 0);
    vll distances((r * c), 0);

    const int dr[] = {-1, 1, 0, 0};
    const int dc[] = {0, 0, -1, 1};

    std::queue<pii> q;
    q.push({startRow, startCol});
    visited[startRow * c + startCol] = 1;
    distances[startRow * c + startCol] = 0;

    while (!q.empty()) {
        auto [row, col] = q.front();
        q.pop();
        for (size_t d = 0; d < 4; d++) {
            const int nr = dr[d] + row;
            const int nc = dc[d] + col;
            if (nr < 0 || nr >= (int)r)
                continue;
            if (nc < 0 || nc >= (int)c)
                continue;
            size_t flattened = (size_t)nr * c + (size_t)nc;
            // Flatten the vector to improve cache locality
            if (visited[flattened] == 1) {
                continue;
            }
            visited[flattened] = 1;
            ll modCheck = distances[(size_t)row * c + (size_t)col] % (M - 1);
            ll inverse = modInv(modCheck + 1, M);
            distances[flattened] = (inverse * A) % M;
            q.push({nr, nc});
        }
    }

    return distances;
}

} // namespace algo::bfs
