#ifndef canvas_h
#define canvas_h
#include "avr/io.h"
#include "ssd1306_driver.h"
#include "math_utils.h"
#include "draw/regions.h"

/*in order to reduce memory requirements, i will reduce character set.
Not use a full buffer, use about a page buffer and drawRegions which will i will 
have an array of 10 regions. If any overlap then store a buffer with a pointer to where
in the page buffer they overlap and the size of how many bytes they overlap.
*/

/*updated solution
split display into 8x8 sections. have a (128/8) 16 byte buffer with bits for when a 1 represents if the 
section has been written to and 0 representing the region is free.
Have a buffer which stores 32 regions, 256 byte buffer which can store 25% of the regions.
Have an array of 32 regions (pos_t), so 64 bytes of pointers
When something want to write to the section
have a */


/*solution 2 
split display by pages and cols so (128 * 8)/8 = 128 byte buffer. to cover 102 overlaps, 10% of screen
would require 102 byte buffer and 102 array of pageAddrs so 202 bytes*/

class c_canvas{
    public:
        c_canvas();
        void fillPos(pos_t);
        void clearPos(pos_t);
        void flush();//writes currPage pixels to display
        void clear();//clears display
        void update();//updates region drawer

        void off();
        void on();
        void brightness(uint8_t value);
        void contrast(uint8_t value);

        c_region_drawer draw;
    private:
        void __updateDisplayRegion(region_t region, block_t* buff_p);
        
        c_ssd1306 __display;
		page_addr_t __currPageAddr;
		uint8_t __currPixels = 0;
        void __updateCurrPageAddr(pos_t);
        page_addr_t __getPageAddrOfPos(pos_t pos);
    

};

#endif