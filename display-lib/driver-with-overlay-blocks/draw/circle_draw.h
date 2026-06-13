#ifndef CIRCLE_DRAW_H
#define CIRCLE_DRAW_H
#include "blocks.h"
#include "region.h"
// circle stored as centre x,y + radius r. Drawn as an outline (midpoint algorithm).
bool isCircleInRegion(region_t rgn, draw_command_t cmd);
void drawCircleOnBlock(block_t* blk_p, draw_command_t cmd, region_t rgn);
#endif
