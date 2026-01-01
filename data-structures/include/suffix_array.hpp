#pragma once

#include "sparse_table.hpp"
#include <algorithm>
#include <climits>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace algo::ds {

using ll = long long;

struct SubString {
    std::size_t length;
    std::size_t startPos;
};

/**
 * For a string S of length n, the suffix array SA[i] contains the starting
 * position of the i-th smallest suffix when all suffixes are sorted lexicographically.
 *
 * Applications:
 * - Pattern matching in O(m log n) - usually not the best choice
 * - Finding longest repeated substring
 * - Counting distinct substrings
 * - Longest common substring of multiple strings
 * - Burrows-Wheeler transform
 */
class SuffixArray {
  private:
    std::string text;
    std::size_t n;
    std::vector<std::size_t> sa;
    std::vector<std::size_t> lcp;
    std::vector<std::size_t> rank;

    /**
     * Build suffix array using the doubling algorithm (Manber-Myers).
     *
     * Time: O(n log^2 n)
     */
    void buildSA() {
        std::vector<std::size_t> temp(n);
        for (std::size_t i = 0; i < n; i++) {
            sa[i] = i;
            rank[i] = static_cast<unsigned char>(text[i]);
        }
        for (std::size_t k = 1; k < n; k <<= 1) {
            const auto cmp = [&](std::size_t i, std::size_t j) {
                if (rank[i] != rank[j]) {
                    return rank[i] < rank[j];
                }
                i += k; // Add cyclic shift (% n) to both of these for BWT
                j += k;
                const std::size_t ri = (i < n) ? rank[i] : 0;
                const std::size_t rj = (j < n) ? rank[j] : 0;
                return ri < rj;
            };
            std::sort(sa.begin(), sa.end(), cmp);
            temp[sa[0]] = 0;
            for (std::size_t i = 1; i < n; i++) {
                const bool equal = !cmp(sa[i - 1], sa[i]);
                temp[sa[i]] = temp[sa[i - 1]] + (equal ? 0 : 1);
            }
            rank = temp;
            if (rank[sa[n - 1]] == n - 1) {
                break;
            }
        }
    }

    /**
     * Build LCP array using Kasai's algorithm
     *
     * LCP[i] = length of longest common prefix between suffixes SA[i] and SA[i-1]
     * LCP[0] is undefined (or 0)
     *
     * Time: O(n)
     */
    void buildLCP() {
        lcp[0] = 0;
        std::size_t len = 0;
        for (std::size_t suff = 0; suff < n; suff++) {
            const std::size_t at = rank[suff];
            if (at <= 0) {
                continue;
            }
            const std::size_t prev = at - 1;
            const std::size_t prevSuff = sa[prev];
            std::size_t r = suff + len;
            std::size_t l = prevSuff + len;
            while (r < n && l < n && text[r] == text[l]) {
                len++;
                r = suff + len;
                l = prevSuff + len;
            }
            lcp[at] = len;
            if (len > 0) {
                len--;
            }
        }
    }

  public:
    explicit SuffixArray(const std::string &in)
        : text(in), n(text.size()), sa(n), lcp(n), rank(n) {
        if (n == 0) {
            return;
        }
        buildSA();
        buildLCP();
    }

    const std::vector<std::size_t> &getSA() const { return sa; }

    const std::vector<std::size_t> &getLCP() const { return lcp; }

    const std::vector<std::size_t> &getRank() const { return rank; }

    const std::string &getText() const { return text; }

    std::string getSuffix(const std::size_t i) const {
        if (i >= n) {
            return "";
        }
        return text.substr(sa[i]);
    }

    /**
     * Count distinct substrings in the string.
     *
     * Time: O(n)
     */
    std::size_t distinct() const {
        const std::size_t total = n * (n + 1) / 2;
        std::size_t duplicates = 0;
        for (const std::size_t lcpVal : lcp) {
            duplicates += lcpVal;
        }
        return total - duplicates;
    }

    /**
     * Find the longest repeated substring.
     *
     * This is simply the maximum value in the LCP array.
     * The substring starts at SA[i] where LCP[i] is maximum.
     *
     * Time: O(n)
     *
     * @return SubString with (length, starting_position)
     */
    SubString LRS() const {
        if (n == 0) {
            return {0, 0};
        }
        std::size_t maxLen = 0;
        std::size_t pos = 0;
        for (std::size_t i = 0; i < n; i++) {
            if (lcp[i] > maxLen) {
                maxLen = lcp[i];
                pos = sa[i];
            }
        }
        return {maxLen, pos};
    }

    /**
     * Pattern matching using binary search on suffix array.
     * Finds all occurrences of pattern in text.
     *
     * Time: O(m log n + occ) where m = pattern length, occ = number of occurrences
     *
     * Note: In almost all cases, either KMP/Z (for single patterns) or Aho-Corasick (for
     * multiple patterns) will be much faster.
     *
     * @return Vector of starting positions where pattern occurs
     */
    std::vector<std::size_t> search(const std::string &pattern) const {
        std::vector<std::size_t> result;
        const std::size_t m = pattern.length();
        if (m == 0 || m > n) {
            return result;
        }
        const auto lower =
            std::lower_bound(sa.begin(), sa.end(), pattern,
                             [&](const std::size_t idx, const std::string &p) {
                                 return text.compare(idx, m, p) < 0;
                             });
        if (lower == sa.end()) {
            return result;
        }
        const auto upper =
            std::upper_bound(sa.begin(), sa.end(), pattern,
                             [&](const std::string &p, const std::size_t idx) {
                                 return text.compare(idx, m, p) > 0;
                             });
        for (auto it = lower; it != upper; it++) {
            result.emplace_back(*it);
        }
        std::sort(result.begin(), result.end());
        return result;
    }

    /**
     * Count occurrences of pattern (without returning positions).
     * Faster than search() when you only need the count.
     *
     * Time: O(m log n)
     */
    std::size_t count(const std::string &pattern) const {
        const std::size_t m = pattern.length();
        if (m == 0 || m > n) {
            return 0;
        }
        const auto lower =
            std::lower_bound(sa.begin(), sa.end(), pattern,
                             [&](const std::size_t idx, const std::string &p) {
                                 return text.compare(idx, m, p) < 0;
                             });
        if (lower == sa.end()) {
            return 0;
        }
        const auto upper =
            std::upper_bound(sa.begin(), sa.end(), pattern,
                             [&](const std::string &p, const std::size_t idx) {
                                 return text.compare(idx, m, p) > 0;
                             });
        return std::distance(lower, upper);
    }

    /**
     * Time: O(m log n)
     */
    bool contains(const std::string &pattern) const { return count(pattern) > 0; }

    /**
     * Find the lexicographically k-th smallest substring of given length.
     *
     * @return The k-th substring, or empty string if k is out of range
     *
     * Time: O(n)
     */
    std::string kthSubstring(const std::size_t len, const std::size_t k) const {
        if (len > n) {
            return "";
        }
        std::size_t count = 0;
        for (std::size_t i = 0; i < n; i++) {
            if (sa[i] + len > n) {
                continue;
            }
            const std::size_t newSubstrings = len - lcp[i];
            if (count + newSubstrings > k) {
                const std::size_t offset = k - count;
                return text.substr(sa[i], lcp[i] + offset);
            }
            count += newSubstrings;
        }
        return "";
    }

    /**
     * Find all maximal repeats (substrings that occur 2+ times and cannot be extended).
     * A repeat is maximal if extending it left or right would make it occur fewer times.
     *
     * Time: O(n)
     */
    std::vector<SubString> maximalRepeats() const {
        std::vector<SubString> result;
        for (std::size_t i = 1; i < n; i++) {
            if (lcp[i] == 0) {
                continue;
            }
            bool isMaximal = true;
            if (i > 1 && lcp[i - 1] >= lcp[i]) {
                isMaximal = false;
            }
            if (i < sa.size() - 1 && lcp[i + 1] >= lcp[i]) {
                isMaximal = false;
            }
            if (isMaximal) {
                result.emplace_back(lcp[i], sa[i]);
            }
        }
        return result;
    }

    /**
     * Compute range minimum query on LCP array.
     * Returns minimum LCP value in range [l, r].
     *
     * This is used for finding LCP between any two suffixes.
     * LCP(suffix i, suffix j) = min(LCP[rank[i]+1], ..., LCP[rank[j]])
     *
     * Uses a sparse table for o(1) queries
     */
    std::size_t lcpRange(std::size_t l, std::size_t r) const {
        if (l > r) {
            std::swap(l, r);
        }
        if (l == r) {
            return n - sa[l];
        }
        SparseTable st = SparseTable(lcp);
        return st.query(l, r);
    }

    /**
     * Get LCP between two specific suffixes (by their starting positions).
     *
     * @param i Starting position of first suffix
     * @param j Starting position of second suffix
     * @return Length of longest common prefix
     */
    std::size_t lcpBetween(const std::size_t i, const std::size_t j) const {
        const std::size_t ri = rank[i];
        const std::size_t rj = rank[j];
        return lcpRange(ri, rj);
    }

    /**
     * Burrows-Wheeler transform.
     * Ensure the SA has a cyclic adjustment in the comparitor!
     *
     * @return Pair of (BWT string, original_pos)
     */
    std::pair<std::string, std::size_t> BWT() const {
        std::string bwt;
        std::size_t pos = 0;
        for (std::size_t i = 0; i < n; i++) {
            if (sa[i] == 0) {
                bwt += text[n - 1];
                pos = i;
            } else {
                bwt += text[sa[i] - 1];
            }
        }
        return {bwt, pos};
    }

    /**
     * Inverse Burrows-Wheeler
     *
     * * Method:
     * 1. Sort BWT to get first column
     * 2. Build "next" array using counting sort
     * 3. Follow pointers to reconstruct original
     *
     * @param bwt The BWT string
     * @param originalPos Position of original string in sorted rotations
     * @return Original text
     *
     * Time: O(n)
     */
    std::string inverse(const std::string &bwt, const std::size_t originalPos) {
        const std::size_t m = bwt.length();
        std::string first = bwt;
        sort(first.begin(), first.end());
        std::vector<int> count(256, 0);
        std::vector<int> next(m);
        for (const char c : bwt) {
            count[static_cast<unsigned char>(c)]++;
        }
        std::size_t sum = 0;
        for (std::size_t i = 0; i < 256; i++) {
            const std::size_t temp = count[i];
            count[i] = sum;
            sum += temp;
        }
        for (std::size_t i = 0; i < n; i++) {
            const unsigned char c = bwt[i];
            next[count[c]] = i;
            count[c]++;
        }
        std::string result;
        std::size_t idx = originalPos;
        for (std::size_t i = 0; i < n; i++) {
            result += first[idx];
            idx = next[idx];
        }
        return result;
    }
};

/**
 * Find longest common substring between two strings.
 *
 * Method: Concatenate strings with a separator: S1 + "#" + S2
 * Build suffix array on concatenated string.
 * Find maximum LCP where one suffix is from S1 and other from S2.
 *
 * Time: O(n1 + n2)
 */
inline SubString LCS(const std::string &a, const std::string &b) {
    const std::string combined = a + "#" + b;
    const SuffixArray SA = SuffixArray(combined);
    const std::size_t n = combined.size();
    const std::size_t separator = a.size();
    std::size_t maxLen = 0;
    std::size_t pos = 0;
    const auto &sa = SA.getSA();
    const auto &lcp = SA.getLCP();
    for (std::size_t i = 1; i < n; i++) {
        const bool firstFromS1 = sa[i - 1] < separator;
        const bool secondFromS1 = sa[i] < separator;
        if (firstFromS1 && secondFromS1) {
            continue;
        }
        if (lcp[i] > maxLen) {
            maxLen = lcp[i];
            pos = (sa[i] < separator) ? sa[i] : sa[i - 1];
        }
    }
    return {maxLen, pos};
}

/**
 * Specialised version for arbitrary integer arrays.
 */
class IntSuffixArray {
  private:
    std::vector<ll> text;
    std::size_t n;
    std::vector<std::size_t> sa;
    std::vector<std::size_t> lcp;
    std::vector<std::size_t> rank;

    void buildSA() {
        std::vector<std::size_t> temp(n);
        for (std::size_t i = 0; i < n; i++) {
            sa[i] = i;
            rank[i] = text[i];
        }
        for (std::size_t k = 1; k < n; k <<= 1) {
            const auto cmp = [&](std::size_t i, std::size_t j) {
                if (rank[i] != rank[j]) {
                    return rank[i] < rank[j];
                }
                i += k;
                j += k;
                const std::size_t ri = (i < n) ? rank[i] : 0;
                const std::size_t rj = (j < n) ? rank[j] : 0;
                return ri < rj;
            };
            std::sort(sa.begin(), sa.end(), cmp);
            temp[sa[0]] = 0;
            for (std::size_t i = 1; i < n; i++) {
                const bool equal = !cmp(sa[i - 1], sa[i]);
                temp[sa[i]] = temp[sa[i - 1]] + (equal ? 0 : 1);
            }
            rank = temp;
            if (rank[sa[n - 1]] == n - 1) {
                break;
            }
        }
    }

    void buildLCP() {
        lcp[0] = 0;
        std::size_t len = 0;
        for (std::size_t suff = 0; suff < n; suff++) {
            const std::size_t at = rank[suff];
            if (at <= 0) {
                continue;
            }
            const std::size_t prev = at - 1;
            const std::size_t prevSuff = sa[prev];
            std::size_t r = suff + len;
            std::size_t l = prevSuff + len;
            while (r < n && l < n && text[r] == text[l]) {
                len++;
                r = suff + len;
                l = prevSuff + len;
            }
            lcp[at] = len;
            if (len > 0) {
                len--;
            }
        }
    }

  public:
    explicit IntSuffixArray(const std::vector<ll> &in)
        : text(in), n(text.size()), sa(n), lcp(n), rank(n) {
        if (n == 0) {
            return;
        }
        buildSA();
        buildLCP();
    }
    const std::vector<std::size_t> &getSA() const { return sa; }

    const std::vector<std::size_t> &getLCP() const { return lcp; }

    const std::vector<std::size_t> &getRank() const { return rank; }
};

inline std::pair<std::vector<ll>, std::vector<ll>>
buildCombined(const std::vector<std::string> &strings) {
    const std::size_t n = strings.size();
    std::vector<ll> combined;
    std::vector<ll> ranges;
    for (std::size_t i = 0; i < n; i++) {
        if (i > 0) {
            combined.emplace_back(-LLONG_MAX + i);
            ranges.emplace_back(-1);
        }
        for (const char c : strings[i]) {
            combined.emplace_back((ll)c + n);
            ranges.emplace_back((ll)i);
        }
    }
    return {combined, ranges};
}

inline std::size_t getLCS(const std::vector<std::string> &strings) {
    const std::size_t n = strings.size();
    if (n == 0) {
        return 0;
    }
    if (n == 1) {
        return strings[0].length();
    }

    const auto [combined, ranges] = buildCombined(strings);
    const std::size_t len = combined.size();

    IntSuffixArray SA = IntSuffixArray(combined);
    const auto &sa = SA.getSA();
    const auto &lcp = SA.getLCP();
    SparseTable rmq = SparseTable(lcp);

    std::size_t result = 0;
    std::unordered_set<ll> inSpan;
    std::unordered_map<ll, size_t> counts;
    std::size_t j = 0;
    for (std::size_t i = 0; i < len; i++) {
        const ll range = ranges[sa[i]];
        if (range == -1) {
            continue;
        }
        inSpan.insert(range);
        counts[range]++;
        while (j < i) {
            const ll jRange = ranges[sa[j]];
            if (jRange == -1) {
                j++;
                continue;
            }
            if (counts[jRange] > 1) {
                counts[jRange]--;
                j++;
            } else {
                break;
            }
        }
        if (inSpan.size() == n && j < i) {
            const std::size_t minLCP = rmq.query(j + 1, i);
            result = std::max(result, minLCP);
        }
    }

    return result;
}

} // namespace algo::ds
