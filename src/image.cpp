#include "image.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include <stdexcept>

static void sdl_init() __attribute__((constructor));
static void sdl_init() {
    SDL_Init(0);
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG | IMG_INIT_TIF);
}

static void sdl_quit() __attribute__((destructor));
static void sdl_quit() {
    IMG_Quit();
    SDL_Quit();
}

Image::Image(const std::string& file) {
    auto tmp_i = IMG_Load(file.c_str());
    if (tmp_i == NULL)
        throw std::runtime_error("Error loading " + file);
    auto temp = SDL_ConvertSurfaceFormat(tmp_i, SDL_PIXELFORMAT_ABGR8888, 0);
    width = temp->w;
    height = temp->h;
    for (unsigned y=0; y<height; y++) {
        for (unsigned x=0; x<width; x++) {
            img.push_back(((unsigned char*)temp->pixels)[temp->pitch*y+4*x]);
            img.push_back(((unsigned char*)temp->pixels)[temp->pitch*y+4*x+1]);
            img.push_back(((unsigned char*)temp->pixels)[temp->pitch*y+4*x+2]);
        }
    }
}

void Image::downscale(size_t w, size_t h, size_t pixel_width, size_t pixel_height) {
    double ppc_row = std::max(width/double(pixel_width*w), height/double(pixel_height*height))*pixel_height;
    double ppc_column = ppc_row*pixel_width/pixel_height;
    if (ppc_row < 1 || ppc_column < 1) return;
    std::vector<char> img_data;
    unsigned x = 0;
    unsigned y = 0;
    for (y=0; y<h; y++) {
        if (ceil(y*ppc_row) > height) break;
        for (x=0; x<w; x++) {
            int red_sum = 0;
            int green_sum = 0;
            int blue_sum = 0;
            int count = 0;
            if (ceil(x*ppc_column) > width) break;
            for (unsigned i=ceil(x*ppc_column); i<width && i<(x+1)*ppc_column; i++) {
                for (unsigned j=ceil(y*ppc_row); j<height && j<(y+1)*ppc_row; j++) {
                    red_sum += r(i, j);
                    green_sum += g(i, j);
                    blue_sum += b(i, j);
                    count++;
                }
            }
            if (count == 0) continue;
            red_sum /= count;
            green_sum /= count;
            blue_sum /= count;
            img_data.push_back(red_sum);
            img_data.push_back(green_sum);
            img_data.push_back(blue_sum);
        }
    }
    img = img_data;
    width = x;
    height = y;
}
