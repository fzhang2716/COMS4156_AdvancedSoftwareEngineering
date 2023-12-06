#define WITHOUT_NUMPY

#include "matplotlibcpp.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace plt = matplotlibcpp;

void plot_hist(const std::vector<float>& data, const std::string& field) {
    float max;
    float min;

    if (data.size() == 0) {
        max = 0;
        min = 0;
    }
    else {
        max = *std::max_element(data.begin(), data.end());
        min = *std::min_element(data.begin(), data.end());
    }

    int numBins = 5;
    float binWidth = (max - min) / numBins;

    std::vector<int> bins(numBins, 0);
    for (unsigned int i = 0; i < data.size(); ++i) {
        int index = (data[i] - min) / binWidth;
        index = std::min(index, numBins-1);
        bins[index] = bins[index] + 1;
    }

    std::vector<std::string> binLabels;
    for (int i = 0; i < numBins; ++i) {
        float left = min + i * binWidth;
        float right = left + binWidth;

        std::ostringstream outLeft;
        outLeft << std::setprecision(1) << std::fixed << std::showpoint << left;
        std::string roundedLeft = outLeft.str();

        std::ostringstream outRight;
        outRight << std::setprecision(1) << std::fixed << std::showpoint << right;
        std::string roundedRight =  outRight.str();

        std::string label = "[" + roundedLeft + "," + roundedRight;
        if (i == numBins-1) {
            label = label + "]";
        }
        else {
            label = label + ")";
        }
        binLabels.push_back(label);
    }

    std::vector<double> x;
    for (int i = 0; i < numBins; ++i) {
        x.push_back(i);
    }

    plt::bar(bins);
    plt::xticks(x, binLabels);

    plt::xlabel("Value");
    plt::ylabel("Frequency");
    plt::title("Histogram of "+field);

    plt::save("figure.pdf");
}

void plot_hist(const std::vector<int>& data, const std::vector<std::string>& categories, const std::string& field) {
    std::vector<double> x;
    for (unsigned int i = 0; i < categories.size(); ++i) {
        x.push_back(i);
    }

    plt::bar(data);
    plt::xticks(x, categories);

    plt::xlabel("Categories");
    plt::ylabel("Frequency");
    plt::title("Histogram of "+field);

    plt::save("figure.pdf");
}

void plot_box(const std::vector<float>& data, const std::string& field) {
    plt::boxplot(data);
    plt::title("Histogram of "+field);
    plt::save("figure.pdf");
}
