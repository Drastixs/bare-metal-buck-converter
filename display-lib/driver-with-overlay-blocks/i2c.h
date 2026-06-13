#ifndef I2C_H
#define I2C_H
#include <stdint.h>
#include <avr/io.h>

void i2c_init();


void i2c_write(uint8_t data);
uint8_t i2c_read_ack();
uint8_t i2c_read_nack();
void i2c_start();
void i2c_stop();

void i2c_write_to_addr(uint8_t addr, uint8_t data);
uint8_t i2c_read_from_addr(uint8_t addr);
#endif
