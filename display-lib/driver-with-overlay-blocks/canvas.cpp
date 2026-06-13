#include "canvas.h"

c_canvas::c_canvas()
 : __display(SSD1306_WIDTH,SSD1306_HEIGHT)
{

}

page_addr_t c_canvas::__getPageAddrOfPos(pos_t pos){
    page_addr_t pageAddr;
    setPageAddrPage(&pageAddr, pos.y >> 3);//div by 8
    setPageAddrCol(&pageAddr, pos.x);
    setPageAddrBit(&pageAddr, pos.y & 0x07);//mod by 8
    return pageAddr;
}

void c_canvas::__updateCurrPageAddr(pos_t pos){//updates the current page addr base on the pos
    page_addr_t pageAddr = __getPageAddrOfPos(pos);
    //if pageAddr in value in __currPixels
    if ((getPageAddrCol(&pageAddr) == getPageAddrCol(&__currPageAddr)) && (getPageAddrPage(&pageAddr) == getPageAddrPage(&__currPageAddr))){
        return;
    }
    flush();
    __currPageAddr = pageAddr;//update __currPage
    __currPixels = 0;
}

void c_canvas::flush(){//writes the current column byte to the display
    __display.setPageAddr(__currPageAddr);
    __display.write(__currPixels);
}

void c_canvas::fillPos(pos_t pos){
    __updateCurrPageAddr(pos);
    __currPixels |= (1 << getPageAddrBit(&__currPageAddr));
}

void c_canvas::clearPos(pos_t pos){
    __updateCurrPageAddr(pos);//updates currPageAddr
    __currPixels &= ~(1 << getPageAddrBit(&__currPageAddr));
}

void c_canvas::clear(){
    __display.clear();
}

// Walk every 8x8 region the drawer knows about; rasterize the touched ones into
// a single scratch block and push each straight to the panel. No framebuffer.
void c_canvas::update(){
    draw.resetLoop();
    block_t block;
    while (draw.hasLoop()){
        clearBlock(&block);
        region_t rgn = draw.loop(&block);
        __updateDisplayRegion(rgn, &block);
    }
}

void c_canvas::__updateDisplayRegion(region_t rgn, block_t* block_p){
    // block is row-major (each byte = 8 horizontal pixels, MSB = left).
    // SSD1306 page memory is column-major (each byte = 8 vertical pixels, LSB = top).
    // rotateBlockCW turns block[i] into the display byte for column i.
    rotateBlockCW(block_p);
    page_addr_t addr;
    setPageAddrPage(&addr, rgn.y >> 3);
    setPageAddrCol(&addr, rgn.x);
    __display.setPageAddr(addr);
    __display.write(*block_p, REGION_WIDTH);
}


void c_canvas::on(){
    __display.displayOn();
}

void c_canvas::off(){
    __display.displayOff();
}

void c_canvas::contrast(uint8_t value){
   __display.setContrast(value);
}

void c_canvas::brightness(uint8_t value){
    __display.setBrightness(value);
}
