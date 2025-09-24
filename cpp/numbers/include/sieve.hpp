#pragma once

#include <cstdint>
#include <vector>

namespace algo::numbers {

using u8 = std::uint8_t;
using u64 = std::uint64_t;

inline std::vector<u8> sieve(u64 N) {
    std::vector<u8> isPrime(N + 1, 1);
    isPrime[0] = isPrime[1] = 0;
    for (std::size_t i = 2; i <= (N / i); i++) {
        if (isPrime[i] == 0) {
            continue;
        }
        for (std::size_t j = (i * i); j <= N; j += i) {
            if (isPrime[j] == 1) {
                isPrime[j] = 0;
            }
        }
    }
    return isPrime;
}

} // namespace algo::numbers
