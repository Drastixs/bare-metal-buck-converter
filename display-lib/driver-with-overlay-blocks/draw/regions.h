#ifndef REGIONS_H
#define REGIONS_H
#include "avr/io.h"
#include "stdlib.h"
#include "region.h"
#include "blocks.h"
#include "pixel_draw.h"
#include "line_draw.h"
#include "rect_draw.h"
#include "circle_draw.h"
#include "text_draw.h"

#define MAX_STORED_DRAW_COMMANDS 32

class c_region_drawer{
    public:
        c_region_drawer(uint8_t width = SSD1306_WIDTH, uint8_t height = SSD1306_HEIGHT);
        void clear();
        bool isFull();
        void line(pos_t startPos, pos_t endPos);
        void rect(pos_t startPos, uint8_t width, uint8_t height);
        void pixel(pos_t pos);
        void circle(pos_t center, uint8_t radius);
        void text(pos_t pos, const char* str);
        void resetLoop();
        region_t loop(block_t* block_p);
        bool hasLoop();


    private:
        void __addDrawCommand(draw_command_t drawCommand);
        void __incrementLoopRegion();
        draw_command_t __drawCommands[MAX_STORED_DRAW_COMMANDS];
        uint8_t __drawCommandsLen = 0;
        region_t __rgn;
        uint8_t __width;
        uint8_t __height;
};


#endif