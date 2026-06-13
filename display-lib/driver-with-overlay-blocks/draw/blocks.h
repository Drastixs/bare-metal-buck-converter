#ifndef BLOCKS_H
#define BLOCKS_H
#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 64
#define REGION_WIDTH 8
#define REGION_HEIGHT 8
#define MAX_STORED_DRAW_COMMANDS 32
#include "avr/io.h"
#include "stdlib.h"

/*block x is zero at leftmost and max at rightmost
block y is zero at highest point and max at lowest point*/

typedef uint8_t block_t[8];//stores 64 bits for 8x8 section
//rotating region buffs to make it easier to write to display
void setColumnByte(block_t* block, uint8_t col, uint8_t byte);//sets bits from top to bottom of column
uint8_t getColumnByte(block_t* buff_p, uint8_t col);//gets bits from top to bottom of a column
uint8_t reverseByte(uint8_t byte);//reverse a byte
void blockCpy(block_t* in_p, block_t* out_p);//copies on block to another
void blockSet(block_t* blk_p, uint8_t data[8]);
void clearBlock(block_t* buff_p);//writes zeros to all of block
void rotateBlockACW(block_t* buff_p);//anticlockwise block rotate
void rotateBlockCW(block_t* buff_p);//clockwise block rotate
void flipBlockOnX(block_t* buff_p);//block flip by x axis
void flipBlockOnY(block_t* buff_p);//block flip by y axis

//operations for drawing on block
void hlineOnBlock(block_t* block_p, uint8_t startX, uint8_t endX, uint8_t y);
void vlineOnBlock(block_t* block_p, uint8_t startY, uint8_t endY, uint8_t x);
void pixelOnBlock(block_t* blk_p, uint8_t x, uint8_t y);

#endif