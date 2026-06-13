#include "text_draw.h"

uint8_t textPixelWidth(const char* str){
    uint8_t n = 0;
    while (str[n]) n++;
    if (n == 0) return 0;
    return n * FONT_ADVANCE - 1;   // last glyph has no trailing spacer
}

bool isTextInRegion(region_t rgn, draw_command_t cmd){
    short x0 = cmd.text.x;
    short y0 = cmd.text.y;
    short x1 = x0 + textPixelWidth(cmd.text.str) - 1;
    short y1 = y0 + FONT_HEIGHT - 1;
    return (rgn.x <= x1) && ((rgn.x + REGION_WIDTH - 1) >= x0) &&
           (rgn.y <= y1) && ((rgn.y + REGION_HEIGHT - 1) >= y0);
}

void drawTextOnBlock(block_t* blk_p, draw_command_t cmd, region_t rgn){
    const char* s = cmd.text.str;
    short penX = cmd.text.x;
    short baseY = cmd.text.y;
    for (uint8_t i = 0; s[i]; i++){
        uint8_t ch = (uint8_t)s[i];
        if (ch > 0x7F) ch = 0;                 // outside table -> blank glyph
        short gx = penX + i * FONT_ADVANCE;
        for (uint8_t col = 0; col < FONT_WIDTH; col++){
            uint8_t bits = FONT_READ(&font5x7[ch * FONT_WIDTH + col]);
            short x = gx + col;
            if (x < rgn.x || x >= rgn.x + REGION_WIDTH) continue;
            for (uint8_t row = 0; row < 8; row++){
                if (!(bits & (1 << row))) continue;   // bit0 = top row
                short y = baseY + row;
                if (y >= rgn.y && y < rgn.y + REGION_HEIGHT){
                    pixelOnBlock(blk_p, x & 0x07, y & 0x07);
                }
            }
        }
    }
}
