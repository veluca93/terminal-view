#ifndef TV_TERMINAL_HPP
#define TV_TERMINAL_HPP
#include "term_color.hpp"
#include "color_distance.hpp"
#include <vector>

class Terminal {
    /**
     *  Ask the terminal the RGB values of a range of colors.
     *  On xterms, use the \033]4;<color_id>;?\007 escape sequence for the
     *  first sixteen colors. The next are fixed, so they are hardcoded.
     *  On console, all colors are fixed, so they are hardcoded.
     */
    std::vector<std::array<unsigned char, 3>> get_colors_rgb(int start, int end);

    /**
     *  Color palette for non-truecolor terminals.
     */
    std::vector<TermColor> color_palette;

    /**
     *  Array of cached approximations of colors with the palette.
     */
    std::vector<int> approx_cache;

    /**
     *  Approximation algorithm used for rgb -> palette conversion.
     */
    dist_algo_t algo;
public:
    /**
     *  Width and height of the terminal.
     */
    int width, height;

    /**
     *  Width and height of a terminal character, in pixels.
     */
    int cwidth, cheight;

    /**
     *  Terminal type. console corresponds to the Linux console, while xterm
     *  corresponds to all terminals that emulate xterm in X11.
     */
    enum term_type_t {console, xterm};
    term_type_t type;

    /**
     *  Terminal color support. ANSI is for ANSI colors, extended for 256
     *  colors, and truecolor for 24-bit colors.
     */
    enum term_colors_t {ansi, extended, truecolor};
    term_colors_t colors;

    /**
     *  Initialize the terminal info (size, font size, color palette,
     *  approximation algorithm).
     */
    Terminal(term_type_t type, term_colors_t colors, dist_algo_t algo = ycgco);

    /**
     *  Print the color palette
     */
    std::string show_palette(int width, int line_width);

    /**
     *  Returns an approximation of the given color with the palette,
     *  or the color itself if the terminal is truecolor.
     */
    TermColor approximate(unsigned char r, unsigned char g, unsigned char b);

    /**
     *  Returns a string that moves the terminal cursor to a given position.
     */
    std::string move_to(int x, int y);

    /**
     *  Returns a string that clears the terminal screen.
     */
    std::string clear();
};

#endif
