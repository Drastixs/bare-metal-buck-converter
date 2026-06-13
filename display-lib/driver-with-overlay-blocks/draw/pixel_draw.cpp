#include "pixel_draw.h"

bool isPixelInRegion(region_t reg, draw_command_t cmd){
    pos_t pos;
    pos.x = cmd.pixel.x;
    pos.y = cmd.pixel.y;
    return isPosInRegion(reg, pos);
}


void drawPixelOnBlock(block_t* blk_p, draw_command_t cmd){
    pixelOnBlock(blk_p,(cmd.pixel.x & 0x07),(cmd.pixel.y & 0x07));
}