#ifndef TV_TERM_COLOR_HPP
#define TV_TERM_COLOR_HPP
#include <tuple>

struct TermColor {
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
     *  Constructors.
     */
    TermColor(
        char id, bool bold,
        unsigned char r, unsigned char g, unsigned char b
    ): type(ansi), ansi_code(id, bold),
       r(r), g(g), b(b) {}
    TermColor(
        blend_mode_t mode,
        char fid, bool bold, char bid, 
        unsigned char r, unsigned char g, unsigned char b
    ): type(blended_ansi), blended_ansi_code(mode, fid, bold, bid),
       r(r), g(g), b(b) {}
    TermColor(
        unsigned char id,
        unsigned char r, unsigned char g, unsigned char b
    ): type(extended), extended_code(id),
       r(r), g(g), b(b) {}
    TermColor(
        blend_mode_t mode, unsigned char fid, unsigned char bid,
        unsigned char r, unsigned char g, unsigned char b
    ): type(blended_extended), blended_extended_code(mode, fid, bid),
       r(r), g(g), b(b) {}
    TermColor(
        unsigned char r, unsigned char g, unsigned char b
    ): type(truecolor),
       r(r), g(g), b(b) {}

    /**
     *  Print a cell with this color.
     */
    void print();

    /**
     *  Returns true if this color can be used as a background in a blending.
     */
    bool can_blend();

    /**
     *  Blend with another color of the same type. Return the blended color.
     *  Works only for ANSI and extended colors.
     */
    TermColor blend(blend_mode_t mode, const TermColor& other);
};

#endif
