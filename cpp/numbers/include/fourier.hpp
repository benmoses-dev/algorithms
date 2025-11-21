#pragma once

#include <cmath>
#include <complex>
#include <cstdint>
#include <numbers>
#include <vector>

using u64 = std::uint64_t;
const double TAU = 2 * std::numbers::pi;

inline std::vector<std::complex<double>> dft(const std::vector<double> &in,
                                             const bool inverse) {
    const u64 N = in.size();
    std::vector<std::complex<double>> spectrum;
    spectrum.reserve(N);
    for (u64 k = 0; k < N; k++) {
        const double freq = static_cast<double>(k) / static_cast<double>(N);
        double sumRe = 0.0;
        double sumIm = 0.0;
        for (u64 n = 0; n < N; n++) {
            const double angle = TAU * static_cast<double>(n) * freq * (inverse ? 1 : -1);
            sumIm += std::sin(angle) * in[n];
            sumRe += std::cos(angle) * in[n];
        }
        spectrum.emplace_back(sumRe, sumIm);
    }
    if (inverse) {
        for (auto &f : spectrum) {
            f /= static_cast<double>(spectrum.size());
        }
    }
    return spectrum;
}

inline std::vector<std::complex<double>>
fftRec(const std::vector<std::complex<double>> &in, const bool inverse) {
    const u64 N = in.size();
    if (N == 1) {
        return in;
    }
    std::vector<std::complex<double>> E;
    E.reserve(N / 2);
    std::vector<std::complex<double>> O;
    O.reserve(N / 2);
    for (u64 i = 0; i < N; i++) {
        if ((i & 1) == 0) {
            E.emplace_back(in[i]);
            continue;
        }
        O.emplace_back(in[i]);
    }
    auto even = fftRec(E, inverse);
    auto odd = fftRec(O, inverse);
    std::vector<std::complex<double>> spectrum(N);
    for (u64 k = 0; k < (N / 2); k++) {
        const double freq = static_cast<double>(k) / static_cast<double>(N);
        const double angle = TAU * freq * (inverse ? 1 : -1);
        const double twiddleRe = std::cos(angle);
        const double twiddleIm = std::sin(angle);
        const double oddRe = odd[k].real();
        const double oddIm = odd[k].imag();
        const double tempRe = twiddleRe * oddRe - twiddleIm * oddIm;
        const double tempIm = twiddleRe * oddIm + twiddleIm * oddRe;
        spectrum[k] =
            std::complex<double>(even[k].real() + tempRe, even[k].imag() + tempIm);
        spectrum[k + (N / 2)] =
            std::complex<double>(even[k].real() - tempRe, even[k].imag() - tempIm);
    }
    return spectrum;
}

inline std::vector<std::complex<double>> fft(std::vector<std::complex<double>> &in,
                                             const bool inverse) {
    const u64 N = in.size();
    if ((N & N - 1) != 0) {
        u64 p = 2;
        while (p < N) {
            p <<= 1;
        }
        const u64 diff = p - N;
        for (u64 i = 0; i < diff; i++) {
            in.emplace_back(0.0);
        }
    }
    std::vector<std::complex<double>> spectrum = fftRec(in, inverse);
    if (inverse) {
        for (auto &f : spectrum) {
            f /= static_cast<double>(spectrum.size());
        }
    }
    return spectrum;
}
