#include "i2c.h"
void i2c_start(){
	TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);
	while (!(TWCR & (1 << TWINT)));
}

void i2c_stop(){
	TWCR = (1 << TWSTO) | (1 << TWEN) | (1 << TWINT);
	while (TWCR & (1 << TWSTO));
}

void i2c_init(){
	TWSR = 0x00;          // prescaler = 1
	TWBR = 72;            // 100 kHz @ 16 MHz (TWBR=1 gave ~440 kHz, over SSD1306 spec)
	TWCR = (1 << TWEN);
}

void i2c_write (uint8_t data){
	TWDR = data;
	TWCR = (1 << TWEN) | (1 << TWINT);
	while (!(TWCR & (1 << TWINT)));
}

// Reads are intentionally unimplemented: the SSD1306 is write-only in this driver.
uint8_t i2c_read_ack(){
	return 0;
}

uint8_t i2c_read_nack(){
	return 0;
}

void i2c_write_to_addr(uint8_t addr, uint8_t data){
	i2c_write(addr << 1);
	i2c_write(data);
	i2c_stop();
}

uint8_t i2c_read_from_addr(uint8_t addr){
	i2c_write(addr << 1 | 1);//indicate write bit
	return 0;
}
