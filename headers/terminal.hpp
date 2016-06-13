#ifndef TV_TERMINAL_HPP
#define TV_TERMINAL_HPP
#include "term_color.hpp"
#include <vector>

class Terminal {
public:
    /**
     *  Color palette for non-truecolor terminals.
     */
    std::vector<TermColor> color_palette;
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
     *  Initialize the terminal info (size, font size, color palette).
     */
    Terminal(term_type_t type, term_colors_t colors);

    /**
     *  Print the color palette
     */
    void print_palette(int width, int line_width);
};

#endif
