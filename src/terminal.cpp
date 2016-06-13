#include "terminal.hpp"

Terminal::Terminal(term_type_t type, term_colors_t colors): type(type), colors(colors) {
    std::vector<TermColor> temp_palette;
    switch (colors) {
    case ansi:
        for (int i=0; i<8; i++) {
            temp_palette.emplace_back(i, false, 0, 0, 0);
            temp_palette.emplace_back(i, true, 0, 0, 0);
        }
        break;
    case extended:
        for (int i=0; i<256; i++) {
            temp_palette.emplace_back(i, 0, 0, 0);
        }
        break;
    case truecolor:
        break;
    };
    for (unsigned i=0; i<temp_palette.size(); i++) {
        color_palette.push_back(temp_palette[i]);
        for (unsigned j=i+1; j<temp_palette.size(); j++) {
            if (temp_palette[i].can_blend()) {
                color_palette.push_back(temp_palette[i].blend(TermColor::one_quarter, temp_palette[j]));
                color_palette.push_back(temp_palette[i].blend(TermColor::one_half, temp_palette[j]));
                color_palette.push_back(temp_palette[i].blend(TermColor::three_quarter, temp_palette[j]));
            }
            else if (temp_palette[j].can_blend()) {
                color_palette.push_back(temp_palette[j].blend(TermColor::one_quarter, temp_palette[i]));
                color_palette.push_back(temp_palette[j].blend(TermColor::one_half, temp_palette[i]));
                color_palette.push_back(temp_palette[j].blend(TermColor::three_quarter, temp_palette[i]));
           }
        }
    }
}

void Terminal::print_palette(int width, int line_width) {
    width = std::max(width, 1);
    int cpl = std::max(1, line_width/width);
    for (unsigned i=0; i<color_palette.size(); i++) {
        if (i % cpl == 0) puts("\033[0m");
        for (int j=0; j<width; j++)
            color_palette[i].print();
    }
    puts("\033[0m");
}
