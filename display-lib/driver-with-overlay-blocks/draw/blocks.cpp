#include "blocks.h"

/* Convention used throughout:
   block_t == uint8_t[8].  (*blk)[y] is row y (y=0 == top).
   Within a row byte, pixel x lives in bit (7 - x), so x=0 is the MSB (leftmost).
   A "column byte" packs the 8 vertical pixels of one column with y=0 in the MSB. */

void setColumnByte(block_t* blk_p, uint8_t col, uint8_t byte){
    uint8_t bitPos = 7 - col;            // x bit within each row
    for (uint8_t y = 0; y < REGION_HEIGHT; y++){
        uint8_t b = (byte >> (7 - y)) & 1;   // top row (y=0) takes the MSB
        (*blk_p)[y] = ((*blk_p)[y] & ~(1 << bitPos)) | (b << bitPos);
    }
}

uint8_t getColumnByte(block_t* blk_p, uint8_t col){
    uint8_t val = 0;
    uint8_t rightShift = 7 - col;//col , x = col, then right shift 7-col bits to right
    for (uint8_t y = 0; y < REGION_HEIGHT; y++){
       val = val << 1;
       val |= ((*blk_p)[y] >> rightShift) & 1;
    }
    return val;
}

uint8_t reverseByte(uint8_t byte){
    uint8_t out = 0;
    for (uint8_t i = 0;i < 8; i++){
        out = (out << 1) | (byte & 1);
        byte >>= 1;
    }
    return out;
}

void blockCpy(block_t* in_p, block_t* out_p){
    //copy input to output
    for (uint8_t i = 0;i < 8; i++){
        (*out_p)[i] = (*in_p)[i];
    }
}

void blockSet(block_t* blk, uint8_t data[8]){
    for (uint8_t i = 0; i < REGION_WIDTH;i++){
        (*blk)[i] = data[i];
    }
}

void clearBlock(block_t* blk_p){
    for (uint8_t i = 0; i < REGION_HEIGHT;i++){
        (*blk_p)[i] = 0;
    }
}

void rotateBlockACW(block_t* in_p){
    block_t out;
    clearBlock(&out);
    for (uint8_t x = 0;x < REGION_HEIGHT;x++){
       out[7-x] = getColumnByte(in_p,x);
    }
    blockCpy(&out, in_p);
}

void rotateBlockCW(block_t* in_p){
    block_t out;
    clearBlock(&out);//clears block
    for (uint8_t x = 0; x < REGION_HEIGHT;x++){
        out[x] = reverseByte(getColumnByte(in_p,x));//get column byte and reverse
    }
    blockCpy(&out,in_p);
}

void flipBlockOnX(block_t* in_p){
    uint8_t tempByte;
    for (uint8_t i = 0;i < REGION_WIDTH/2;i++){
        tempByte = (*in_p)[i];
        (*in_p)[i] = (*in_p)[7-i];
        (*in_p)[7-i] = tempByte;
    }
}

void flipBlockOnY(block_t* in_p){
    uint8_t tempByte;
    for (uint8_t i = 0;i < REGION_HEIGHT/2;i++){
        tempByte = getColumnByte(in_p,i);
        setColumnByte(in_p, i, getColumnByte(in_p, 7-i));
        setColumnByte(in_p,7-i,tempByte);
    }
}

void hlineOnBlock(block_t* block_p, uint8_t startX, uint8_t endX, uint8_t y){
    startX &= 0x07; endX &= 0x07; y &= 0x07;
    if (startX > endX){ uint8_t t = startX; startX = endX; endX = t; }
    for (uint8_t x = startX; x <= endX; x++){
        (*block_p)[y] |= (1 << (7 - x));
    }
}

void vlineOnBlock(block_t* block_p, uint8_t startY, uint8_t endY, uint8_t x){
    startY &= 0x07; endY &= 0x07; x &= 0x07;
    if (startY > endY){ uint8_t t = startY; startY = endY; endY = t; }
    uint8_t bitPos = 7 - x;
    for (uint8_t y = startY; y <= endY; y++){
        (*block_p)[y] |= (1 << bitPos);
    }
}

void pixelOnBlock(block_t* blk_p, uint8_t x, uint8_t y){
    (*blk_p)[y] |= (1 << (REGION_WIDTH - 1 - x));
}
