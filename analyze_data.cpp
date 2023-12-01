#include <string>
#include <vector>
#include <algorithm>
#include "./analyze_data.hpp"
#include <cmath>

float Analyze::get_mean(const std::vector<float>& data) {
    if (data.size() == 0) {
        return 0;
    }
    float mean = 0;
    for (unsigned int i = 0; i < data.size(); ++i) {
        mean = mean + data[i];
    }
    mean = mean / data.size();
    return mean;
}

float Analyze::get_median(const std::vector<float>& data) {
    if (data.size() == 0) {
        return 0;
    }
    return data[data.size()/2];
}

float Analyze::get_min(const std::vector<float>& data) {
    if (data.size() == 0) {
        return 0;
    }
    return data[0];
}

float Analyze::get_max(const std::vector<float>& data) {
    if (data.size() == 0) {
        return 0;
    }
    return data[data.size()-1];
}

float Analyze::get_variance(const std::vector<float>& data) {
    if (data.size() == 0) {
        return 0;
    }
    float mean = get_mean(data);
    float variance = 0;
    for (unsigned int i = 0; i < data.size(); ++i) {
        variance = variance + pow(data[i]-mean, 2);
    }
    variance = variance / data.size();
    return variance;
}

float Analyze::get_sdv(const std::vector<float>& data) {
    float variance = Analyze::get_variance(data);
    float sdv = pow(variance, 0.5);
    return sdv;
}

std::string Analyze::analyze(const std::vector<float>& data, const std::string& field) {
    std::vector<float> sortedData = data;
    std::sort(sortedData.begin(), sortedData.end());

    float mean = Analyze::get_mean(sortedData);
    float median = Analyze::get_median(sortedData);
    float min = Analyze::get_min(sortedData);
    float max = Analyze::get_max(sortedData);
    float variance = Analyze::get_variance(sortedData);
    float sdv = Analyze::get_sdv(sortedData);

    std::string result = "";
    result = result + "Analysis of " + field + "\\n";
    result = result + "Mean: " + std::to_string(mean) + "\\n";
    result = result + "Median: " + std::to_string(median) + "\\n";
    result = result + "Min: " + std::to_string(min) + "\\n";
    result = result + "Max: " + std::to_string(max) + "\\n";
    result = result + "Variance: " + std::to_string(variance) + "\\n";
    result = result + "Standard Deviation: " + std::to_string(sdv) + "\\n";

    return result;
}
