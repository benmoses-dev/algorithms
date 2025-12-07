#include "primes.hpp"
#include <chrono>
#include <gtest/gtest.h>
#include <numeric>

using namespace algo::numbers;

class PrimesTest : public ::testing::Test {
  protected:
    void SetUp() override {}

    void TearDown() override {}
};

TEST_F(PrimesTest, SieveSmallN) {
    const auto primes = sieve(20);
    std::vector<std::size_t> expected = {2, 3, 5, 7, 11, 13, 17, 19};

    for (const auto p : expected) {
        EXPECT_EQ(primes[p], 1) << "Expected " << p << " to be prime";
    }
}

TEST_F(PrimesTest, SieveLargeN) {
    const std::size_t N = 100'000'000;

    const auto start = std::chrono::high_resolution_clock::now();
    const auto primes = sieve(N);
    const auto end = std::chrono::high_resolution_clock::now();

    const auto primeCount = std::accumulate(primes.begin(), primes.end(), 0);
    const auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "Sieve up to " << N << " found " << primeCount << " primes\n";
    std::cout << "Execution time: " << duration << " ms\n";

    // Verify the count is reasonable (known value for primes up to 100M is 5,761,455)
    EXPECT_GT(primeCount, 0) << "Should find at least some primes";
    EXPECT_EQ(primeCount, 5761455) << "Expected known prime count for N=100,000,000";
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
