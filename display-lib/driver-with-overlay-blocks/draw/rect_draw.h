#ifndef RECT_DRAW_H
#define RECT_DRAW_H
#include "blocks.h"
#include "region.h"
// rect is stored as x,y (top-left) + w,h. Drawn as an outline.
bool isRectInRegion(region_t rgn, draw_command_t cmd);
void drawRectOnBlock(block_t* blk_p, draw_command_t cmd, region_t rgn);
#endif
