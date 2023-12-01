#define CATCH_CONFIG_MAIN

#include <string>
#include <./catch2/catch.hpp>
#include "./analyze_data.hpp"
#include <cmath>

bool areEqual(float a, float b, double tolerance = 1e-9) {
    return std::abs(a - b) < tolerance;
}

std::vector<float> empty;
std::vector<float> basic = {1, 2, 3, 4, 5};

Analyze analyzer;

TEST_CASE("Get Mean", "[single-file]") {
    float mean1 = analyzer.get_mean(empty);
    REQUIRE(areEqual(mean1, 0));

    float mean2 = analyzer.get_mean(basic);
    REQUIRE(areEqual(mean2, 3));
}

TEST_CASE("Get Median", "[single-file]") {
    float median1 = analyzer.get_median(empty);
    REQUIRE(areEqual(median1, 0));

    float median2 = analyzer.get_median(basic);
    REQUIRE(areEqual(median2, 3));
}

TEST_CASE("Get Min", "[single-file]") {
    float min1 = analyzer.get_min(empty);
    REQUIRE(areEqual(min1, 0));

    float min2 = analyzer.get_min(basic);
    REQUIRE(areEqual(min2, 1));
}

TEST_CASE("Get Max", "[single-file]") {
    float max1 = analyzer.get_max(empty);
    REQUIRE(areEqual(max1, 0));

    float max2 = analyzer.get_max(basic);
    REQUIRE(areEqual(max2, 5));
}

TEST_CASE("Get Variance", "[single-file]") {
    float var1 = analyzer.get_variance(empty);
    REQUIRE(areEqual(var1, 0));

    float var2 = analyzer.get_variance(basic);
    REQUIRE(areEqual(var2, 2));
}

TEST_CASE("Get Standard Deviation", "[single-file]") {
    float sdv1 = analyzer.get_sdv(empty);
    REQUIRE(areEqual(sdv1, 0));

    float sdv2 = analyzer.get_sdv(basic);
    REQUIRE(areEqual(sdv2, pow(2, 0.5)));
}
