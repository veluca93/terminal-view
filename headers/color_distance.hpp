#ifndef TV_COLOR_DISTANCE_HPP
#define TV_COLOR_DISTANCE_HPP

enum dist_algo_t {
    ycgco
};

double color_distance(
    unsigned char r1, unsigned char g1, unsigned char b1,
    unsigned char r2, unsigned char g2, unsigned char b2,
    dist_algo_t algo
);

#endif
