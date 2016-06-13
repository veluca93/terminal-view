#include "term_color.hpp"
#include <math.h>
#define EMPTY_BLOCK   " "
#define ONE_QUARTER   "\xe2\x96\x91"
#define ONE_HALF      "\xe2\x96\x92"
#define THREE_QUARTER "\xe2\x96\x93"
#define FULL_BLOCK    "\xe2\x96\x88"

void TermColor::print() {
    switch (type) {
    case ansi:
        printf("\033[0m\033[3%d", std::get<0>(ansi_code));
        if (std::get<1>(ansi_code)) printf(";1");
        printf("m" FULL_BLOCK);
        break;
    case blended_ansi:
        printf("\033[0m\033[3%d", std::get<1>(blended_ansi_code));
        printf(";4%d", std::get<3>(blended_ansi_code));
        if (std::get<2>(blended_ansi_code)) printf(";1");
        printf("m");
        switch (std::get<0>(blended_ansi_code)) {
        case empty: printf(EMPTY_BLOCK); break;
        case one_quarter: printf(ONE_QUARTER); break;
        case one_half: printf(ONE_HALF); break;
        case three_quarter: printf(THREE_QUARTER); break;
        case full: printf(FULL_BLOCK); break;
        }
        break;
    case extended:
        printf("\033[38;5;%dm" FULL_BLOCK, std::get<0>(extended_code));
        break;
    case blended_extended:
        printf("\033[38;5;%dm", std::get<1>(blended_extended_code));
        printf("\033[48;5;%dm", std::get<2>(blended_extended_code));
        switch (std::get<0>(blended_extended_code)) {
        case empty: printf(EMPTY_BLOCK); break;
        case one_quarter: printf(ONE_QUARTER); break;
        case one_half: printf(ONE_HALF); break;
        case three_quarter: printf(THREE_QUARTER); break;
        case full: printf(FULL_BLOCK); break;
        }
        break;
    case truecolor:
        printf("\03338;2;%d;%d;%dm" FULL_BLOCK, r, g, b);
        break;
    }
}

TermColor TermColor::blend(blend_mode_t mode, const TermColor& other) {
    if (other.type != type)
        throw std::logic_error("You tried to blend colors of different type!");
    int blend_quarters = 0;
    switch (mode) {
    case empty: blend_quarters = 0; break;
    case one_quarter: blend_quarters = 1; break;
    case one_half: blend_quarters = 2; break;
    case three_quarter: blend_quarters = 3; break;
    case full: blend_quarters = 4; break;
    }
    int new_r = sqrt(blend_quarters*other.r*other.r/4 + (4-blend_quarters)*r*r/4);
    int new_g = sqrt(blend_quarters*other.g*other.g/4 + (4-blend_quarters)*g*g/4);
    int new_b = sqrt(blend_quarters*other.b*other.b/4 + (4-blend_quarters)*b*b/4);
    switch (type) {
    case ansi:
        if (std::get<1>(ansi_code)) throw std::logic_error("You can't use a bold color as background!");
        return TermColor(mode, std::get<0>(other.ansi_code), std::get<1>(other.ansi_code), std::get<0>(ansi_code), new_r, new_g, new_b);
    case extended:
        return TermColor(mode, std::get<0>(other.extended_code), std::get<0>(extended_code), new_r, new_g, new_b);
    default: throw std::logic_error("Invalid color type for blending!");
    }
}

bool TermColor::can_blend() {
    if (type == extended) return true;
    if (type == ansi) return !std::get<1>(ansi_code);
    return false;
}
