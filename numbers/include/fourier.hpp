#pragma once

#include <cmath>
#include <complex>
#include <cstdint>
#include <numbers>
#include <utility>
#include <vector>

namespace algo::numbers {

using u64 = std::uint64_t;

class FTransform {
  private:
    const double TAU = 2 * std::numbers::pi;

    void radix2Pad(std::vector<std::complex<double>> &in) const {
        const std::size_t N = in.size();
        if ((N & (N - 1)) != 0) {
            std::size_t p = 2;
            while (p < N) {
                p <<= 1;
            }
            const std::size_t diff = p - N;
            for (std::size_t i = 0; i < diff; i++) {
                in.emplace_back(0.0, 0.0);
            }
        }
    }

    void bitReverse(std::vector<std::complex<double>> &in) const {
        const std::size_t N = in.size();
        std::size_t j = 0;
        for (std::size_t i = 1; i < N; i++) {
            // Walk bit-wise backwards and swap
            std::size_t msb = N >> 1;
            while (j & msb) {
                j ^= msb;
                msb >>= 1;
            }
            j ^= msb;
            if (i < j) {
                std::swap(in[i], in[j]);
            }
        }
    }
    std::pair<std::complex<double>, std::complex<double>>
    applyTwiddle(const std::complex<double> &even, const std::complex<double> &odd,
                 const std::complex<double> &twiddle) const {
        const double tempRe = odd.real() * twiddle.real() - odd.imag() * twiddle.imag();
        const double tempIm = odd.real() * twiddle.imag() + odd.imag() * twiddle.real();
        return {std::complex<double>(even.real() + tempRe, even.imag() + tempIm),
                std::complex<double>(even.real() - tempRe, even.imag() - tempIm)};
    }

    std::pair<std::complex<double>, std::complex<double>>
    butterfly(const std::complex<double> &even, const std::complex<double> &odd,
              const double freq, const bool inverse) const {
        const double angle = TAU * freq * (inverse ? 1 : -1);
        const std::complex<double> twiddle = {std::cos(angle), std::sin(angle)};
        return applyTwiddle(even, odd, twiddle);
    }

    std::vector<std::complex<double>> fftRec(const std::vector<std::complex<double>> &in,
                                             const bool inverse) const {

        const std::size_t N = in.size();
        if (N == 1) {
            return in;
        }
        const std::size_t half = N / 2;
        std::vector<std::complex<double>> E;
        E.reserve(half);
        std::vector<std::complex<double>> O;
        O.reserve(half);
        for (std::size_t i = 0; i < N; i++) {
            if (i & 1) {
                O.emplace_back(in[i]);
                continue;
            }
            E.emplace_back(in[i]);
        }
        const auto even = fftRec(E, inverse);
        const auto odd = fftRec(O, inverse);
        std::vector<std::complex<double>> spectrum(N);
        for (std::size_t k = 0; k < half; k++) {
            const double freq = static_cast<double>(k) / static_cast<double>(N);
            const auto [first, second] = butterfly(even[k], odd[k], freq, inverse);
            spectrum[k] = first;
            spectrum[k + half] = second;
        }
        return spectrum;
    }

    void fftIt(std::vector<std::complex<double>> &in, const bool inverse) const {
        const std::size_t N = in.size();
        if (N == 1) {
            return;
        }
        bitReverse(in);
        for (std::size_t length = 2; length <= N; length <<= 1) {
            const std::size_t half = length / 2;
            std::vector<std::complex<double>> twiddles(half);
            for (std::size_t k = 0; k < half; k++) {
                const double angle = TAU * static_cast<double>(k) /
                                     static_cast<double>(length) * (inverse ? 1 : -1);
                twiddles[k] = {std::cos(angle), std::sin(angle)};
            }
            for (std::size_t i = 0; i < N; i += length) {
                for (std::size_t k = 0; k < half; k++) {
                    const std::complex<double> &even = in[i + k];
                    const std::complex<double> &odd = in[i + k + half];
                    const auto [first, second] = applyTwiddle(even, odd, twiddles[k]);
                    in[i + k] = first;
                    in[i + k + half] = second;
                }
            }
        }
    }

    template <typename T>
    std::vector<std::complex<double>> normalise(const std::vector<T> &in) const {
        if constexpr (std::is_same_v<T, std::complex<double>>) {
            return in;
        }
        const std::size_t N = in.size();
        std::vector<std::complex<double>> res(N, {0.0, 0.0});
        for (std::size_t i = 0; i < N; i++) {
            res[i] = in[i];
        }
        return res;
    }

  public:
    explicit FTransform() {}

    /**
     * Don't actually use this - this is just for reference...
     */
    template <typename T>
    std::vector<std::complex<double>> dft(const std::vector<T> &input,
                                          const bool inverse) const {
        const std::vector<std::complex<double>> in = normalise(input);
        const std::size_t N = in.size();
        std::vector<std::complex<double>> spectrum;
        spectrum.reserve(N);
        for (std::size_t k = 0; k < N; k++) {
            const double freq = static_cast<double>(k) / static_cast<double>(N);
            double sumRe = 0.0;
            double sumIm = 0.0;
            for (std::size_t i = 0; i < N; i++) {
                const double angle =
                    TAU * static_cast<double>(i) * freq * (inverse ? 1 : -1);
                const double cosine = std::cos(angle);
                const double sine = std::sin(angle);
                sumRe += in[i].real() * cosine - in[i].imag() * sine;
                sumIm += in[i].real() * sine + in[i].imag() * cosine;
            }
            spectrum.emplace_back(sumRe, sumIm);
        }
        if (inverse) {
            for (auto &f : spectrum) {
                f /= static_cast<double>(N);
            }
        }
        return spectrum;
    }

    /**
     * Iterative version is faster...
     */
    template <typename T>
    std::vector<std::complex<double>> fft(const std::vector<T> &input,
                                          const bool inverse = false,
                                          const bool recursive = false) const {
        std::vector<std::complex<double>> spectrum = normalise(input);
        radix2Pad(spectrum);
        if (recursive) {
            spectrum = fftRec(spectrum, inverse);
        } else {
            fftIt(spectrum, inverse);
        }
        if (inverse) {
            for (auto &f : spectrum) {
                f /= static_cast<double>(spectrum.size());
            }
        }
        return spectrum;
    }
};

} // namespace algo::numbers
