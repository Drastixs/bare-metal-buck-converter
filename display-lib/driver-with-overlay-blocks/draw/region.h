#ifndef REGION_H
#define REGION_H
#include <stdint.h>

#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 64
#define REGION_WIDTH 8
#define REGION_HEIGHT 8

struct Position{
	uint8_t x;
	uint8_t y;
};
typedef struct Position pos_t;
typedef struct Position region_t;

enum DrawCommandType{
    CMD_PIXEL,
    CMD_LINE,
    CMD_RECT,
    CMD_CIRCLE,
    CMD_TEXT,
};

struct DrawCommand{
    enum DrawCommandType type;
    struct {uint8_t x, y;} pixel;
    struct {uint8_t x0, y0, x1, y1; float m; short c;} line;
    struct {uint8_t x, y, w, h;} rect;
    struct {uint8_t x, y, r;} circle;
    struct {uint8_t x, y; const char* str;} text;
};

typedef struct DrawCommand draw_command_t;

inline bool isPosInRegion(region_t reg, pos_t pos){
    //chop last 3 bits off and compare
    return (reg.x == (pos.x & 0xF8)) && (reg.y == (pos.y & 0xF8));
}

#endif