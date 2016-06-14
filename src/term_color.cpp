#include "term_color.hpp"
#include <math.h>
#include <assert.h>
#define EMPTY_BLOCK   " "
#define ONE_QUARTER   "\xe2\x96\x91"
#define ONE_HALF      "\xe2\x96\x92"
#define THREE_QUARTER "\xe2\x96\x93"
#define FULL_BLOCK    "\xe2\x96\x88"

std::string TermColor::cell_string() {
    auto block = [](blend_mode_t mode) {
        switch (mode) {
        case empty: return EMPTY_BLOCK;
        case one_quarter: return ONE_QUARTER;
        case one_half: return ONE_HALF;
        case three_quarter: return THREE_QUARTER;
        case full: return FULL_BLOCK;
        }
        assert(false);
    };
    std::string out;
    switch (type) {
    case ansi:
        out += "\033[0m\033[3";
        out += std::to_string(std::get<0>(ansi_code));
        if (std::get<1>(ansi_code)) out += ";1";
        out += "m" FULL_BLOCK;
        break;
    case blended_ansi:
        out += "\033[0m\033[3";
        out += std::to_string(std::get<1>(blended_ansi_code));
        out += ";4";
        out += std::to_string(std::get<3>(blended_ansi_code));
        if (std::get<2>(blended_ansi_code)) out += ";1";
        out += "m";
        out += block(std::get<0>(blended_ansi_code));
        break;
    case extended:
        out += "\033[38;5;";
        out += std::to_string(std::get<0>(extended_code));
        out += "m" FULL_BLOCK;
        break;
    case blended_extended:
        out += "\033[38;5;";
        out += std::to_string(std::get<1>(blended_extended_code));
        out += "m\033[48;5;";
        out += std::to_string(std::get<2>(blended_extended_code));
        out += "m";
        out += block(std::get<0>(blended_extended_code));
        break;
    case truecolor:
        out += "\033[38;2;";
        out += std::to_string(r);
        out += ";";
        out += std::to_string(g);
        out += ";";
        out += std::to_string(b);
        out += "m" FULL_BLOCK;
        break;
    }
    return out;
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
