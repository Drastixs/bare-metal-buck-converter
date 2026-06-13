#ifndef LINE_DRAW_H
#define LINE_DRAW_H
#include "blocks.h"
#include "region.h"
#define INFINITE_GRADIENT 255
bool isLineInRegion(region_t rgn, draw_command_t cmd);
void drawLineOnBlock(block_t* blk_p, draw_command_t cmd, region_t rgn);


#endif