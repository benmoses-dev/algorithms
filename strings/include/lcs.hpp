#pragma once

#include <algorithm>
#include <string>
#include <vector>

namespace algo::strings {

/**
 * Longest Common Subsequence (LCS): Find longest sequence that appears
 * in both strings (characters don't need to be contiguous).
 *
 * Example:
 * S1 = "ABCDGH"
 * S2 = "AEDFHR"
 * LCS = "ADH" (length 3)
 *
 * Time Complexity: O(n * m)
 * Space Complexity: O(n * m) or O(min(n, m)) with optimisations
 */
class LCS {
  private:
    std::string s1, s2;
    std::size_t n, m;
    std::vector<std::vector<int>> dp;

  public:
    /**
     * Build the DP table.
     *
     * DP recurrence:
     * dp[i][j] = LCS length of s1[0..i-1] and s2[0..j-1]
     *
     * If s1[i-1] == s2[j-1]:
     *   dp[i][j] = dp[i-1][j-1] + 1  (characters match, extend LCS)
     * Else:
     *   dp[i][j] = max(dp[i-1][j], dp[i][j-1])  (skip one character)
     */
    explicit LCS(const std::string &s1, const std::string &s2)
        : s1(s1), s2(s2), n(s1.length()), m(s2.length()) {
        dp.assign(n + 1, std::vector<int>(m + 1, 0));
        for (std::size_t i = 1; i <= n; i++) {
            for (std::size_t j = 1; j <= m; j++) {
                if (s1[i - 1] == s2[j - 1]) {
                    dp[i][j] = dp[i - 1][j - 1] + 1;
                } else {
                    dp[i][j] = std::max(dp[i - 1][j], dp[i][j - 1]);
                }
            }
        }
    }

    int length() { return dp[n][m]; }

    /**
     * Get the actual LCS string (one of possibly many).
     *
     * Backtracks through DP table to reconstruct the sequence.
     *
     * Time: O(n + m)
     */
    std::string getLCS() {
        std::string result;
        std::size_t i = n, j = m;
        while (i > 0 && j > 0) {
            if (s1[i - 1] == s2[j - 1]) {
                result += s1[i - 1];
                i--;
                j--;
            } else if (dp[i - 1][j] > dp[i][j - 1]) {
                i--;
            } else {
                j--;
            }
        }
        std::reverse(result.begin(), result.end());
        return result;
    }

    /**
     * Get the DP table (for debugging or further processing).
     */
    const std::vector<std::vector<int>> &getDP() { return dp; }
};

/**
 * Space-optimised LCS (only computes length, not actual sequence).
 */
class LCSOptimised {
  private:
    std::string s1, s2;
    std::size_t n, m;

  public:
    LCSOptimised(const std::string &s1, const std::string &s2)
        : s1(s1), s2(s2), n(s1.length()), m(s2.length()) {
        if (n > m) {
            std::swap(this->s1, this->s2);
            std::swap(n, m);
        }
    }

    /**
     * Compute LCS length with O(min(n, m)) space.
     *
     * We only need the previous row to compute current row.
     */
    int length() {
        std::vector<int> prev(n + 1, 0);
        std::vector<int> curr(n + 1, 0);
        for (std::size_t j = 1; j <= m; j++) {
            for (std::size_t i = 1; i <= n; i++) {
                if (s1[i - 1] == s2[j - 1]) {
                    curr[i] = prev[i - 1] + 1;
                } else {
                    curr[i] = std::max(curr[i - 1], prev[i]);
                }
            }
            std::swap(prev, curr);
        }
        return prev[n];
    }
};

/**
 * Edit Distance (Levenshtein Distance): Minimum operations to transform s1 to s2.
 * Operations: insert, delete, replace
 *
 * Related to LCS: edit_distance = n + m - 2 * LCS_length (for insert/delete only)
 */
class EditDistance {
  private:
    std::string s1, s2;
    std::size_t n, m;
    std::vector<std::vector<int>> dp;

  public:
    /**
     * Compute minimum edit distance.
     *
     * DP recurrence:
     * dp[i][j] = min edits to transform s1[0..i-1] to s2[0..j-1]
     *
     * If s1[i-1] == s2[j-1]:
     *   dp[i][j] = dp[i-1][j-1]  (no operation needed)
     * Else:
     *   dp[i][j] = 1 + min(
     *       dp[i-1][j],      // delete from s1
     *       dp[i][j-1],      // insert into s1
     *       dp[i-1][j-1]     // replace
     *   )
     */
    EditDistance(const std::string &s1, const std::string &s2)
        : s1(s1), s2(s2), n(s1.length()), m(s2.length()),
          dp(n + 1, std::vector<int>(m + 1, 0)) {
        for (std::size_t i = 0; i <= n; i++) {
            dp[i][0] = i;
        }
        for (std::size_t j = 0; j <= m; j++) {
            dp[0][j] = j;
        }
        for (std::size_t i = 1; i <= n; i++) {
            for (std::size_t j = 1; j <= m; j++) {
                if (s1[i - 1] == s2[j - 1]) {
                    dp[i][j] = dp[i - 1][j - 1];
                } else {
                    dp[i][j] =
                        std::min({dp[i - 1][j], dp[i][j - 1], dp[i - 1][j - 1]}) + 1;
                }
            }
        }
    }

    int distance() { return dp[n][m]; }

    /**
     * Get the sequence of operations.
     */
    std::vector<std::string> getOperations() {
        std::vector<std::string> ops;
        std::size_t i = n, j = m;
        while (i > 0 || j > 0) {
            if (i == 0) {
                ops.emplace_back("Insert '" + std::string(1, s2[j - 1]) + "'");
                j--;
            } else if (j == 0) {
                ops.emplace_back("Delete '" + std::string(1, s1[i - 1]) + "'");
                i--;
            } else if (s1[i - 1] == s2[j - 1]) {
                ops.emplace_back("Match '" + std::string(1, s1[i - 1]) + "'");
                i--;
                j--;
            } else {
                const int deleteCost = dp[i - 1][j];
                const int insertCost = dp[i][j - 1];
                const int replaceCost = dp[i - 1][j - 1];
                if (deleteCost <= insertCost && deleteCost <= replaceCost) {
                    ops.emplace_back("Delete '" + std::string(1, s1[i - 1]) + "'");
                    i--;
                } else if (insertCost <= replaceCost) {
                    ops.emplace_back("Insert '" + std::string(1, s2[j - 1]) + "'");
                    j--;
                } else {
                    ops.emplace_back("Replace '" + std::string(1, s1[i - 1]) +
                                     "' with '" + std::string(1, s2[j - 1]) + "'");
                    i--;
                    j--;
                }
            }
        }
        std::reverse(ops.begin(), ops.end());
        return ops;
    }
};

} // namespace algo::strings
