#ifndef _I2C_API_H
#define _I2C_API_H

#include "i2c_hal.h"

void i2c_setSpeed(uint32_t clockSpeed);
void i2c_enableDMAMode(bool enableDMAMode);
void i2c_stretchClock(bool stretch);

void i2c_begin(void);
void i2c_beginTransmission(uint8_t address);
void i2c_end(void);
uint8_t i2c_endTransmission(uint8_t sendStop);

uint8_t i2c_requestFrom(uint8_t address, uint8_t quantity, uint8_t sendStop);
size_t i2c_writeOneByte(uint8_t data);
size_t i2c_writeBytes(uint8_t *data, size_t len);
int i2c_available(void);
int i2c_read(void);
int i2c_peek(void);
void i2c_flush(void);
bool i2c_isEnabled(void);
void i2c_onReceive( void (*function)(int) );
void i2c_onRequest( void (*function)(void) );


#endif
