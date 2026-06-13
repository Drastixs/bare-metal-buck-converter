#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

#include "driver-with-overlay-blocks/ssd1306_driver.h"
#include "driver-with-overlay-blocks/draw/blocks.h"
#include "driver-with-overlay-blocks/canvas.h"
// -------------------- Main --------------------

// Vector demo: queue a pixel, a line segment, a rectangle outline and a circle,
// then let the canvas walk the regions and flush only the touched 8x8 blocks.
int test_drawer(){
    c_canvas canvas;
    canvas.clear();

    pos_t lineA; lineA.x = 10; lineA.y = 8;
    pos_t lineB; lineB.x = 60; lineB.y = 40;
    canvas.draw.line(lineA, lineB);

    pos_t rectPos; rectPos.x = 70; rectPos.y = 5;
    canvas.draw.rect(rectPos, 40, 22);

    pos_t circleC; circleC.x = 30; circleC.y = 44;
    canvas.draw.circle(circleC, 14);

    pos_t dot; dot.x = 100; dot.y = 50;
    canvas.draw.pixel(dot);

    while (1){
        canvas.update();      // full vector re-render via region/dirty-block flush
        _delay_ms(500);
    }
    return 0;
}

int test_blocks(){
    c_ssd1306 display(128,64);
    display.clear();
    _delay_ms(1000);
    display.fillPage(0);
    _delay_ms(1000);
    page_addr_t pageAddr;
    setPageAddrPage(&pageAddr, 0);setPageAddrCol(&pageAddr, 0);
    uint8_t col;
    uint8_t page;
    display.setPageAddr(pageAddr);
    static block_t fill = {0x81,0x42,0x24,0x18,0x18,0x24,0x42,0x81};
    while (1){
        if (page == 0 && col == 0){
            display.clear();
            _delay_ms(100);
        }
        display.write(fill,8);
        col = getPageAddrCol(&pageAddr);
        col += 8;
        if (col > 127){
            col = 0;
            page++;
            if (page >= 8){
                page = 0;
            }
            setPageAddrPage(&pageAddr, page);
            setPageAddrCol(&pageAddr,col);
            display.setPageAddr(pageAddr);
        }

    }

    return 0;

}


int main(void) {
    i2c_init();
    test_drawer();
}
