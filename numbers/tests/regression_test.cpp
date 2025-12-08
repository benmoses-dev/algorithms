#include "regression.hpp"
#include <gtest/gtest.h>
#include <iomanip>
#include <iostream>
#include <vector>

using namespace algo::numbers;

class RegressionTest : public ::testing::Test {
  protected:
    std::vector<u64> yearData;
    std::vector<u64> monthData;
    std::vector<u64> weekData;
    std::vector<u64> dayData;
    u64 target = 1500;

    void SetUp() override {
        // Yearly data: steady linear increase
        for (std::size_t i = 0; i < 365; i++) {
            const double day = stcast(i);
            const double y = 100.0 + 2.5 * day + (std::sin(day / 10.0) * 5.0);
            yearData.push_back(castu64(y));
        }

        // Monthly data: steeper recent trend
        for (std::size_t i = 0; i < 30; i++) {
            const double day = stcast(i);
            const double y = 800.0 + 3.0 * day + (std::cos(day / 4.0) * 2.0);
            monthData.push_back(castu64(y));
        }

        // Weekly data: more volatile, faster increase
        for (std::size_t i = 0; i < 7; i++) {
            const double day = stcast(i);
            const double y = 900.0 + 5.0 * day + (std::sin(day * 1.3) * 4.0);
            weekData.push_back(castu64(y));
        }

        // Daily data: small sample, small fluctuations
        for (std::size_t i = 0; i < 24; i++) {
            const double hour = stcast(i);
            const double y = 950.0 + 1.5 * hour + (std::sin(hour / 2.0) * 1.0);
            dayData.push_back(castu64(y));
        }
    }

    void printResult(const std::string &label, const Result &r) {
        std::cout << label << ":\n";
        std::cout << "  slope (m): " << r.m << "\n";
        std::cout << "  intercept (c): " << r.c << "\n";
        std::cout << "  std dev: " << r.stDev << "\n";
        std::cout << "  predicted end: " << r.t << "\n\n";
    }
};

TEST_F(RegressionTest, DataGeneration) {
    EXPECT_EQ(yearData.size(), 365);
    EXPECT_EQ(monthData.size(), 30);
    EXPECT_EQ(weekData.size(), 7);
    EXPECT_EQ(dayData.size(), 24);

    // Verify data ranges are reasonable
    EXPECT_GT(yearData.front(), 0);
    EXPECT_GT(monthData.front(), 0);
    EXPECT_GT(weekData.front(), 0);
    EXPECT_GT(dayData.front(), 0);
}

TEST_F(RegressionTest, IndividualRegressions) {
    const auto yr = endPred(yearData, target);
    const auto mo = endPred(monthData, target);
    const auto wk = endPred(weekData, target);
    const auto dy = endPred(dayData, target);

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "\n--- Individual regressions ---\n";
    printResult("Year", yr);
    printResult("Month", mo);
    printResult("Week", wk);
    printResult("Day", dy);

    // Verify slopes are positive (data is increasing)
    EXPECT_GT(yr.m, 0.0) << "Year regression should have positive slope";
    EXPECT_GT(mo.m, 0.0) << "Month regression should have positive slope";
    EXPECT_GT(wk.m, 0.0) << "Week regression should have positive slope";
    EXPECT_GT(dy.m, 0.0) << "Day regression should have positive slope";

    // Verify predicted endpoints are reasonable
    EXPECT_GT(yr.t, 0) << "Year prediction should be positive";
    EXPECT_GT(mo.t, 0) << "Month prediction should be positive";
    EXPECT_GT(wk.t, 0) << "Week prediction should be positive";
    EXPECT_GT(dy.t, 0) << "Day prediction should be positive";

    // Verify standard deviations are non-negative
    EXPECT_GE(yr.stDev, 0.0);
    EXPECT_GE(mo.stDev, 0.0);
    EXPECT_GE(wk.stDev, 0.0);
    EXPECT_GE(dy.stDev, 0.0);
}

TEST_F(RegressionTest, CombinedPrediction) {
    const auto [avgEnd, worstEnd] =
        getTimestamps(yearData, monthData, weekData, dayData, target);

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "-----------------------------------------------\n";
    std::cout << "  REGRESSION PREDICTION TEST\n";
    std::cout << "-----------------------------------------------\n";
    std::cout << "Average predicted exhaustion time index: " << avgEnd << "\n";
    std::cout << "Worst-case predicted exhaustion time index: " << worstEnd << "\n";

    if (avgEnd != 0) {
        std::cout << "Predicted average exhaustion after ~" << avgEnd << " time units.\n";
    }
    if (worstEnd != 0) {
        std::cout << "Predicted worst-case exhaustion after ~" << worstEnd
                  << " time units.\n";
    }
    std::cout << "-----------------------------------------------\n";

    // Verify predictions are reasonable
    EXPECT_GT(avgEnd, 0) << "Average prediction should be positive";
    EXPECT_GT(worstEnd, 0) << "Worst-case prediction should be positive";

    // Worst-case should be more conservative (earlier or equal) than average
    EXPECT_LE(worstEnd, avgEnd)
        << "Worst-case should predict exhaustion at or before average case";
}

TEST_F(RegressionTest, SlopeOrdering) {
    const auto yr = endPred(yearData, target);
    const auto mo = endPred(monthData, target);
    const auto wk = endPred(weekData, target);
    const auto dy = endPred(dayData, target);

    // Based on data generation, expect weekly data to have steepest slope
    // and yearly data to have shallowest slope
    EXPECT_GT(wk.m, yr.m) << "Week data should have steeper slope than year data";
    EXPECT_GT(mo.m, yr.m) << "Month data should have steeper slope than year data";
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
