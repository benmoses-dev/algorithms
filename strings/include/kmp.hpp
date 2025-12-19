#include <string>
#include <vector>

namespace algo::strings {

inline std::vector<std::size_t> LPS(const std::string &pattern) {
    const std::size_t m = pattern.length();
    std::vector<std::size_t> lps(m, 0);
    std::size_t len = 0;
    for (std::size_t i = 1; i < m; i++) {
        while (len > 0 && pattern[i] != pattern[len]) {
            len = lps[len - 1];
        }
        if (pattern[i] == pattern[len]) {
            len++;
        }
        lps[i] = len;
    }
    return lps;
}

inline std::vector<std::size_t> KMP(const std::string &text, const std::string &pattern) {
    std::vector<std::size_t> result;
    std::vector<std::size_t> lps = LPS(pattern);
    const std::size_t n = text.length();
    const std::size_t m = pattern.length();
    std::size_t j = 0;
    for (std::size_t i = 0; i < n; i++) {
        while (j > 0 && text[i] != pattern[j]) {
            j = lps[j - 1];
        }
        if (text[i] == pattern[j]) {
            j++;
        }
        if (j == m) {
            result.emplace_back(i - m + 1);
            j = lps[j - 1];
        }
    }
    return result;
}

} // namespace algo::strings
