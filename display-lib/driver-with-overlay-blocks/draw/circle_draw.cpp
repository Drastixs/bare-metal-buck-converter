#include "circle_draw.h"

static inline void plotIfInRegion(block_t* blk_p, region_t rgn, short x, short y){
    if (x >= rgn.x && x < rgn.x + REGION_WIDTH &&
        y >= rgn.y && y < rgn.y + REGION_HEIGHT){
        pixelOnBlock(blk_p, x & 0x07, y & 0x07);
    }
}

bool isCircleInRegion(region_t rgn, draw_command_t cmd){
    short cx = cmd.circle.x, cy = cmd.circle.y, r = cmd.circle.r;
    short x0 = cx - r, x1 = cx + r;
    short y0 = cy - r, y1 = cy + r;
    return (rgn.x <= x1) && ((rgn.x + REGION_WIDTH - 1) >= x0) &&
           (rgn.y <= y1) && ((rgn.y + REGION_HEIGHT - 1) >= y0);
}

// Midpoint circle: generate the 8 symmetric octant points, plot the ones that
// fall inside the current region. Integer-only.
void drawCircleOnBlock(block_t* blk_p, draw_command_t cmd, region_t rgn){
    short cx = cmd.circle.x, cy = cmd.circle.y;
    short r = cmd.circle.r;
    if (r == 0){ plotIfInRegion(blk_p, rgn, cx, cy); return; }

    short x = 0, y = r;
    short d = 1 - r;
    while (x <= y){
        plotIfInRegion(blk_p, rgn, cx + x, cy + y);
        plotIfInRegion(blk_p, rgn, cx - x, cy + y);
        plotIfInRegion(blk_p, rgn, cx + x, cy - y);
        plotIfInRegion(blk_p, rgn, cx - x, cy - y);
        plotIfInRegion(blk_p, rgn, cx + y, cy + x);
        plotIfInRegion(blk_p, rgn, cx - y, cy + x);
        plotIfInRegion(blk_p, rgn, cx + y, cy - x);
        plotIfInRegion(blk_p, rgn, cx - y, cy - x);
        if (d < 0){
            d += 2 * x + 3;
        } else {
            d += 2 * (x - y) + 5;
            y--;
        }
        x++;
    }
}
