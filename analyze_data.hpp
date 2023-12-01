/*
 *   Copyright (c) 2023 Debugteam
 *   All rights reserved.
 */
#ifndef ANALYZE_HPP_
#define ANALYZE_HPP_
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

class Analyze {
public:
    float get_mean(const std::vector<float>& data);
    float get_median(const std::vector<float>& data);
    float get_min(const std::vector<float>& data);
    float get_max(const std::vector<float>& data);
    float get_variance(const std::vector<float>& data);
    float get_sdv(const std::vector<float>& data);
    std::string analyze(const std::vector<float>& data, const std::string& field);
};

#endif  //  ANALYZE_HPP
