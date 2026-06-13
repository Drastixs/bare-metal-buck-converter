#include "ssd1306_driver.h"

void setPageAddrCol(page_addr_t* pageAddr_p, uint8_t col){
	pageAddr_p->__col = col;
}

void setPageAddrPage(page_addr_t* pageAddr_p, uint8_t page){
	pageAddr_p->__pageAndBit &= 0xf0;//clear page but keep bit
    pageAddr_p->__pageAndBit |= (page & PAGE_ADDR_PAGE);
}

void setPageAddrBit(page_addr_t* pageAddr_p, uint8_t bit){
    pageAddr_p->__pageAndBit &= 0x0f;//clear bit but keep page
    pageAddr_p->__pageAndBit |= ((bit << 4) & PAGE_ADDR_BIT);
}

uint8_t getPageAddrCol(page_addr_t* pageAddr_p){
	return pageAddr_p->__col;
}

uint8_t getPageAddrPage(page_addr_t* pageAddr_p){
	return pageAddr_p->__pageAndBit & PAGE_ADDR_PAGE;
}

uint8_t getPageAddrBit(page_addr_t* pageAddr_p){
	return (pageAddr_p->__pageAndBit & PAGE_ADDR_BIT) >> 4;
}

void c_ssd1306::__startTransaction(uint8_t type){
    if (__inTransaction){
        return;
    }
    i2c_start();
    i2c_write(SSD1306_ADDR << 1);//send address
    i2c_write(type);
    __inTransaction = true;
}

void c_ssd1306::__stopTransaction(){
    if (!__inTransaction){
        return;
    }
    i2c_stop();
    __inTransaction = false;
}


void c_ssd1306::__sendCmd(uint8_t command){
    //i2c_start//_i2c_write already includes start 
    __startTransaction(TRANSACTION_CMD);
    i2c_write(command);
    __stopTransaction();
}

void c_ssd1306::__sendCmd(uint8_t* cmds, uint8_t size){
    __startTransaction(TRANSACTION_CMD);
    for (uint8_t i = 0;i < size;i++){
        i2c_write(cmds[i]);
    }
    __stopTransaction();
}

void c_ssd1306::__sendData(uint8_t data){
    //i2c_start//_i2c_write already includes start
    __startTransaction(TRANSACTION_DATA);
    i2c_write(data);
    __stopTransaction();
}

void c_ssd1306::__sendData(uint8_t* data_p, uint8_t size){
    __startTransaction(TRANSACTION_DATA);
    for (uint8_t i = 0;i < size;i++){
        i2c_write(data_p[i]);
    }
    __stopTransaction();
}


void c_ssd1306::__setPage(uint8_t page){
    if (page > MAX_PAGES){
        return;
    }
    __sendCmd(PAGE_START_ADDR + page);
}

void c_ssd1306::__setCol(uint8_t col){
    if (col > 127){
        return;
    }
    uint8_t cmds[2] = {LOW_COLUMN_START_ADDR + (col & 0x0f), HIGH_COLUMN_START_ADDR + ((col >> 4) & 0x0f)};
    __sendCmd(cmds,2);
}

void c_ssd1306::__init(){
    _delay_ms(100);
    uint8_t com_config_pins;
    uint8_t multiplex_display;
    if (MAX_PAGES == 3){
        com_config_pins = DEFAULT_COM_CONFIG_PINS_32PX;
        multiplex_display = MULTIPLEX_32_PX_DISPLAY;
    }
    else {
        com_config_pins = DEFAULT_COM_CONFIG_PINS_64PX;
        multiplex_display = MUTLIPLEX_64_PX_DISPLAY;
    }
    uint8_t cmds[] = {DISPLAY_OFF,SET_MEM_ADDR_MODE,HORIZONTAL_MEM_ADDR_MODE,PAGE_START_ADDR,
                COM_OUTPUT_SCAN_DIRECTION_INVERT, LOW_COLUMN_START_ADDR, HIGH_COLUMN_START_ADDR,
                START_LINE_START_ADDR,SET_CONTRAST,255,SEGMENT_REMAP_INVERT,SET_MULTIPLEX_RATIO,
                multiplex_display,SET_DISPLAY_OFFSET,0x00,DISPLAY_CLOCK,FAST_CLOCK,
                DEFAULT_VCOMH,SET_CHARGE_PUMP,CHARGE_PUMP_ENABLE,DISPLAY_ON};
    __sendCmd(cmds,sizeof(cmds));
}

c_ssd1306::c_ssd1306(uint8_t width, uint8_t height){
    if (height == 32){
        MAX_PAGES = 3;
    }
    else {
        MAX_PAGES = 7;
    }
    __init();
}

void c_ssd1306::__incrementPageAddr(uint8_t addon){
    uint8_t col = getPageAddrCol(&__currPageAddr);
    col += addon;
    if (col > 127){
        col -= 128;
        uint8_t page = getPageAddrPage(&__currPageAddr);
        page++;
        if (page >= 8){
            page = 0;
        }
        setPageAddrPage(&__currPageAddr,page);
    }
    setPageAddrCol(&__currPageAddr,col);
}



void c_ssd1306::displayOn(){
    __sendCmd(DISPLAY_ON);
}

void c_ssd1306::displayOff(){
    __sendCmd(DISPLAY_OFF);
}

void c_ssd1306::setContrast(uint8_t value){
    uint8_t cmds[2] = {SET_CONTRAST,value};
    __sendCmd(cmds,2);
}

void c_ssd1306::setBrightness(uint8_t value){
    uint8_t charge_value = value >> 4;//scale to 15-0
    uint8_t discharge_value = 16 - charge_value;
    uint8_t cmds[2] = {SET_DISPLAY_PRECHARGE,(charge_value << CHARGE) | (discharge_value << DISCHARGE)};
    __sendCmd(cmds,2);
}

void c_ssd1306::setPageAddr(page_addr_t pageAddr){
    __currPageAddr = pageAddr;
    __setPage(getPageAddrPage(&pageAddr));
    __setCol(getPageAddrCol(&pageAddr));
}

page_addr_t c_ssd1306::getPageAddr(){
    return __currPageAddr;
}

uint8_t c_ssd1306::convertPosToPageAddr(uint8_t x, uint8_t y, page_addr_t* pageAddr_p){
    if (x > 127 || (y >> 3) > MAX_PAGES){
        return 0;//out of bounds
    }
    setPageAddrPage(pageAddr_p, y >> 3);
    setPageAddrCol(pageAddr_p, x);
    setPageAddrBit(pageAddr_p, y & 0x07);
    return 1;
}

void c_ssd1306::write(uint8_t data){
    __sendData(data);
    __incrementPageAddr(1);
}

void c_ssd1306::write(uint8_t* data, uint8_t size){
    __sendData(data,size); 
    __incrementPageAddr(size);
}


void c_ssd1306::fillPage(uint8_t page){
    __setPage(page);
    __setCol(0);
    for (int col = 0; col <= 127; col ++){
        __sendData(0xFF);
    }
}

void c_ssd1306::clearPage(uint8_t page){
    __setPage(page);
    __setCol(0);
    for (int col = 0; col <= 127; col ++){
        __sendData(0x00);
    }
}

void c_ssd1306::clear(){
    for (uint8_t page = 0; page <= MAX_PAGES; page++){
        clearPage(page);
    }
}