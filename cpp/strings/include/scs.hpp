#pragma once

#include <algorithm>
#include <string>
#include <vector>

namespace algo::strings {

/**
 * Build up a DP recurrence based on the fact that the current shortest supersequence
 * depends on whether we can merge the two latest characters or not, and on the length
 * of the shortest supersequence found so far.
 */
inline std::string shortestSupersequence(std::string a, std::string b) {
    std::size_t m = a.size();
    std::size_t n = b.size();

    std::vector<std::vector<std::size_t>> dp(m + 1, std::vector<std::size_t>(n + 1));

    for (std::size_t i = 0; i <= m; i++)
        dp[i][0] = i;
    for (std::size_t j = 0; j <= n; j++)
        dp[0][j] = j;

    /**
     * Compare characters from b to characters from a (in increasing length)
     * Recurrence:
     * let scs(a, b) =
     * a[i - 1] == b[j -1] => scs(a[i - 1], b[j - 1]) + 1
     * a[i - 1] != b[j -1] => min(scs(a[i - 1], b[j]), scs(a[i], b[j - 1])) + 1
     */
    for (std::size_t i = 1; i <= m; i++)
        for (std::size_t j = 1; j <= n; j++)
            if (a[i - 1] == b[j - 1]) {
                dp[i][j] = 1 + dp[i - 1][j - 1];
            } else {
                dp[i][j] = 1 + std::min(dp[i - 1][j], dp[i][j - 1]);
            }

    std::string result;
    std::size_t i = m;
    std::size_t j = n;
    while (i > 0 && j > 0) {
        if (a[i - 1] == b[j - 1]) { // We only added this once, move diagonally
            result += a[i - 1];
            i--;
            j--;
        } else if (dp[i - 1][j] <= dp[i][j - 1]) {
            /**
             * We added a[i - 1] and recursed scs(a[i - 1], b[j])
             * If they are the same, min() prefers the first
             */
            result += a[i - 1];
            i--; // Move the pointer in the direction that we recursed into
        } else {
            // We added b[j - 1] and recursed scs(a[i], b[j - 1])
            result += b[j - 1];
            j--;
        }
    }

    // Add remaining
    while (i > 0) {
        result += a[i - 1];
        i--;
    }
    while (j > 0) {
        result += b[j - 1];
        j--;
    }

    // We built in reserve, so we need to reverse the reversed!
    reverse(result.begin(), result.end());
    return result;
}

} // namespace algo::strings
