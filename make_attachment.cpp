#define WITHOUT_NUMPY

#include "matplotlibcpp.h"

namespace plt = matplotlibcpp;

void plot_minimal() {
    plt::plot({1,3,2,4});
    plt::save("minimal.pdf");
}
