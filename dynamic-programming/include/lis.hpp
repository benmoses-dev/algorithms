#pragma once

#include <algorithm>
#include <vector>

namespace algo::dp {

inline std::vector<std::size_t> LIS(const std::vector<int> &nums) {
    const std::size_t n = nums.size();
    if (n == 0) {
        return {};
    }
    if (n == 1) {
        return {0};
    }
    std::vector<std::size_t> len;
    len.reserve(n);
    std::vector<int> prev(n);
    len.emplace_back(0);
    prev[0] = -1;
    for (std::size_t i = 1; i < n; i++) {
        const int &num = nums[i];
        const std::size_t l = len.size();
        std::size_t pos = l;
        int high = static_cast<int>(l) - 1;
        int low = 0;
        while (low <= high) {
            const int mid = (high + low) / 2;
            const int &val = nums[len[mid]];
            if (val >= num) {
                high = mid - 1;
                pos = static_cast<std::size_t>(mid);
            } else {
                low = mid + 1;
            }
        }
        if (pos < l) {
            len[pos] = i;
        } else {
            len.emplace_back(i);
        }
        if (pos > 0) {
            prev[i] = static_cast<int>(len[pos - 1]);
        } else {
            prev[i] = -1;
        }
    }
    int p = static_cast<int>(len.back());
    std::vector<std::size_t> path;
    while (p >= 0) {
        path.emplace_back(static_cast<std::size_t>(p));
        p = prev[p];
    }
    std::reverse(path.begin(), path.end());
    return path;
}

} // namespace algo::dp
