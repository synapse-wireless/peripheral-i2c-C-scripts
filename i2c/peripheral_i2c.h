#ifndef PERIPHERAL_I2C_H
#define PERIPHERAL_I2C_H

#include "snappy.h"

void delay(uint16_t cycles);

void i2c_init(uint8_t twbr, uint8_t twps, uint16_t timeout, uint8_t num_retries);

int16_t i2c_start(uint8_t address);
void i2c_restart();
void i2c_stop();

int16_t i2c_read(uint8_t address, char __generic * buffer, uint8_t length, bool endWithRestart);
int16_t i2c_read_byte(char __generic *data);

int16_t i2c_write(uint8_t address, char __generic * data, uint8_t length, bool endWithRestart);
int16_t i2c_write_byte(char __generic data);

#endif // PERIPHERAL_I2C_H
