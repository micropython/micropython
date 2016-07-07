#pragma once

#include "stm32f7xx.h"
#include "gfx.h"

bool_t i2cInit(I2C_TypeDef* i2c);

void i2cSend(I2C_TypeDef* i2c, uint8_t slaveAddr, uint8_t* data, uint16_t length);
void i2cSendByte(I2C_TypeDef* i2c, uint8_t slaveAddr, uint8_t data);
void i2cWriteReg(I2C_TypeDef* i2c, uint8_t slaveAddr, uint8_t regAddr, uint8_t value);

void i2cRead(I2C_TypeDef* i2c, uint8_t slaveAddr, uint8_t* data, uint16_t length);
uint8_t i2cReadByte(I2C_TypeDef* i2c, uint8_t slaveAddr, uint8_t regAddr);
uint16_t i2cReadWord(I2C_TypeDef* i2c, uint8_t slaveAddr, uint8_t regAddr);
