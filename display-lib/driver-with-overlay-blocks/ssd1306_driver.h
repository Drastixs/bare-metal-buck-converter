#ifndef SSD1306_DRIVER_H
#define SSD1306_DRIVER_H
#include "i2c.h"
#include "avr/io.h"
#include <util/delay.h>
#include "math.h"
#include "ssd1306_driver_commands.h"

#define SSD1306_ADDR 0x3C
#define PAGE_ADDR_PAGE 0x07
#define PAGE_ADDR_BIT 0x70
#define TRANSACTION_CMD 0x00
#define TRANSACTION_DATA 0x40

struct PageAddr{
	uint8_t __pageAndBit = 0;//Most significant hex char represents bit
	//least significant hex char represents page
	uint8_t __col = 0;
	
	uint8_t col(){
		return __col;
	}

	void col(uint8_t newCol){
		__col = newCol;
	}

	uint8_t page(){
		return __pageAndBit & PAGE_ADDR_PAGE;
	}

	void page(uint8_t newPage){
		__pageAndBit |= (newPage & PAGE_ADDR_PAGE);
	}
	
	uint8_t bit(){
		return (__pageAndBit & PAGE_ADDR_BIT) >> 4;
	}

	void bit(uint8_t newBit){
		__pageAndBit |= (newBit << 4) & PAGE_ADDR_BIT; 
	}
};

typedef struct PageAddr page_addr_t;

void setPageAddrCol(page_addr_t* pageAddr_p, uint8_t col);
void setPageAddrPage(page_addr_t* pageAddr_p, uint8_t page);
void setPageAddrBit(page_addr_t* pageAddr_p, uint8_t bit);
uint8_t getPageAddrCol(page_addr_t* pageAddr_p);
uint8_t getPageAddrPage(page_addr_t* pageAddr_p);
uint8_t getPageAddrBit(page_addr_t* pageAddr_p);

class c_ssd1306 {
	public:
		c_ssd1306(uint8_t width, uint8_t height);
        void displayOff();
        void displayOn(); 
		void setContrast(uint8_t value);
		void setBrightness(uint8_t value);
		void setPageAddr(page_addr_t addr);
		page_addr_t getPageAddr();
		uint8_t convertPosToPageAddr(uint8_t x, uint8_t y, page_addr_t* pageAddr);
		void write(uint8_t data);
		void write(uint8_t* data, uint8_t size);
        void fillPage(uint8_t page);
        void clearPage(uint8_t page);
        void clear();
	
	private:
		void __init();
		void __incrementPageAddr(uint8_t addon);
		uint8_t MAX_PAGES;
		page_addr_t __currPageAddr;
		//way to send
		void __startTransaction(uint8_t type);
		void __stopTransaction();
		void __sendCmd(uint8_t command);
		void __sendCmd(uint8_t* cmds,uint8_t size);
		void __sendData(uint8_t data);
		void __sendData(uint8_t* data, uint8_t size);
		bool __inTransaction = false;
		//commands 
		void __setPage(uint8_t page);
		void __setCol(uint8_t col);	
		

		
}; 

#endif
