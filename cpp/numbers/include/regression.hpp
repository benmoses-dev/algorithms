#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <vector>

namespace algo::numbers {

using i64 = std::int64_t;
using u64 = std::uint64_t;

const double EPS = 1e-9;
const double nan = std::numeric_limits<double>::quiet_NaN();

inline double stcast(const u64 x) { return static_cast<double>(x); }

/**
 * Safely cast a double to u64.
 * Returns 0 if value is negative, NaN, infinite, or exceeds u64 max.
 */
inline u64 castu64(double v) {
    if (!std::isfinite(v))
        return 0;
    if (v < 0.0)
        return 0;
    const double maxd = static_cast<double>(std::numeric_limits<u64>::max());
    if (v > maxd)
        return 0;
    return static_cast<u64>(v);
};

/**
 * Structure to store the result of a linear regression.
 * t: predicted "time" for a given target y
 * m: slope of the regression line
 * c: y-intercept of the regression line
 * stDev: standard deviation of residuals
 */
struct Result {
    double t;
    double m;
    double c;
    double stDev;
};

/**
 * Performs simple linear regression on a vector of data.
 *
 * Assumes x-values are sequential integers: 0, 1, 2, ..., n-1.
 * The goal is to find the line y = m*x + c that minimizes the sum of squared residuals:
 *
 *   L(c, m) = Σ_i (y_i - (c + m*x_i))^2
 *
 * Solving ∂L/∂c = 0 and ∂L/∂m = 0 for the parabola L gives:
 *
 *   m = Σ_i (x_i - x̄)*(y_i - ȳ) / Σ_i (x_i - x̄)^2
 *   c = ȳ - m * x̄
 *
 * Where x̄ and ȳ are the mean of x and y values respectively.
 *
 * Standard deviation of residuals (stDev) is calculated as:
 *
 *   stDev = sqrt(Σ_i (y_i - (c + m*x_i))^2 / (n - 2))
 *
 * Predicts t for a given target y using the regression line:
 *
 *   t = (target - c) / m
 *
 * Returns NaN if slope is near zero or insufficient data for standard deviation.
 */
inline Result endPred(const std::vector<u64> &data, const u64 target) {
    const std::size_t s = data.size();
    double ySum = 0;
    double xSum = 0.0;
    for (std::size_t i = 0; i < s; i++) {
        ySum += stcast(data[i]);
        xSum += stcast(i);
    }
    const double xAvg = xSum / stcast(s);
    const double yAvg = ySum / stcast(s);
    double numerator = 0.0;
    double denominator = 0.0;
    for (std::size_t i = 0; i < s; i++) {
        const double y = stcast(data[i]);
        const double yDelta = y - yAvg;
        const double xDelta = stcast(i) - xAvg;
        const double coVar = xDelta * yDelta;
        numerator += coVar;
        denominator += (xDelta * xDelta);
    }
    if (denominator < EPS) {
        return {nan, nan, nan, nan};
    }
    const double m = numerator / denominator;
    const double c = yAvg - (m * xAvg);
    double sqRes = 0.0;
    for (std::size_t i = 0; i < s; i++) {
        const double x = stcast(i);
        const double y = stcast(data[i]);
        const double yHat = c + m * x;
        sqRes += (y - yHat) * (y - yHat);
    }
    double stDev = nan;
    if (s > 2) {
        stDev = std::sqrt(sqRes / (stcast(s) - 2.0));
    }
    double t = nan;
    if (std::fabs(m) > EPS) {
        t = (stcast(target) - c) / m;
    }
    return {t, m, c, stDev};
}

/**
 * Combine regression results from multiple time periods (year, month, week, day) to
 * compute two estimates:
 * 1. averageEnd: the mean predicted end time across valid regressions
 * 2. worstEnd: the predicted end time accounting for the worst-case standard deviation
 *
 * ci: confidence interval to adjust the worst-case estimate (default 0.95 for 95% CI)
 *
 * Only regressions with finite slope and prediction are included.
 * WorstEnd uses the maximum residual standard deviation among the valid regressions.
 */
inline std::pair<u64, u64> fullTimestamps(const std::vector<u64> &yearData,
                                          const std::vector<u64> &monthData,
                                          const std::vector<u64> &weekData,
                                          const std::vector<u64> &dayData,
                                          const u64 target, const double ci = 0.95) {
    const auto yearR = endPred(yearData, target);
    const auto monthR = endPred(monthData, target);
    const auto weekR = endPred(weekData, target);
    const auto dayR = endPred(dayData, target);
    std::vector<Result> all = {yearR, monthR, weekR, dayR};
    std::vector<Result> valid;
    for (auto &r : all) {
        if (std::isfinite(r.m) && std::isfinite(r.t)) {
            valid.push_back(r);
        }
    }
    double sumPred = 0.0;
    for (auto &r : valid)
        sumPred += r.t;
    const double averageEnd = sumPred / static_cast<double>(valid.size());
    double sumM = 0.0, sumC = 0.0;
    for (auto &r : valid) {
        sumM += r.m;
        sumC += r.c;
    }
    const double avgM = sumM / static_cast<double>(valid.size());
    const double avgC = sumC / static_cast<double>(valid.size());
    double worstStd = 0.0;
    bool foundStd = false;
    for (auto &r : valid) {
        if (std::isfinite(r.stDev)) {
            worstStd = std::max(worstStd, r.stDev);
            foundStd = true;
        }
    }
    double worstEnd = nan;
    const double z = (0.5 + (ci / 2.0));
    if (foundStd) {
        const double shift = std::copysign(worstStd * z, avgM);
        worstEnd = (static_cast<double>(target) - avgC - shift) / avgM;
    } else {
        worstEnd = (static_cast<double>(target) - avgC) / avgM;
    }
    return {castu64(averageEnd), castu64(worstEnd)};
}

} // namespace algo::numbers
