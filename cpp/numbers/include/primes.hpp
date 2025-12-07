#pragma once

#include <cmath>
#include <cstdint>
#include <unordered_map>
#include <vector>

namespace algo::numbers {

using u8 = std::uint8_t;
using ll = long long;

inline bool isPrime(ll n) {
    if (n <= 1) {
        return false;
    }
    if (n <= 3) {
        return true;
    }
    if (n % 2 == 0 || n % 3 == 0) {
        return false;
    }
    for (ll i = 5; i <= n / i; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) {
            return false;
        }
    }
    return true;
}

inline std::vector<u8> sieve(const std::size_t N) {
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

inline std::pair<int, int> reduce(const int num, const std::vector<int> &primes,
                                  const std::vector<uint8_t> &sieve,
                                  std::unordered_map<int, std::pair<int, int>> &memo) {
    if (memo.count(num)) {
        return memo[num];
    }
    if (isPrime(num)) {
        memo[num] = {num, 1};
        return memo[num];
    }
    int sum = 0;
    int temp = num;
    std::size_t i = 0;
    do {
        const int prime = primes[i];
        while ((temp % prime) == 0) {
            sum += prime;
            temp /= prime;
        }
        i++;
    } while (temp > 1 && i < primes.size() && (primes[i] * primes[i] <= temp));
    if (temp > 1) {
        sum += temp;
    }
    std::pair<int, int> result = reduce(sum, primes, sieve, memo);
    memo[num] = {result.first, result.second + 1};
    return memo[num];
}

inline std::pair<int, int> getReduction(const int n) {
    const std::size_t squirt = static_cast<std::size_t>(sqrt(n)) + 1;
    const auto &s = sieve(squirt);
    std::vector<int> primes;
    for (std::size_t i = 1; i < squirt; i++) {
        if (s[i] == 1) {
            primes.emplace_back(i);
        }
    }
    std::unordered_map<int, std::pair<int, int>> memo;

    return reduce(n, primes, s, memo);
}

} // namespace algo::numbers
