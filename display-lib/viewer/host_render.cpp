// Host-side renderer: drives the REAL drawing library (the same regions.cpp /
// blocks.cpp / *_draw.cpp the firmware uses) but renders into a flat RAM
// framebuffer instead of pushing over I2C to an SSD1306. Exposed via a C ABI so
// Python (ctypes) can call it. The drawing logic is identical to on-hardware;
// only the transport differs.
#include <list>
#include <string>
#include <cstring>
#include "draw/regions.h"

struct HostCanvas {
    c_region_drawer drawer;
    int w, h;
    std::list<std::string> strings;   // stable addresses; keeps text alive for the drawer
    HostCanvas(int width, int height) : drawer(width, height), w(width), h(height) {}
};

extern "C" {

void* canvas_new(int w, int h){ return new HostCanvas(w, h); }
void  canvas_free(void* c){ delete (HostCanvas*)c; }
void  canvas_clear(void* c){ HostCanvas* hc=(HostCanvas*)c; hc->drawer.clear(); hc->strings.clear(); }

void canvas_pixel(void* c, int x, int y){
    pos_t p; p.x=x; p.y=y; ((HostCanvas*)c)->drawer.pixel(p);
}
void canvas_line(void* c, int x0, int y0, int x1, int y1){
    pos_t a{(uint8_t)x0,(uint8_t)y0}, b{(uint8_t)x1,(uint8_t)y1};
    ((HostCanvas*)c)->drawer.line(a,b);
}
void canvas_rect(void* c, int x, int y, int w, int h){
    pos_t p{(uint8_t)x,(uint8_t)y}; ((HostCanvas*)c)->drawer.rect(p,w,h);
}
void canvas_circle(void* c, int cx, int cy, int r){
    pos_t p{(uint8_t)cx,(uint8_t)cy}; ((HostCanvas*)c)->drawer.circle(p,r);
}
void canvas_text(void* c, int x, int y, const char* s){
    HostCanvas* hc=(HostCanvas*)c;
    hc->strings.emplace_back(s);
    pos_t p{(uint8_t)x,(uint8_t)y};
    hc->drawer.text(p, hc->strings.back().c_str());
}

// Render every region through the real loop; unpack each 8x8 block into `out`
// (one byte per pixel, 0 or 1), row-major, size w*h.
void canvas_render(void* c, uint8_t* out){
    HostCanvas* hc=(HostCanvas*)c;
    std::memset(out, 0, (size_t)hc->w * hc->h);
    hc->drawer.resetLoop();
    block_t blk;
    while (hc->drawer.hasLoop()){
        clearBlock(&blk);
        region_t rg = hc->drawer.loop(&blk);
        for (uint8_t row=0; row<8; row++){
            for (uint8_t col=0; col<8; col++){
                if (blk[row] & (1 << (7-col))){
                    int X = rg.x + col, Y = rg.y + row;
                    if (X < hc->w && Y < hc->h) out[Y*hc->w + X] = 1;
                }
            }
        }
    }
}

} // extern "C"
