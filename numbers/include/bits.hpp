#pragma once

namespace algo::numbers {

using ui = unsigned int;

constexpr bool isPowerOfTwo(const ui x) { return x != 0 && (x & (x - 1u)) == 0; }

constexpr int countSetBits(ui x) {
    int count = 0;
    while (x) {
        x &= (x - 1u);
        count++;
    }
    return count;
}

constexpr ui lowestBit(const ui x) { return x & (-x); }

constexpr bool bitIsSet(const ui x, const ui k) { return (x & (1u << k)) != 0; }

constexpr ui clearLowestBit(const ui x) { return x & (x - 1u); }

constexpr ui flipBits(const ui x) { return ~x; }

constexpr ui toggleBit(const ui x, const ui k) { return x ^ (1u << k); }

/**
 * Ensure k is not larger than sizeof(unsigned int) * 8
 */
constexpr ui setBit(const ui x, const ui k) { return x | (1u << k); }

/**
 * Ensure k is not larger than sizeof(unsigned int) * 8
 */
constexpr ui clearBit(const ui x, const ui k) { return x & ~(1u << k); }

constexpr ui halveNTimes(const ui x, const ui n) { return x >> n; }

/**
 * Ensure n is not larger than sizeof(unsigned int) * 8
 */
constexpr ui doubleNTimes(const ui x, const ui n) { return x << n; }

} // namespace algo::numbers
