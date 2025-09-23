#pragma once

#include <cstdint>
#include <vector>

namespace algo::numbers {

using ll = long long;

inline std::vector<uint8_t> sieve(ll N) {
    std::vector<uint8_t> isPrime(N + 1, 1);
    isPrime[0] = isPrime[1] = 0;
    for (ll i = 2; i <= (N / i); i++) {
        if (isPrime[i] == 0) {
            continue;
        }
        for (ll j = (i * i); j <= N; j += i) {
            if (isPrime[j] == 1) {
                isPrime[j] = 0;
            }
        }
    }
    return isPrime;
}

} // namespace algo::numbers
