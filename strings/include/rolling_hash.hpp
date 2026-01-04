#pragma once

#include <string>
#include <unordered_set>
#include <vector>

namespace algo::strings {

using ll = long long;

/**
 * Polynomial Rolling Hash for fast substring comparison.
 *
 * Allows O(1) comparison of any two substrings after O(n) preprocessing.
 *
 * Space Complexity: O(n)
 */
class StringHash {
  private:
    static constexpr ll BASE = 31;
    static constexpr ll MOD1 = 1e9 + 7;
    static constexpr ll MOD2 = 1e9 + 9;

    std::string text;
    std::size_t n;
    std::vector<ll> hash1, hash2;
    std::vector<ll> power1, power2;

  public:
    explicit StringHash(const std::string &in)
        : text(in), n(text.length()), hash1(n + 1), hash2(n + 1), power1(n + 1),
          power2(n + 1) {
        power1[0] = power2[0] = 1;
        for (std::size_t i = 1; i <= n; i++) {
            power1[i] = (power1[i - 1] * BASE) % MOD1;
            power2[i] = (power2[i - 1] * BASE) % MOD2;
        }
        for (std::size_t i = 0; i < n; i++) {
            hash1[i + 1] = (hash1[i] * BASE + text[i]) % MOD1;
            hash2[i + 1] = (hash2[i] * BASE + text[i]) % MOD2;
        }
    }

    /**
     * Get hashes of substring text[left..right] (inclusive).
     *
     * @param left Starting index (inclusive)
     * @param right Ending index (inclusive)
     * @return Hash value of substring
     *
     * Time: O(1)
     */
    std::pair<ll, ll> getHash(const std::size_t left, const std::size_t right) const {
        if (left > right || right >= n) {
            return {0, 0};
        }
        ll h1 = (hash1[right + 1] - hash1[left] * power1[right - left + 1]) % MOD1;
        ll h2 = (hash2[right + 1] - hash2[left] * power2[right - left + 1]) % MOD2;
        if (h1 < 0) {
            h1 += MOD1;
        }
        if (h2 < 0) {
            h2 += MOD2;
        }
        return {h1, h2};
    }

    /**
     * Compare substrings with double hashing (extremely low collision probability).
     *
     * @param left1 Start of first substring
     * @param right1 End of first substring
     * @param left2 Start of second substring
     * @param right2 End of second substring
     * @return true if substrings are equal (with high probability)
     *
     * Time: O(1)
     */
    bool compare(const std::size_t left1, const std::size_t right1,
                 const std::size_t left2, const std::size_t right2) const {
        return getHash(left1, right1) == getHash(left2, right2);
    }

    /**
     * Check if string is a palindrome.
     *
     * @return true if entire string is a palindrome
     *
     * Time: O(n) for building reverse hash, O(1) for query
     */
    bool isPalindrome() const {
        const std::string reversed(text.rbegin(), text.rend());
        const StringHash revHash(reversed);
        return getHash(0, n - 1) == revHash.getHash(0, n - 1);
    }

    /**
     * Get the original text.
     */
    const std::string &getText() const { return text; }
};

struct pairHash {
    std::size_t operator()(const std::pair<ll, ll> &p) const {
        return std::hash<ll>()(p.first) ^ (std::hash<ll>()(p.second) << 1);
    }
};

inline std::size_t check(const std::vector<std::string> &strings, const int len,
                         const std::vector<StringHash> &hashes) {
    std::unordered_set<std::pair<ll, ll>, pairHash> subs;
    subs.reserve(strings[0].size());
    std::size_t i = 0;
    while (i + len <= strings[0].size()) {
        subs.insert(hashes[0].getHash(i, i + len - 1));
        i++;
    }
    for (std::size_t k = 1; k < strings.size(); k++) {
        std::unordered_set<std::pair<ll, ll>, pairHash> at;
        at.reserve(strings[k].size());
        std::size_t j = 0;
        while (j + len <= strings[k].size()) {
            const auto &h = hashes[k].getHash(j, j + len - 1);
            if (subs.count(h)) {
                at.insert(h);
            }
            j++;
        }
        if (at.empty()) {
            return 0;
        }
        subs = std::move(at);
    }
    return subs.size();
}

inline std::size_t LCS(const std::vector<std::string> &strings) {
    const std::size_t N = strings.size();
    std::vector<StringHash> hashes;
    hashes.reserve(N);
    std::size_t minS = 100;
    for (const auto &str : strings) {
        minS = std::min(str.size(), minS);
        hashes.emplace_back(str);
    }
    int low = 1;
    int high = static_cast<int>(minS);
    size_t ans = 0;
    while (low <= high) {
        const int len = (low + high) / 2;
        const auto result = check(strings, len, hashes);
        if (result > 0) {
            ans = len;
            low = len + 1;
        } else {
            high = len - 1;
        }
    }
    return ans;
}

} // namespace algo::strings
