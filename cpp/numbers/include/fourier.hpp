#pragma once

#include <cmath>
#include <complex>
#include <cstdint>
#include <numbers>
#include <vector>

using u64 = std::uint64_t;

inline std::vector<std::complex<double>> dft(std::vector<double> &re) {
    const u64 N = re.size();
    std::vector<std::complex<double>> transform;
    transform.reserve(N);
    const double tau = 2 * std::numbers::pi;
    for (u64 k = 0; k < N; k++) {
        const double freq = static_cast<double>(k) / static_cast<double>(N);
        double sumRe = 0.0;
        double sumIm = 0.0;
        for (u64 n = 0; n < N; n++) {
            const double angle = tau * static_cast<double>(n) * freq;
            sumIm -= std::sin(angle) * re[n];
            sumRe += std::cos(angle) * re[n];
        }
        transform.emplace_back(sumRe, sumIm);
    }
    return transform;
}
