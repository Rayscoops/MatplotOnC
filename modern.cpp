#include "matplotlibcpp.h"
namespace plt = matplotlibcpp;

using namespace plt;

int main() {
    plot({1,3,2,4});
    show();
}
