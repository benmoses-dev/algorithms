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

inline std::vector<std::complex<double>> fftRec(std::vector<double> &re) {
    const u64 N = re.size();
    if (N == 1) {
        return {std::complex<double>(re[0], 0.0)};
    }
    std::vector<double> E;
    E.reserve(N / 2);
    std::vector<double> O;
    O.reserve(N / 2);
    for (u64 i = 0; i < N; i++) {
        if (i % 2 == 0) {
            E.emplace_back(re[i]);
            continue;
        }
        O.emplace_back(re[i]);
    }
    auto even = fftRec(E);
    auto odd = fftRec(O);
    std::vector<std::complex<double>> transform(N);
    for (u64 k = 0; k < (N / 2); k++) {
        const double tau = 2 * std::numbers::pi;
        const double freq = static_cast<double>(k) / static_cast<double>(N);
        const double angle = tau * freq;
        const double twiddleRe = std::cos(angle);
        const double twiddleIm = std::sin(-angle);
        const double oddRe = odd[k].real();
        const double oddIm = odd[k].imag();
        const double tempRe = twiddleRe * oddRe - twiddleIm * oddIm;
        const double tempIm = twiddleRe * oddIm + twiddleIm * oddRe;
        transform[k] =
            std::complex<double>(even[k].real() + tempRe, even[k].imag() + tempIm);
        transform[k + (N / 2)] =
            std::complex<double>(even[k].real() - tempRe, even[k].imag() - tempIm);
    }
    return transform;
}

inline std::vector<std::complex<double>> fft(std::vector<double> &re) {
    const u64 N = re.size();
    if ((N & N - 1) != 0) {
        u64 p = 2;
        while (p < N) {
            p <<= 1;
        }
        const u64 diff = p - N;
        for (u64 i = 0; i < diff; i++) {
            re.emplace_back(0.0);
        }
    }
    return fftRec(re);
}
