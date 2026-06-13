#ifndef TEXT_DRAW_H
#define TEXT_DRAW_H
#include "blocks.h"
#include "region.h"
#include "font5x7.h"
// text stored as x,y (top-left) + pointer to a NUL-terminated string.
uint8_t textPixelWidth(const char* str);   // rendered width in pixels
bool isTextInRegion(region_t rgn, draw_command_t cmd);
void drawTextOnBlock(block_t* blk_p, draw_command_t cmd, region_t rgn);
#endif
