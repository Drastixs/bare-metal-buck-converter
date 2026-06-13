#include "line_draw.h"
#include "../math_utils.h"

// Region (8x8 at rgn.x,rgn.y) intersects the segment's bounding box?
// Conservative: may pass a region the line just misses; drawLineOnBlock then
// plots nothing there, so it is only a (cheap) wasted block, never a wrong pixel.
bool isLineInRegion(region_t rgn, draw_command_t cmd){
    uint8_t sx0 = min(cmd.line.x0, cmd.line.x1);
    uint8_t sx1 = max(cmd.line.x0, cmd.line.x1);
    uint8_t sy0 = min(cmd.line.y0, cmd.line.y1);
    uint8_t sy1 = max(cmd.line.y0, cmd.line.y1);
    return (rgn.x <= sx1) && ((rgn.x + REGION_WIDTH - 1) >= sx0) &&
           (rgn.y <= sy1) && ((rgn.y + REGION_HEIGHT - 1) >= sy0);
}

// Integer Bresenham over the whole segment; only pixels that land inside this
// region get written. No floating point, no infinite-line overdraw.
void drawLineOnBlock(block_t* blk_p, draw_command_t cmd, region_t rgn){
    short x0 = cmd.line.x0, y0 = cmd.line.y0;
    short x1 = cmd.line.x1, y1 = cmd.line.y1;

    short dx = abs((short)(x1 - x0));
    short dy = -abs((short)(y1 - y0));
    short sx = x0 < x1 ? 1 : -1;
    short sy = y0 < y1 ? 1 : -1;
    short err = dx + dy;

    while (1){
        if (x0 >= rgn.x && x0 < rgn.x + REGION_WIDTH &&
            y0 >= rgn.y && y0 < rgn.y + REGION_HEIGHT){
            pixelOnBlock(blk_p, x0 & 0x07, y0 & 0x07);
        }
        if (x0 == x1 && y0 == y1) break;
        short e2 = 2 * err;
        if (e2 >= dy){ err += dy; x0 += sx; }
        if (e2 <= dx){ err += dx; y0 += sy; }
    }
}
