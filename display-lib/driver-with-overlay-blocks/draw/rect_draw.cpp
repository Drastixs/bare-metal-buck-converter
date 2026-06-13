#include "rect_draw.h"

static inline void plotIfInRegion(block_t* blk_p, region_t rgn, short x, short y){
    if (x >= rgn.x && x < rgn.x + REGION_WIDTH &&
        y >= rgn.y && y < rgn.y + REGION_HEIGHT){
        pixelOnBlock(blk_p, x & 0x07, y & 0x07);
    }
}

bool isRectInRegion(region_t rgn, draw_command_t cmd){
    short x0 = cmd.rect.x;
    short y0 = cmd.rect.y;
    short x1 = x0 + cmd.rect.w - 1;
    short y1 = y0 + cmd.rect.h - 1;
    return (rgn.x <= x1) && ((rgn.x + REGION_WIDTH - 1) >= x0) &&
           (rgn.y <= y1) && ((rgn.y + REGION_HEIGHT - 1) >= y0);
}

void drawRectOnBlock(block_t* blk_p, draw_command_t cmd, region_t rgn){
    if (cmd.rect.w == 0 || cmd.rect.h == 0) return;
    short x0 = cmd.rect.x;
    short y0 = cmd.rect.y;
    short x1 = x0 + cmd.rect.w - 1;
    short y1 = y0 + cmd.rect.h - 1;

    for (short x = x0; x <= x1; x++){      // top + bottom edges
        plotIfInRegion(blk_p, rgn, x, y0);
        plotIfInRegion(blk_p, rgn, x, y1);
    }
    for (short y = y0; y <= y1; y++){      // left + right edges
        plotIfInRegion(blk_p, rgn, x0, y);
        plotIfInRegion(blk_p, rgn, x1, y);
    }
}
