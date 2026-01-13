#pragma once

#include <algorithm>
#include <string>

inline std::string int128ToStr(__int128 x) {
    if (x == 0) {
        return "0";
    }
    std::string s;
    while (x > 0) {
        s += char('0' + (x % 10));
        x /= 10;
    }
    std::reverse(s.begin(), s.end());
    return s;
}
