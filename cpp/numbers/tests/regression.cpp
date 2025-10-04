#include "regression.hpp"
#include <iomanip>
#include <iostream>
#include <vector>

using namespace algo::numbers;

int main() {
    // Simulate data over different time windows
    // Trend:  growth rate slows slightly in shorter windows

    // Yearly data: steady linear increase
    std::vector<u64> yearData;
    for (std::size_t i = 0; i < 365; i++) {
        const double day = stcast(i);
        const double y = 100.0 + 2.5 * day + (std::sin(day / 10.0) * 5.0);
        yearData.push_back(castu64(y));
    }

    // Monthly data: steeper recent trend
    std::vector<u64> monthData;
    for (std::size_t i = 0; i < 30; i++) {
        const double day = stcast(i);
        const double y = 800.0 + 3.0 * day + (std::cos(day / 4.0) * 2.0);
        monthData.push_back(castu64(y));
    }

    // Weekly data: more volatile, faster increase
    std::vector<u64> weekData;
    for (std::size_t i = 0; i < 7; i++) {
        const double day = stcast(i);
        const double y = 900.0 + 5.0 * day + (std::sin(day * 1.3) * 4.0);
        weekData.push_back(castu64(y));
    }

    // Daily data: small sample, small fluctuations
    std::vector<u64> dayData;
    for (std::size_t i = 0; i < 24; i++) { // hourly samples
        const double hour = stcast(i);
        const double y = 950.0 + 1.5 * hour + (std::sin(hour / 2.0) * 1.0);
        dayData.push_back(castu64(y));
    }

    const u64 target = 1500;

    const auto [avgEnd, worstEnd] =
        getTimestamps(yearData, monthData, weekData, dayData, target);

    std::cout << std::fixed << std::setprecision(2);

    std::cout << "-----------------------------------------------\n";
    std::cout << "  REGRESSION PREDICTION TEST\n";
    std::cout << "-----------------------------------------------\n";
    std::cout << "Average predicted exhaustion time index: " << avgEnd << "\n";
    std::cout << "Worst-case predicted exhaustion time index: " << worstEnd << "\n";

    // Inspect intermediate regressions too
    const auto yr = endPred(yearData, target);
    const auto mo = endPred(monthData, target);
    const auto wk = endPred(weekData, target);
    const auto dy = endPred(dayData, target);

    const auto printRes = [](const std::string &label, const Result &r) {
        std::cout << label << ":\n";
        std::cout << "  slope (m): " << r.m << "\n";
        std::cout << "  intercept (c): " << r.c << "\n";
        std::cout << "  std dev: " << r.stDev << "\n";
        std::cout << "  predicted end: " << r.t << "\n\n";
    };

    std::cout << "\n--- Individual regressions ---\n";
    printRes("Year", yr);
    printRes("Month", mo);
    printRes("Week", wk);
    printRes("Day", dy);

    // Simple interpretation
    if (avgEnd != 0)
        std::cout << "Predicted average exhaustion after ~" << avgEnd << " time units.\n";
    if (worstEnd != 0)
        std::cout << "Predicted worst-case exhaustion after ~" << worstEnd
                  << " time units.\n";

    std::cout << "-----------------------------------------------\n";
    return 0;
}
