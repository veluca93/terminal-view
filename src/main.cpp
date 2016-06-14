#include "image.hpp"
#include "terminal.hpp"
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <unistd.h>

Terminal::term_type_t detect_term_type() {
    char* TERM = getenv("TERM");
    if (TERM == NULL) {
        fprintf(stderr, "Unknown terminal type - TERM is not set!\n");
        exit(1);
    }
    bool is_xterm_256 = strcmp(TERM, "xterm-256color") == 0;
    bool is_xterm = is_xterm_256 || strcmp(TERM, "xterm") == 0;
    if (!is_xterm && strcmp(TERM, "linux") != 0) {
        fprintf(stderr, "Unknown terminal type %s!\n", TERM);
        exit(1);
    }
    return is_xterm ? Terminal::xterm : Terminal::console;
}

Terminal::term_colors_t detect_term_colors() {
    char* TERM = getenv("TERM");
    char* COLORTERM = getenv("COLORTERM");
    if (TERM == NULL) {
        fprintf(stderr, "Unknown terminal type - TERM is not set!\n");
        exit(1);
    }
    bool is_xterm_256 = strcmp(TERM, "xterm-256color") == 0;
    bool is_xterm = is_xterm_256 || strcmp(TERM, "xterm") == 0;
    bool is_truecolor = COLORTERM ? (strcmp(COLORTERM, "truecolor") == 0) : false;
    if (is_xterm) {
        if (is_truecolor) return Terminal::truecolor;
        return is_xterm_256 ? Terminal::extended : Terminal::ansi;
    }
    if (strcmp(TERM, "linux") != 0) {
        fprintf(stderr, "Unknown terminal type %s!\n", TERM);
        exit(1);
    }
    return Terminal::ansi;
}

int main(int argc, char** argv) {
    std::vector<char*> other_args;
    Terminal::term_type_t type = Terminal::xterm;
    Terminal::term_colors_t colors = Terminal::ansi;
    bool found_term_type = false;
    bool found_term_colors = false;
    for (int i=1; i<argc; i++) {
        if (strcmp(argv[i], "--console") == 0) {
            found_term_type = true;
            type = Terminal::console;
        } else if (strcmp(argv[i], "--xterm") == 0) {
            found_term_type = true;
            type = Terminal::xterm;
        } else if (strcmp(argv[i], "--ansi") == 0) {
            found_term_colors = true;
            colors = Terminal::ansi;
        } else if (strcmp(argv[i], "--extended") == 0) {
            found_term_colors = true;
            colors = Terminal::extended;
        } else if (strcmp(argv[i], "--truecolor") == 0) {
            found_term_colors = true;
            colors = Terminal::truecolor;
        } else if (argv[i][0] == '-') {
            fprintf(stderr, "Unknown option %s!\n", argv[i]);
            return 1;
        } else {
            other_args.emplace_back(argv[i]);
        }
    }
    if (other_args.size() < 1) {
        fprintf(stderr, "You need to specify at least an image to show!\n");
        return 1;
    }
    Image img{other_args[0]};
    if (!found_term_type) type = detect_term_type();
    if (!found_term_colors) colors = detect_term_colors();
    Terminal term(type, colors);

    // Image preparation
    img.downscale(term.width, term.height, term.cwidth, term.cheight);
    int start_row = (term.height-img.height)/2;
    int start_col = (term.width-img.width)/2;
    if (start_row < 0) start_row = 0;
    if (start_col < 0) start_col = 0;

    std::cout << term.clear();
    
    std::string out;
    for (unsigned y=0; y<img.height; y++) {
        out += term.move_to(start_col, y+start_row);
        for (unsigned x=0; x<img.width; x++)
            out += term.approximate(img.r(x, y), img.g(x, y), img.b(x, y)).cell_string();
        out += term.clear_color();
    }
    out += term.move_to(1, 1000);

    std::cout << out << std::flush;
    sleep(1);
}
