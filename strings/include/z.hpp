#pragma once

#include <algorithm>
#include <string>
#include <vector>

namespace algo::strings {

/**
 * Z Algorithm: For each position i, computes Z[i] = length of longest substring
 * starting at i that matches a prefix of the string.
 *
 * Example: "aabcaabxaaz"
 * Z = [-, 1, 0, 0, 3, 1, 0, 0, 2, 1, 0]
 *      ^ undefined (entire string)
 *        ^ "a" matches prefix "a" (length 1)
 *          ^ "b" doesn't match prefix "a" (length 0)
 *              ^ "aab" matches prefix "aab" (length 3)
 *                      ^ "aa" matches prefix "aa" (length 2)
 *
 * Time Complexity: O(n)
 * Space Complexity: O(n)
 */
class ZAlgorithm {
  private:
    std::string text;
    std::size_t n;
    std::vector<std::size_t> z;

    /**
     * Compute Z array using the Z algorithm.
     *
     * Maintain [L, R] = rightmost Z-box found so far
     * A Z-box is a substring that matches a prefix.
     *
     * For position i:
     * - If i > R: Compute Z[i] naively by comparing characters
     * - If i <= R: We're inside a Z-box, can reuse previous computation
     *
     * The algorithm ensures each character is compared at most twice.
     */
    void computeZ() {
        z.resize(n);
        std::size_t L = 0, R = 0;
        for (std::size_t i = 1; i < n; i++) {
            if (i < R) {
                z[i] = std::min(z[i - L], R - i);
            }
            while (i + z[i] < n && text[z[i]] == text[i + z[i]]) {
                z[i]++;
            }
            if (i + z[i] > R) {
                L = i;
                R = i + z[i];
            }
        }
    }

  public:
    explicit ZAlgorithm(const std::string &in) : text(in), n(text.length()) {
        if (n > 0) {
            computeZ();
        }
    }

    const std::vector<std::size_t> &getZ() const { return z; }

    const std::string &getText() const { return text; }
};

/**
 * Pattern matching using Z algorithm.
 *
 * Method: Concatenate pattern and text with separator
 * Build Z array. Positions where Z[i] == |P| are matches.
 *
 * Important: The separator MUST NOT exist in the pattern or text!
 *
 * @param pattern The pattern to search for
 * @return Vector of positions where pattern occurs in text
 *
 * Time: O(|text| + |pattern|)
 */
inline std::vector<std::size_t> search(const std::string &pattern,
                                       const std::string &text) {
    std::vector<std::size_t> result;
    const std::size_t n = text.length();
    if (pattern.empty() || pattern.length() > n) {
        return result;
    }
    std::string combined = pattern + "$" + text; // Or use \1 if $ could exist
    ZAlgorithm z(combined);
    const auto &zArray = z.getZ();
    std::size_t m = pattern.length();
    for (std::size_t i = m + 1; i < combined.length(); i++) {
        if (zArray[i] == m) {
            result.emplace_back(i - m - 1);
        }
    }
    return result;
}

} // namespace algo::strings
