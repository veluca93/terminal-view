#include "terminal.hpp"
#include <algorithm>
#include <sys/stat.h>
#include <sys/select.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <assert.h>
#include <iostream>
#ifdef __linux__
#include <linux/kd.h>
#endif

std::array<unsigned char, 3> console_colors[] = {
    {0x00, 0x00, 0x00},
    {0xaa, 0x00, 0x00},
    {0x00, 0xaa, 0x00},
    {0xaa, 0x55, 0x00},
    {0x00, 0x00, 0xaa},
    {0xaa, 0x00, 0xaa},
    {0x00, 0xaa, 0xaa},
    {0xaa, 0xaa, 0xaa},
    {0x55, 0x55, 0x55},
    {0xff, 0x55, 0x55},
    {0x55, 0xff, 0x55},
    {0xff, 0xff, 0x55},
    {0x55, 0x55, 0xff},
    {0xff, 0x55, 0xff},
    {0x55, 0xff, 0xff},
    {0xff, 0xff, 0xff}
};

std::array<unsigned char, 3> extended_colors[] = {
    {0x00, 0x00, 0x00},
    {0x00, 0x00, 0x5f},
    {0x00, 0x00, 0x87},
    {0x00, 0x00, 0xaf},
    {0x00, 0x00, 0xd7},
    {0x00, 0x00, 0xff},
    {0x00, 0x5f, 0x00},
    {0x00, 0x5f, 0x5f},
    {0x00, 0x5f, 0x87},
    {0x00, 0x5f, 0xaf},
    {0x00, 0x5f, 0xd7},
    {0x00, 0x5f, 0xff},
    {0x00, 0x87, 0x00},
    {0x00, 0x87, 0x5f},
    {0x00, 0x87, 0x87},
    {0x00, 0x87, 0xaf},
    {0x00, 0x87, 0xd7},
    {0x00, 0x87, 0xff},
    {0x00, 0xaf, 0x00},
    {0x00, 0xaf, 0x5f},
    {0x00, 0xaf, 0x87},
    {0x00, 0xaf, 0xaf},
    {0x00, 0xaf, 0xd7},
    {0x00, 0xaf, 0xff},
    {0x00, 0xd7, 0x00},
    {0x00, 0xd7, 0x5f},
    {0x00, 0xd7, 0x87},
    {0x00, 0xd7, 0xaf},
    {0x00, 0xd7, 0xd7},
    {0x00, 0xd7, 0xff},
    {0x00, 0xff, 0x00},
    {0x00, 0xff, 0x5f},
    {0x00, 0xff, 0x87},
    {0x00, 0xff, 0xaf},
    {0x00, 0xff, 0xd7},
    {0x00, 0xff, 0xff},
    {0x5f, 0x00, 0x00},
    {0x5f, 0x00, 0x5f},
    {0x5f, 0x00, 0x87},
    {0x5f, 0x00, 0xaf},
    {0x5f, 0x00, 0xd7},
    {0x5f, 0x00, 0xff},
    {0x5f, 0x5f, 0x00},
    {0x5f, 0x5f, 0x5f},
    {0x5f, 0x5f, 0x87},
    {0x5f, 0x5f, 0xaf},
    {0x5f, 0x5f, 0xd7},
    {0x5f, 0x5f, 0xff},
    {0x5f, 0x87, 0x00},
    {0x5f, 0x87, 0x5f},
    {0x5f, 0x87, 0x87},
    {0x5f, 0x87, 0xaf},
    {0x5f, 0x87, 0xd7},
    {0x5f, 0x87, 0xff},
    {0x5f, 0xaf, 0x00},
    {0x5f, 0xaf, 0x5f},
    {0x5f, 0xaf, 0x87},
    {0x5f, 0xaf, 0xaf},
    {0x5f, 0xaf, 0xd7},
    {0x5f, 0xaf, 0xff},
    {0x5f, 0xd7, 0x00},
    {0x5f, 0xd7, 0x5f},
    {0x5f, 0xd7, 0x87},
    {0x5f, 0xd7, 0xaf},
    {0x5f, 0xd7, 0xd7},
    {0x5f, 0xd7, 0xff},
    {0x5f, 0xff, 0x00},
    {0x5f, 0xff, 0x5f},
    {0x5f, 0xff, 0x87},
    {0x5f, 0xff, 0xaf},
    {0x5f, 0xff, 0xd7},
    {0x5f, 0xff, 0xff},
    {0x87, 0x00, 0x00},
    {0x87, 0x00, 0x5f},
    {0x87, 0x00, 0x87},
    {0x87, 0x00, 0xaf},
    {0x87, 0x00, 0xd7},
    {0x87, 0x00, 0xff},
    {0x87, 0x5f, 0x00},
    {0x87, 0x5f, 0x5f},
    {0x87, 0x5f, 0x87},
    {0x87, 0x5f, 0xaf},
    {0x87, 0x5f, 0xd7},
    {0x87, 0x5f, 0xff},
    {0x87, 0x87, 0x00},
    {0x87, 0x87, 0x5f},
    {0x87, 0x87, 0x87},
    {0x87, 0x87, 0xaf},
    {0x87, 0x87, 0xd7},
    {0x87, 0x87, 0xff},
    {0x87, 0xaf, 0x00},
    {0x87, 0xaf, 0x5f},
    {0x87, 0xaf, 0x87},
    {0x87, 0xaf, 0xaf},
    {0x87, 0xaf, 0xd7},
    {0x87, 0xaf, 0xff},
    {0x87, 0xd7, 0x00},
    {0x87, 0xd7, 0x5f},
    {0x87, 0xd7, 0x87},
    {0x87, 0xd7, 0xaf},
    {0x87, 0xd7, 0xd7},
    {0x87, 0xd7, 0xff},
    {0x87, 0xff, 0x00},
    {0x87, 0xff, 0x5f},
    {0x87, 0xff, 0x87},
    {0x87, 0xff, 0xaf},
    {0x87, 0xff, 0xd7},
    {0x87, 0xff, 0xff},
    {0xaf, 0x00, 0x00},
    {0xaf, 0x00, 0x5f},
    {0xaf, 0x00, 0x87},
    {0xaf, 0x00, 0xaf},
    {0xaf, 0x00, 0xd7},
    {0xaf, 0x00, 0xff},
    {0xaf, 0x5f, 0x00},
    {0xaf, 0x5f, 0x5f},
    {0xaf, 0x5f, 0x87},
    {0xaf, 0x5f, 0xaf},
    {0xaf, 0x5f, 0xd7},
    {0xaf, 0x5f, 0xff},
    {0xaf, 0x87, 0x00},
    {0xaf, 0x87, 0x5f},
    {0xaf, 0x87, 0x87},
    {0xaf, 0x87, 0xaf},
    {0xaf, 0x87, 0xd7},
    {0xaf, 0x87, 0xff},
    {0xaf, 0xaf, 0x00},
    {0xaf, 0xaf, 0x5f},
    {0xaf, 0xaf, 0x87},
    {0xaf, 0xaf, 0xaf},
    {0xaf, 0xaf, 0xd7},
    {0xaf, 0xaf, 0xff},
    {0xaf, 0xd7, 0x00},
    {0xaf, 0xd7, 0x5f},
    {0xaf, 0xd7, 0x87},
    {0xaf, 0xd7, 0xaf},
    {0xaf, 0xd7, 0xd7},
    {0xaf, 0xd7, 0xff},
    {0xaf, 0xff, 0x00},
    {0xaf, 0xff, 0x5f},
    {0xaf, 0xff, 0x87},
    {0xaf, 0xff, 0xaf},
    {0xaf, 0xff, 0xd7},
    {0xaf, 0xff, 0xff},
    {0xd7, 0x00, 0x00},
    {0xd7, 0x00, 0x5f},
    {0xd7, 0x00, 0x87},
    {0xd7, 0x00, 0xaf},
    {0xd7, 0x00, 0xd7},
    {0xd7, 0x00, 0xff},
    {0xd7, 0x5f, 0x00},
    {0xd7, 0x5f, 0x5f},
    {0xd7, 0x5f, 0x87},
    {0xd7, 0x5f, 0xaf},
    {0xd7, 0x5f, 0xd7},
    {0xd7, 0x5f, 0xff},
    {0xd7, 0x87, 0x00},
    {0xd7, 0x87, 0x5f},
    {0xd7, 0x87, 0x87},
    {0xd7, 0x87, 0xaf},
    {0xd7, 0x87, 0xd7},
    {0xd7, 0x87, 0xff},
    {0xd7, 0xaf, 0x00},
    {0xd7, 0xaf, 0x5f},
    {0xd7, 0xaf, 0x87},
    {0xd7, 0xaf, 0xaf},
    {0xd7, 0xaf, 0xd7},
    {0xd7, 0xaf, 0xff},
    {0xd7, 0xd7, 0x00},
    {0xd7, 0xd7, 0x5f},
    {0xd7, 0xd7, 0x87},
    {0xd7, 0xd7, 0xaf},
    {0xd7, 0xd7, 0xd7},
    {0xd7, 0xd7, 0xff},
    {0xd7, 0xff, 0x00},
    {0xd7, 0xff, 0x5f},
    {0xd7, 0xff, 0x87},
    {0xd7, 0xff, 0xaf},
    {0xd7, 0xff, 0xd7},
    {0xd7, 0xff, 0xff},
    {0xff, 0x00, 0x00},
    {0xff, 0x00, 0x5f},
    {0xff, 0x00, 0x87},
    {0xff, 0x00, 0xaf},
    {0xff, 0x00, 0xd7},
    {0xff, 0x00, 0xff},
    {0xff, 0x5f, 0x00},
    {0xff, 0x5f, 0x5f},
    {0xff, 0x5f, 0x87},
    {0xff, 0x5f, 0xaf},
    {0xff, 0x5f, 0xd7},
    {0xff, 0x5f, 0xff},
    {0xff, 0x87, 0x00},
    {0xff, 0x87, 0x5f},
    {0xff, 0x87, 0x87},
    {0xff, 0x87, 0xaf},
    {0xff, 0x87, 0xd7},
    {0xff, 0x87, 0xff},
    {0xff, 0xaf, 0x00},
    {0xff, 0xaf, 0x5f},
    {0xff, 0xaf, 0x87},
    {0xff, 0xaf, 0xaf},
    {0xff, 0xaf, 0xd7},
    {0xff, 0xaf, 0xff},
    {0xff, 0xd7, 0x00},
    {0xff, 0xd7, 0x5f},
    {0xff, 0xd7, 0x87},
    {0xff, 0xd7, 0xaf},
    {0xff, 0xd7, 0xd7},
    {0xff, 0xd7, 0xff},
    {0xff, 0xff, 0x00},
    {0xff, 0xff, 0x5f},
    {0xff, 0xff, 0x87},
    {0xff, 0xff, 0xaf},
    {0xff, 0xff, 0xd7},
    {0xff, 0xff, 0xff},
    {0x08, 0x08, 0x08},
    {0x12, 0x12, 0x12},
    {0x1c, 0x1c, 0x1c},
    {0x26, 0x26, 0x26},
    {0x30, 0x30, 0x30},
    {0x3a, 0x3a, 0x3a},
    {0x44, 0x44, 0x44},
    {0x4e, 0x4e, 0x4e},
    {0x58, 0x58, 0x58},
    {0x62, 0x62, 0x62},
    {0x6c, 0x6c, 0x6c},
    {0x76, 0x76, 0x76},
    {0x80, 0x80, 0x80},
    {0x8a, 0x8a, 0x8a},
    {0x94, 0x94, 0x94},
    {0x9e, 0x9e, 0x9e},
    {0xa8, 0xa8, 0xa8},
    {0xb2, 0xb2, 0xb2},
    {0xbc, 0xbc, 0xbc},
    {0xc6, 0xc6, 0xc6},
    {0xd0, 0xd0, 0xd0},
    {0xda, 0xda, 0xda},
    {0xe4, 0xe4, 0xe4},
    {0xee, 0xee, 0xee}
};

std::vector<std::array<unsigned char, 3>> Terminal::get_colors_rgb(int start, int end) {
    std::vector<std::array<unsigned char, 3>> res;
    if (start >= end) return res;
    if (type == console) {
        for (int i=start; i<16 && i<end; i++)
            res.push_back(console_colors[i]);
        // The console does not support extended colors, but if asked we
        // output them anyway.
        for (int i=16; i<end; i++)
            res.push_back(extended_colors[i-16]);
        return res;
    }
    if (end > 16) {
        res = get_colors_rgb(start, 16);
        for (int i=16; i<end; i++)
            res.push_back(extended_colors[i-16]);
        return res;
    }
    auto read_color = [](FILE* tty, int id) {
        unsigned char r, g, b;
        int read_id;
        char r_buf[10], g_buf[10], b_buf[10];
        int ret = fscanf(tty, "\033]4;%d;rgb:%4s/%4s/%4s\007", &read_id, r_buf, g_buf, b_buf);
        if (ret != 4 || read_id != id)
            throw std::logic_error("Could not read terminal color info!");
        r_buf[2] = g_buf[2] = b_buf[2] = 0;
        r = std::stoi(r_buf, 0, 16);
        g = std::stoi(g_buf, 0, 16);
        b = std::stoi(b_buf, 0, 16);
        return std::array<unsigned char, 3>{r, g, b};
    };
    FILE* tty = fopen("/dev/tty", "r+");
    if (tty == NULL)
        throw std::runtime_error("This process has no controlling terminal!\n");
    int fd = fileno(tty);
    fd_set readset;
    struct timeval time;
    struct termios term, initial_term;
    tcgetattr(fd, &initial_term);
    term = initial_term;
    term.c_lflag &=~ICANON;
    term.c_lflag &=~ECHO;
    tcsetattr(fd, TCSANOW, &term);
    for (int i=start; i<end; i++) {
        fprintf(tty, "\033]4;%d;?\007", i);
    }
    fflush(tty);
    FD_ZERO(&readset);
    FD_SET(fd, &readset);
    time.tv_sec = 0;
    time.tv_usec = 1000000;
    if (select(fd + 1, &readset, NULL, NULL, &time) == 1) {
        for (int i=start; i<end; i++) {
            res.emplace_back(read_color(tty, i));
        }
    } else {
        throw std::logic_error("Could not read terminal color info!");
    }
    tcsetattr(fd, TCSADRAIN, &initial_term);
    return res;
}

Terminal::Terminal(term_type_t type, term_colors_t colors, dist_algo_t algo): algo(algo), type(type), colors(colors) {
    FILE* tty = fopen("/dev/tty", "r+");
    if (tty == NULL)
        throw std::runtime_error("This process has no controlling terminal!\n");

    // Detect terminal width and height
    struct winsize ts;
    if (ioctl(fileno(tty), TIOCGWINSZ, &ts) == -1)
        throw std::runtime_error("Could not get terminal size!\n");
    width = ts.ws_col;
    height = ts.ws_row;

    // Detect character size
    switch (type) {
    case console: {
        cwidth = 8;
#ifdef __linux__
        char empty[512*32*8];
        struct consolefontdesc font;
        font.charcount = sizeof(empty)/(32*8);
        font.chardata = empty;
        if (ioctl(fileno(tty), GIO_FONTX, &font) == -1)
            throw std::runtime_error("Cannot get font size!\n");
        cheight = font.charheight;
#else
        cheight = 16;
#endif
        break;
    }
    case xterm: {
        int w, h;
        fd_set readset;
        struct timeval time;
        struct termios term, initial_term;
        tcgetattr(fileno(tty), &initial_term);
        term = initial_term;
        term.c_lflag &=~ICANON;
        term.c_lflag &=~ECHO;
        tcsetattr(fileno(tty), TCSANOW, &term);
        fprintf(tty, "\033[14t");
        fflush(tty);
        FD_ZERO(&readset);
        FD_SET(fileno(tty), &readset);
        time.tv_sec = 0;
        time.tv_usec = 1000000;
        if (select(fileno(tty) + 1, &readset, NULL, NULL, &time) == 1) {
            if (fscanf(tty, "\033[4;%d;%dj", &h, &w) != 2)
                throw std::runtime_error("Cannot get font size!\n");
        } else {
            throw std::runtime_error("Cannot get font size!\n");
        }
        tcsetattr(fileno(tty), TCSADRAIN, &initial_term);
        cwidth = w / width;
        cheight = h / height;
        break;
    }};

    // Populate the palette, if needed.
    std::vector<TermColor> temp_palette;
    switch (colors) {
    case ansi: {
        auto cols = get_colors_rgb(0, 16);
        for (int i=0; i<8; i++) {
            temp_palette.emplace_back(i, false, cols[i][0], cols[i][1], cols[i][2]);
            temp_palette.emplace_back(i, true, cols[i+8][0], cols[i+8][1], cols[i+8][2]);
        }
        break;
    }
    case extended: {
        auto cols = get_colors_rgb(0, 256);
        for (int i=0; i<256; i++) {
            temp_palette.emplace_back(i, cols[i][0], cols[i][1], cols[i][2]);
        }
        break;
    }
    case truecolor:
        fclose(tty);
        return;
    };
    fclose(tty);
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
    std::sort(color_palette.begin(), color_palette.end());
    auto ptr = std::unique(color_palette.begin(), color_palette.end());
    unsigned count = ptr - color_palette.begin();
    while (count > color_palette.size()) color_palette.pop_back();
    approx_cache.resize(256*256*256, -1);
    switch (colors) {
    case truecolor: assert(false);
    case ansi: bucket_width = 64; break;
    case extended: bucket_width = 16; break;
    }
    int bucket_count = 256/bucket_width;
    buckets.resize(bucket_count*bucket_count*bucket_count);
    for (unsigned i=0; i<color_palette.size(); i++) {
        int ar = color_palette[i].r / bucket_width;
        int ag = color_palette[i].g / bucket_width;
        int ab = color_palette[i].b / bucket_width;
        buckets[(ar * bucket_count * bucket_count) | (ag * bucket_count) | ab].push_back(i);
    }
}

std::string Terminal::show_palette(int width, int line_width) {
    std::string out;
    width = std::max(width, 1);
    int cpl = std::max(1, line_width/width);
    int i = 0;
    for (const auto& bucket: buckets) {
        for (auto cur: bucket) {
            if (i % cpl == 0 && i > 0) out += "\033[0m\n";
            for (int j=0; j<width; j++)
                out += color_palette[cur].cell_string();
            i++;
        }
    }
    out += "\033[0m\n";
    return out;
}

TermColor Terminal::approximate(unsigned char r, unsigned char g, unsigned char b) {
    if (colors == truecolor) return TermColor(r, g, b);
    if (approx_cache[(r<<16) | (g<<8) | b] != -1)
        return color_palette[approx_cache[(r<<16) | (g<<8) | b]];
    std::vector<int> candidates;


    int bucket_count = 256/bucket_width;
    int ar = r / bucket_width;
    int ag = g / bucket_width;
    int ab = b / bucket_width;
    auto add_bkt = [&](int ar, int ag, int ab) {
        if (ar < 0 || ag < 0 || ab < 0) return;
        if (ar >= bucket_count || ag >= bucket_count || ab >= bucket_count) return;
        for (auto x: buckets[(ar * bucket_count * bucket_count) | (ag * bucket_count) | ab])
        candidates.emplace_back(x);
    };
    add_bkt(ar, ag, ab);
    for (int i=1; i<5; i++) {
        if (candidates.size() >= 256 && candidates.size() >= color_palette.size()) break;
        for (int x=0; x<i; x++) {
            for (int y=0; x+y<i; y++) {
                int z = i-x-y;
                add_bkt(ar+x, ag+y, ab+z);
                add_bkt(ar+x, ag+y, ab-z);
                add_bkt(ar+x, ag-y, ab+z);
                add_bkt(ar+x, ag-y, ab-z);
                add_bkt(ar-x, ag+y, ab+z);
                add_bkt(ar-x, ag+y, ab-z);
                add_bkt(ar-x, ag-y, ab+z);
                add_bkt(ar-x, ag-y, ab-z);
            }
        }
    }

    int best = -1;
    double dist = std::numeric_limits<double>::max();
    for (auto i: candidates) {
        double cdist = color_distance(r, g, b, color_palette[i].r, color_palette[i].g, color_palette[i].b, algo);
        if (cdist < dist) {
            best = i;
            dist = cdist;
        }
    }
    approx_cache[(r<<16) | (g<<8) | b] = best;
    return color_palette[best];
}

std::string Terminal::move_to(int x, int y) {
    std::string ret = "\033[";
    ret += std::to_string(y);
    ret += ";";
    ret += std::to_string(x);
    ret += "H";
    return ret;
}

std::string Terminal::clear() {
    return "\033[2J\033[1;1H";
}

std::string Terminal::clear_color() {
    return "\033[0m";
}
