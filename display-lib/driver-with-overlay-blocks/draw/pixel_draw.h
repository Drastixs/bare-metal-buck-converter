#ifndef PIXEL_DRAW_H
#define PIXEL_DRAW_H
#include "region.h"
#include "blocks.h"
//method for if pixel in region
bool isPixelInRegion(region_t reg, draw_command_t);
//draw pixel on block
void drawPixelOnBlock(block_t* block_p, draw_command_t cmd);


#endif