#include <cstdint>
#include <numeric>
#include <stdexcept>
#include <vector>

namespace algo::numbers {

using ll = long long;
using u32 = uint32_t;
using i32 = int32_t;
using Matrix = std::vector<std::vector<ll>>;

/**
 * Helper function for normalising inputs mod m.
 * BE CAREFUL IF CALLING THIS ALL THE TIME!
 */
inline ll normalise(const ll x, const ll m) { return (x % m + m) % m; }

/**
 * Helper function to ensure the modulus is greater than 1.
 */
inline void checkMod(const ll m) {
    if (m <= 1) {
        throw std::invalid_argument("Modulus must be greater than 1");
    }
}

inline ll modMul(const ll a, const ll b, const ll m) {
    checkMod(m);
    return ((__int128)a * b) % m; // __int128 is not necessary if inputs <= int32
}

inline ll modAdd(const ll a, const ll b, const ll m) {
    checkMod(m);
    ll res = a + b;
    if (res >= m) {
        res -= m;
    }
    return res;
}

inline ll modSub(const ll a, const ll b, const ll m) {
    checkMod(m);
    ll res = a - b;
    if (res < 0) {
        res += m;
    }
    return res;
}

/**
 * EEA to find greatest common divisor and Bezout coefficients.
 * gcd(a, b) = gcd(b, a % b)
 */
inline ll extendedGCD(const ll a, const ll b, ll &x, ll &y) {
    if (b == 0) {
        // a * x + 0 * y = gcd(a, 0) = a
        x = 1;
        y = 0;
        return a;
    }
    ll x1, y1;
    const ll gcd = extendedGCD(b, a % b, x1, y1);
    /**
     * gcd(b, a % b) = (b * x) + ((a % b) * y) = gcd(a, b)
     * a % b = a - (a / b) * b
     * (b * x) + ((a - (a / b) * b) * y) = gcd(a, b)
     * (b * x) + (a * y) - ([a / b] * b * y)
     * (a * y) + b * (x) - b * ([a / b] * y)
     * a * y + b * (x - [a / b] * y)
     * Therefore:
     */
    x = y1;
    y = x1 - (a / b) * y1;
    return gcd;
}

/**
 * Use if m is not prime but is coprime with b.
 */
inline ll modInvEEA(const ll b, const ll m) {
    ll x, y; // Find x, y where (b * x) + (m * y) = gcd(b, m)
    extendedGCD(b, m, x, y);
    return normalise(x, m);
}

/**
 * Fast binary exponentiation in modular arithmetic.
 */
inline ll modPow(ll b, ll exp, const ll m) {
    checkMod(m);
    ll r = 1;
    while (exp > 0) {
        if (exp & 1) { // If odd, multiply by the base
            r = modMul(r, b, m);
        }
        b = modMul(b, b, m); // Square the base and halve the exponent
        exp >>= 1;
    }
    return r;
}

/**
 * Use if m is prime.
 */
inline ll modInvFermat(const ll b, const ll m) {
    ll exp = m - 2; // b^-1 is modular congruent with b^m-2 mod m
    return modPow(b, exp, m);
}

/**
 * This is a wrapper around Fermat's Little Theorem and the Extended Euclidean Algorithm.
 * Ensure that m is coprime with b.
 * If m is prime, it will use Fermat's, otherwise it will use EEA.
 */
inline ll modInv(const ll b, const ll m, const bool isPrime = false) {
    checkMod(m);
    if (std::gcd(b, m) != 1) {
        throw std::invalid_argument("modInv: b and m are not coprime!");
    }
    if (isPrime) {
        return modInvFermat(b, m);
    }
    return modInvEEA(b, m);
}

/**
 * Easy if m is prime, otherwise ensure that it is coprime with the denominator.
 */
inline ll modDivide(const ll numerator, const ll denominator, const ll m,
                    const bool isPrime = false) {
    const ll invB = modInv(denominator, m, isPrime);
    return modMul(numerator, invB, m);
}

/**
 * Handy struct to pre-calculate factorials and their inverses in modular arithmetic.
 * Also supports constant time nCr calculations.
 * n must be non-negative.
 */
struct ModFact {
    std::vector<ll> fact, invFact;
    ll mod;

    ModFact(const std::size_t n, const ll m, const bool prime = false)
        : fact(n + 1), invFact(n + 1), mod(m) {
        checkMod(m);
        fact[0] = 1;
        invFact[0] = 1;
        if (n == 0) {
            return;
        }
        for (std::size_t i = 1; i <= n; i++) {
            fact[i] = modMul(fact[i - 1], static_cast<ll>(i), mod);
        }
        invFact[n] = modInv(fact[n], mod, prime);
        for (std::size_t i = (n - 1); i > 0; i--) {
            invFact[i] = modMul(invFact[i + 1], static_cast<ll>(i + 1), mod);
        }
    }

    /**
     * Factorial nCr mod M.
     * Pre-computed for O(1) queries.
     * Do not use if N >= M, use Lucas theorem instead.
     */
    ll nCr(const ll n, const ll r) {
        if (n < 0) {
            throw std::invalid_argument("n must be non-negative");
        }
        if (r > n || r < 0) {
            return 0;
        }
        if (r == 0) {
            return 1;
        }
        if (n >= mod) {
            throw std::invalid_argument(
                "Modulus is less than N, please use Lucas theorem");
        }
        const ll temp = modMul(invFact[static_cast<std::size_t>(r)],
                               invFact[static_cast<std::size_t>(n - r)], mod);
        return modMul(fact[static_cast<std::size_t>(n)], temp, mod);
    }
};

/**
 * Pre-compute modular inverses up to n mod m.
 */
inline std::vector<ll> modInverseAll(const ll n, const ll m) {
    if (n < 0) {
        throw std::invalid_argument("n must be non-negative");
    }
    checkMod(m);
    const std::size_t nIdx = static_cast<std::size_t>(n);
    const std::size_t mIdx = static_cast<std::size_t>(m);
    std::vector<ll> inv(nIdx + 1, 1);
    for (std::size_t i = 2; i <= nIdx; i++) {
        const ll temp = modMul(static_cast<ll>(mIdx / i), inv[mIdx % i], m);
        inv[i] = modSub(m, temp, m);
    }
    return inv;
}

class NCRMod {
  private:
    ll m;

  public:
    NCRMod(const ll mod) : m(mod) { checkMod(m); }

    /**
     * Fast nCr in modular arithmetic
     * Uses Lucas theorem if n >= m
     */
    ll nCr(const ll n, const ll r) const {
        if (n < 0) {
            throw std::invalid_argument("n must be non-negative");
        }
        if (r > n || r < 0) {
            return 0;
        }
        if (r == 0) {
            return 1;
        }
        if (n >= m) {
            return nCrLucasIt(n, r);
        }
        return nCrMul(n, r);
    }

  private:
    /**
     * Multiplicative formula mod M.
     * O(r) complexity.
     * Do not use if N >= M, use Lucas theorem instead.
     */
    ll nCrMul(const ll n, ll r) const {
        if (r > (n - r)) { // Symmetry to increase performance
            r = n - r;
        }
        ll numerator = 1, denominator = 1;
        for (ll i = 1; i <= r; i++) {
            numerator = modMul(numerator, (n - i + 1), m);
            denominator = modMul(denominator, i, m);
        }
        return modDivide(numerator, denominator, m, true);
    }

    /**
     * Lucas theorem for arbitrary n and r mod prime.
     * Works even when N >= M.
     * O(logm(N) * m) complexity.
     */
    ll nCrLucas(const ll n, const ll r) const {
        if (r > n || r < 0) {
            return 0;
        }
        if (r == 0) {
            return 1;
        }
        const ll small = nCrMul(n % m, r % m);
        const ll lucas = nCrLucas(n / m, r / m);
        return modMul(small, lucas, m);
    }

    /**
     * Iterative version of Lucas theorem.
     */
    ll nCrLucasIt(ll n, ll r) const {
        ll res = 1;
        while (n > 0 || r > 0) {
            const ll ni = n % m;
            const ll ri = r % m;
            if (ri > ni) {
                return 0;
            }
            const ll small = nCrMul(ni, ri);
            res = modMul(res, small, m);
            n /= m;
            r /= m;
        }
        return res;
    }
};

inline Matrix matMul(const Matrix &A, const Matrix &B, const ll mod) {
    const std::size_t aH = A.size();
    const std::size_t aW = A[0].size();
    const std::size_t bH = B.size();
    const std::size_t bW = B[0].size();
    if (aW != bH) {
        throw std::runtime_error("Matrices are the incorrect size for multiplication");
    }
    Matrix C(aH, std::vector<ll>(bW, 0));
    for (std::size_t i = 0; i < aH; i++) {
        for (std::size_t j = 0; j < bW; j++) {
            for (std::size_t k = 0; k < aW; k++) {
                const ll mul = modMul(A[i][k], B[k][j], mod);
                C[i][j] = modAdd(C[i][j], mul, mod);
            }
        }
    }
    return C;
}

inline Matrix matPow(Matrix b, ll exp, const ll mod) {
    const std::size_t n = b.size();
    Matrix r(n, std::vector<ll>(n, 0));
    for (std::size_t i = 0; i < n; i++) {
        r[i][i] = 1;
    }
    while (exp > 0) {
        if (exp & 1) {
            r = matMul(r, b, mod);
        }
        b = matMul(b, b, mod);
        exp >>= 1;
    }
    return r;
}

/**
 * Calculate the value that results in the remainders a[i] when divided by m[i]
 * for all i from 1 to the length of a and m.
 * Ensure that all m are pairwise coprime, and ensure that a and m have the same length.
 * Ensure that the product of m is greater than twice the potential output value.
 */
inline std::pair<__int128, __int128> crt(const std::vector<ll> &a,
                                         const std::vector<u32> &m) {
    std::size_t n = m.size();
    if (n != a.size()) {
        throw std::invalid_argument("Number of remainders and moduli do not match");
    }

    // Multiply moduli
    __int128 M = 1;
    for (std::size_t i = 0; i < n; i++) {
        checkMod(m[i]);
        M *= (__int128)m[i];
    }

    std::vector<__int128> modFractions(n);
    std::vector<ll> invs(n);
    for (std::size_t i = 0; i < n; i++) {
        modFractions[i] = M / m[i];
        invs[i] = modInv(modFractions[i] % m[i], m[i]);
    }

    __int128 result = 0;
    for (std::size_t i = 0; i < n; i++) {
        const __int128 intermediate = (__int128)a[i] * modFractions[i] % M;
        result += (intermediate * invs[i]) % M;
    }
    result %= M;

    /**
     * Recover negative values
     * This will work as long as M is more than double the potential output
     */
    if (result > M / 2) {
        result -= M;
    }

    return {result, M};
}

/**
 * Use when moduli are not coprime (otherwise use the function above).
 */
inline std::pair<ll, ll> generalisedCRT(const std::vector<ll> &a,
                                        const std::vector<ll> &m) {
    ll x = a[0];
    ll lcm = m[0];
    for (std::size_t i = 1; i < a.size(); i++) {
        ll x1, y1;
        ll g = extendedGCD(lcm, m[i], x1, y1);
        if ((a[i] - x) % g != 0) {
            throw std::invalid_argument("Congruences are incompatible");
        }
        const ll mod = m[i] / g;
        const ll delta = ((a[i] - x) / g) % mod;
        ll temp = ((__int128)delta * x1) % mod;
        if (temp < 0)
            temp += mod;
        lcm = (lcm / g) * m[i];
        x += ((__int128)lcm * temp) % lcm;
    }
    return {x, lcm};
}

class NTT {
  private:
    /**
     * Mods for NTT and convolution with negative values.
     * Use 2 for smaller inputs, 3 for larger.
     * For smaller inputs, you may be able to avoid __int128 in the CRT.
     * Look to use Garner instead of CRT for performance.
     */
    const u32 MOD1 = 998244353;
    const u32 MOD2 = 1004535809;
    const u32 MOD3 = 985661441;
    const u32 ROOT = 3;
    const std::vector<u32> MODS = {MOD1, MOD2, MOD3};

  public:
    NTT() {}

    std::vector<ll> convolution(const std::vector<i32> &A,
                                const std::vector<i32> &B) const {
        const std::size_t N = A.size() + B.size() - 1;
        std::size_t p = 1; // Next power of two (could overflow if very large N)
        while (p < N) {
            p <<= 1;
        }

        /**
         * Carry out NTT using 2 or 3 moduli (depending on input/output size).
         */
        std::vector<ll> FA1(p, 0);
        std::vector<ll> FA2(p, 0);
        // std::vector<ll> FA3(p, 0);
        for (std::size_t i = 0; i < A.size(); i++) {
            FA1[i] = normalise(A[i], MOD1);
            FA2[i] = normalise(A[i], MOD2);
            // FA3[i] = normalise(A[i], MOD3);
        }
        std::vector<ll> FB1(p, 0);
        std::vector<ll> FB2(p, 0);
        // std::vector<ll> FB3(p, 0);
        for (std::size_t i = 0; i < B.size(); i++) {
            FB1[i] = normalise(B[i], MOD1);
            FB2[i] = normalise(B[i], MOD2);
            // FB3[i] = normalise(B[i], MOD3);
        }

        ntt(FA1, false, MOD1);
        ntt(FB1, false, MOD1);
        ntt(FA2, false, MOD2);
        ntt(FB2, false, MOD2);
        // ntt(FA3, false, MOD3);
        // ntt(FB3, false, MOD3);

        std::vector<ll> FC1(p, 0);
        std::vector<ll> FC2(p, 0);
        // std::vector<ll> FC3(p, 0);
        for (std::size_t i = 0; i < p; i++) {
            FC1[i] = modMul(FA1[i], FB1[i], MOD1);
            FC2[i] = modMul(FA2[i], FB2[i], MOD2);
            // FC3[i] = modMul(FA3[i], FB3[i], MOD3);
        }
        ntt(FC1, true, MOD1);
        ntt(FC2, true, MOD2);
        // ntt(FC3, true, MOD3);

        /**
         * If using CRT here -
         * Move the product, fractions, and inverses out of the CRT function.
         * Then just pass into the CRT function in the loop.
         */
        // -- Precompute (M, modFractions, invs) here. --
        // std::vector<__int128> FC(N, 0);
        // for (size_t i = 0; i < N; i++) {
        //     std::vector<ll> remainders = {FC1[i], FC2[i], FC3[i]};
        //     FC[i] = crt(remainders, MODS).first;
        // }

        /**
         * Garner instead of CRT for 2 moduli - slight performance improvement.
         */
        std::vector<ll> FC(N, 0);
        const ll mOneTwo = modInv(MOD1 % MOD2, MOD2);
        const __int128 M = (__int128)MOD1 * (__int128)MOD2;
        const __int128 half = M / 2;

        for (std::size_t i = 0; i < N; i++) {
            ll diff;
            if (FC2[i] >= FC1[i] % MOD2) {
                diff = FC2[i] - FC1[i] % MOD2;
            } else {
                diff = FC2[i] + MOD2 - FC1[i] % MOD2;
            }
            const ll t = ((__int128)diff * mOneTwo) % MOD2;
            __int128 combined = (__int128)FC1[i] + (__int128)MOD1 * (__int128)t;
            if (combined > half) {
                combined -= M;
            }
            FC[i] = (ll)combined;
        }

        return FC;
    }

  private:
    /**
     * NTT/FTT output in reverse bit order.
     * We need to reverse first to get correctly-ordered output.
     * This modifies the input vector in-place.
     */
    void bitReverse(std::vector<ll> &input) const {
        const std::size_t N = input.size();
        std::size_t r = 0;
        for (std::size_t i = 1; i < N; i++) {
            std::size_t bit = N >> 1; // start at the leftmost bit
            while (r & bit) { // carry by flipping 1 to 0 from left to right - 100 => 000
                r ^= bit;
                bit >>= 1;
            }
            r ^= bit;    // set the next left-most bit to 1 - 000 => 010
            if (i < r) { // only swap one way
                std::swap(input[i], input[r]);
            }
        }
    }

    void ntt(std::vector<ll> &input, const bool inverse, const u32 M) const {
        bitReverse(input);
        const std::size_t N = input.size();
        for (std::size_t length = 2; length <= N; length <<= 1) {
            ll wlen = modPow(ROOT, (M - 1) / length, M);
            if (inverse) {
                wlen = modInv(wlen, M);
            }
            for (std::size_t i = 0; i < N; i += length) {
                ll w = 1;
                for (std::size_t j = 0; j < length / 2; j++) {
                    const ll even = input[i + j];
                    const ll odd = modMul(input[i + j + length / 2], w, M);
                    input[i + j] = modAdd(even, odd, M);
                    input[i + j + length / 2] = modSub(even, odd, M);
                    w = modMul(w, wlen, M);
                }
            }
        }
        if (inverse) {
            const ll invN = modInv(static_cast<ll>(N), M, true);
            for (std::size_t i = 0; i < N; i++) {
                input[i] = modMul(input[i], invN, M);
            }
        }
    }
};

} // namespace algo::numbers
