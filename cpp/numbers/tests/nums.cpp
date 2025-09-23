#include "pascal.hpp"
#include "sieve.hpp"
#include <cassert>
#include <chrono>
#include <iostream>
#include <numeric>
#include <vector>

using namespace algo::numbers;

int main() {
    {
        // Test 1: Small sieve correctness
        auto primes = sieve(20);
        std::vector<size_t> expected = {2, 3, 5, 7, 11, 13, 17, 19};
        for (size_t p : expected) {
            assert(primes[p] == 1);
        }
        std::cout << "Sieve test (N=20) passed\n";
    }

    // Test 2: Large sieve performance
    {
        ll N = 100'000'000;
        auto start = std::chrono::high_resolution_clock::now();
        auto primes = sieve(N);
        auto end = std::chrono::high_resolution_clock::now();

        int primeCount = std::accumulate(primes.begin(), primes.end(), 0);
        std::cout << "Sieve up to " << N << " found " << primeCount << " primes\n";
        std::cout
            << "Execution time: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
            << " ms\n";
    }

    // Test 3: Pascal small correctness
    {
        auto pt = pascalsTriangle(5, 5);
        // Check a few known values
        assert(pt[5][0] == 1);
        assert(pt[5][5] == 1);
        assert(pt[5][2] == 10);
        assert(pt[4][2] == 6);
        std::cout << "Pascal small test passed\n";
    }

    // Test 4: Pascal mid-sized values
    {
        int N = 60, R = 30;
        auto start = std::chrono::high_resolution_clock::now();
        auto pt = pascalsTriangle(N, R);
        auto end = std::chrono::high_resolution_clock::now();

        std::cout << "Pascal computed up to n=" << N << ", r=" << R << "\n";
        std::cout << "C(60,30) = " << pt[60][30] << "\n";
        std::cout
            << "Execution time: "
            << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
            << " µs\n";
    }

    std::cout << "All tests completed.\n";
    return 0;
}
