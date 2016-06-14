#ifndef TV_IMAGE_HPP
#define TV_IMAGE_HPP
#include <vector>
#include <string>
#include <stdlib.h>

class Image {
    /**
     *  Image representation - a vector of 3*width*height characters.
     */
    std::vector<char> img;
public:
    /**
     *  Image size in pixels.
     */
    size_t width, height;

    /**
     *  Constructor - load the image from a (jpg|png|tif) file
     */
    Image(const std::string& file);

    /**
     *  Functions to access the r/g/b components of a pixel in position x, y
     */
    inline unsigned char r(int x, int y) {return img[y*3*width+3*x];}
    inline unsigned char g(int x, int y) {return img[y*3*width+3*x+1];}
    inline unsigned char b(int x, int y) {return img[y*3*width+3*x+2];}

    /**
     *  Function to downscale the image to a new resolution.
     *  If the new resolution is bigger than the current one, nothing happens.
     *  The pixel_width and pixel_height parameters allow to specify the shape
     *  of the new pixels in terms of the old ones.
     */
    void downscale(size_t w, size_t h, size_t pixel_width, size_t pixel_height);
};

#endif
