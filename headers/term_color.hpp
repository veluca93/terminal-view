#ifndef TV_TERM_COLOR_HPP
#define TV_TERM_COLOR_HPP
#include <tuple>

class TermColor {
private:
    /**
     *  Compute the hsv values from the rgb ones.
     */
    void compute_hsv();
public:
    /**
     *  Color type.
     *
     *  ansi: standard eight ANSI colors. May be bold.
     *  blended_ansi: the result of blending two ANSI colors.
     *  extended: ANSI colors for 256-color terminals.
     *  blended_extended: the result of blending two extended colors.
     *  truecolor: for terminals that support 24-bit colors.
     */
    enum color_t {ansi, blended_ansi, extended, blended_extended, truecolor};
    color_t type;
    
    /**
     *  Color blending mode for blended colors.
     */
    enum blend_mode_t {empty, one_quarter, one_half, three_quarter, full};

    /**
     *  Type-specific parameters. Blended colors have an extra parameter that
     *  specifies the blending mode.
     *
     *  ansi_code: identifier of the ANSI color, plus a flag for bold.
     *  blended_ansi_code: foreground + background ANSI color. Note that only
     *                     the foreground color may be bold.
     *  extended_code: identifier of extended ANSI color.
     *  blended_extended: foreground + background ANSI color.
     *  truecolor: no extra information required.
     */
    union {
        std::tuple<char, bool> ansi_code;
        std::tuple<blend_mode_t, char, bool, char> blended_ansi_code;
        std::tuple<unsigned char> extended_code;
        std::tuple<blend_mode_t, unsigned char, unsigned char> blended_extended_code; 
    };

    /**
     *  RGB color values for the color.
     *
     *  They range from 0 to 255.
     */
    unsigned char r, g, b;

    /**
     *  HSV values for the color.
     *
     *  They range from 0 to 1.
     */
    double h, s, v;

    /**
     *  Constructors.
     */
    TermColor(
        char id, bool bold,
        unsigned char r, unsigned char g, unsigned char b
    ): type(ansi), ansi_code(id, bold),
       r(r), g(g), b(b) {compute_hsv();}
    TermColor(
        blend_mode_t mode,
        char fid, bool bold, char bid, 
        unsigned char r, unsigned char g, unsigned char b
    ): type(blended_ansi), blended_ansi_code(mode, fid, bold, bid),
       r(r), g(g), b(b) {compute_hsv();}
    TermColor(
        unsigned char id,
        unsigned char r, unsigned char g, unsigned char b
    ): type(extended), extended_code(id),
       r(r), g(g), b(b) {compute_hsv();}
    TermColor(
        blend_mode_t mode, unsigned char fid, unsigned char bid,
        unsigned char r, unsigned char g, unsigned char b
    ): type(blended_extended), blended_extended_code(mode, fid, bid),
       r(r), g(g), b(b) {compute_hsv();}
    TermColor(
        unsigned char r, unsigned char g, unsigned char b
    ): type(truecolor),
       r(r), g(g), b(b) {compute_hsv();}

    /**
     *  String that prints a cell with this color.
     */
    std::string cell_string();

    /**
     *  Returns true if this color can be used as a background in a blending.
     */
    bool can_blend();

    /**
     *  Blend with another color of the same type. Return the blended color.
     *  Works only for ANSI and extended colors.
     */
    TermColor blend(blend_mode_t mode, const TermColor& other);

    /**
     *  Assignment operator
     */
    TermColor& operator=(const TermColor& other) {
        type = other.type;
        r = other.r;
        g = other.g;
        b = other.b;
        h = other.h;
        s = other.s;
        v = other.v;
        switch (type) {
        case ansi: ansi_code = other.ansi_code; break;
        case blended_ansi: blended_ansi_code = other.blended_ansi_code; break;
        case extended: extended_code = other.extended_code; break;
        case blended_extended: blended_extended_code = other.blended_extended_code; break;
        default: break;
        }
        return *this;
    }

    /**
     *  Comparison operators
     */
    bool operator<(const TermColor& other) const {
        if (s > other.s) return true;
        if (s < other.s) return false;
        if (v > other.v) return true;
        if (v < other.v) return false;
        return h < other.h;
    }
    bool operator==(const TermColor& other) const {
        return !((*this < other) || (other < *this));
    }
    bool operator!=(const TermColor& other) const {
        return !(*this == other);
    }
};

#endif
