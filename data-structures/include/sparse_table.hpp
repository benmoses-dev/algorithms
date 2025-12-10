#pragma once

#include <algorithm>
#include <stdexcept>
#include <vector>

namespace algo::ds {

class SparseTable {
  private:
    std::vector<std::vector<int>> st;
    std::vector<std::size_t> lg;
    std::size_t n;

    int combine(int a, int b) { return std::min(a, b); }

  public:
    SparseTable(std::vector<int> &arr) {
        n = arr.size();
        std::size_t maxLog = std::__lg(n) + 1;
        st.assign(n, std::vector<int>(maxLog));
        lg.assign(n + 1, 0);
        for (std::size_t i = 2; i <= n; i++) {
            lg[i] = lg[i / 2] + 1;
        }
        for (std::size_t i = 0; i < n; i++) {
            st[i][0] = arr[i];
        }
        for (std::size_t j = 1; j < maxLog; j++) {
            for (std::size_t i = 0; i + (1 << j) <= n; i++) {
                st[i][j] = combine(st[i][j - 1], st[i + (1 << (j - 1))][j - 1]);
            }
        }
    }

    int query(std::size_t L, std::size_t R) {
        if (L > R) {
            throw std::invalid_argument("L cannot be greater than R");
        }
        std::size_t len = R - L + 1;
        std::size_t k = lg[len];
        return combine(st[L][k], st[R - (1 << k) + 1][k]);
    }
};

} // namespace algo::ds
