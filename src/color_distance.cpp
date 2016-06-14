#include "color_distance.hpp"
#include <assert.h>
#include <tuple>

static std::tuple<int, int, int> to_ycgco(int r, int g, int b) {
    int  y =   r + 2*g + 2*b;
    int cg =  -r + 2*g -   b;
    int co = 2*r       - 2*b;
    return {y, cg, co};
}

static double ycgco_distance(int r1, int g1, int b1, int r2, int g2, int b2) {
    auto first = to_ycgco(r1, g1, b1);
    auto second = to_ycgco(r2, g2, b2);
    double y_diff = std::get<0>(first) - std::get<0>(second);
    double cg_diff = std::get<1>(first) - std::get<1>(second);
    double co_diff = std::get<2>(first) - std::get<2>(second);
    double dist = std::get<2>(second)*std::get<2>(second) + std::get<1>(second)*std::get<1>(second);
    double mult = 1;
    if (dist < 100) mult = 4;
    return y_diff*y_diff + mult*(cg_diff*cg_diff + co_diff*co_diff);
}

double color_distance(
    unsigned char r1, unsigned char g1, unsigned char b1,
    unsigned char r2, unsigned char g2, unsigned char b2,
    dist_algo_t algo
) {
    switch (algo) {
    case ycgco:
        return ycgco_distance(r1, g1, b1, r2, g2, b2);
    }
    assert(false);
}
