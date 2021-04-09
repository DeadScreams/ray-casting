#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <cassert>
#include <math.h>

using namespace std;

uint32_t pack_color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a=255) {
    return (a<<24) + (b<<16) + (g<<8) + r; 
}


void unpack_color(const uint32_t &color, uint8_t &r, uint8_t &g, uint8_t &b, uint8_t &a) {
    r = (color >>  0) & 255;
    g = (color >>  8) & 255;
    b = (color >> 16) & 255;
    a = (color >> 24) & 255;
}


void draw_rectangle(vector<uint32_t> &img, const size_t img_w, const size_t img_h, const size_t x, const size_t y, const size_t w, const size_t h, const uint32_t color) {
    assert(img.size()==img_w*img_h);
    for (size_t i=0; i<w; i++) {
        for (size_t j=0; j<h; j++) {
            size_t rx = x+i;
            size_t ry = y+j;
            if (rx>=img_w || ry>=img_h) continue;
            img[rx + ry*img_w] = color;
        }
    }
}


void drop_ppm_image(const string filename, const vector<uint32_t> &image, const size_t w, const size_t h) {
    assert(image.size() == w*h);
    ofstream ofs(filename);
    ofs << "P6\n" << w << " " << h << "\n255\n";
    for (size_t i = 0; i < h*w; ++i) {
        uint8_t r, g, b, a;
        unpack_color(image[i], r, g, b, a);
        ofs << static_cast<char>(r) << static_cast<char>(g) << static_cast<char>(b);
    }
    ofs.close();
}


int main() {
    const size_t win_w = 1024;
    const size_t win_h = 512;  

    const size_t map_w = 16;
    const size_t map_h = 16;
    const char map[] =  "0222222222222220"\
                        "1              1"\
                        "1     333333   1"\
                        "1     3        1"\
                        "1     3  1111111"\
                        "1     3        1"\
                        "1     311      1"\
                        "1       11111  1"\
                        "1       1      1"\
                        "1   3   1  00001"\
                        "1       1      1"\
                        "1       1      1"\
                        "1       1      1"\
                        "1 0000000      1"\
                        "1              1"\
                        "0222222222222220";
    assert(sizeof(map) == map_w*map_h+1);

    //colors
    const size_t ncolors = 10;
    std::vector<uint32_t> colors(ncolors);
    for (size_t i=0; i<ncolors; i++) {
        colors[i] = pack_color(rand()%255, rand()%255, rand()%255);
    }
    colors[1] = pack_color(10, 10, 10);


    //player
    float player_x = 3.456; 
    float player_y = 2.345; 
    const float fov = M_PI/3;
    vector<uint32_t> framebuffer(win_w*win_h, pack_color(0, 0, 0));
    float player_v = 1.523;


    //draw map
    const size_t rect_w = win_w/(map_w*2);
    const size_t rect_h = win_h/map_h;
    for (size_t j=0; j<map_h; j++) {
        for (size_t i=0; i<map_w; i++) {
            if (map[i+j*map_w]==' ') continue;
            size_t rect_x = i*rect_w;
            size_t rect_y = j*rect_h;
            size_t icolor = map[i+j*map_w] - '0';
            draw_rectangle(framebuffer, win_w, win_h, rect_x, rect_y, rect_w, rect_h, colors[icolor]);
        }
    }
    //draw 3D
    for (size_t i=0; i<win_w/2; i++) {
        float angle = player_v-fov/2 + fov*i/float(win_w/2);
        for (float t=0; t<20; t+=.01) {
            float cx = player_x + t*cos(angle);
            float cy = player_y + t*sin(angle);

            //FOV visualization(OFF)

            size_t pix_x = cx*rect_w;
            size_t pix_y = cy*rect_h;
            framebuffer[pix_x + pix_y*win_w] = pack_color(0, 160, 160);

            if (map[int(cx)+int(cy)*map_w]!=' ') {
                size_t icolor = map[int(cx)+int(cy)*map_w] - '0';
                size_t column_height = win_h/(t*cos(angle-player_v));
                draw_rectangle(framebuffer, win_w, win_h, win_w/2+i, win_h/2-column_height/2, 1, column_height, colors[icolor]);
                break;
            }
        }
    }

     draw_rectangle(framebuffer, win_w, win_h, player_x*rect_w-2, player_y*rect_h, 7, 6, pack_color(255, 255, 255)); //draw player

    string name = "output.ppm";
    drop_ppm_image(name, framebuffer, win_w, win_h);
    return 0;
}

