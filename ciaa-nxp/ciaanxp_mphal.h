/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Ernesto Gigliotti <ernestogigliotti@gmail.com>
 * Copyright (c) 2015 Martin Ribelotta
 * Copyright (c) 2015 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef __MICROPY_INCLUDED_EDUCIAA_MPHAL_H__
#define __MICROPY_INCLUDED_EDUCIAA_MPHAL_H__

#define HAL_GetTick mp_hal_get_milliseconds

#include "py/mpstate.h"

void mp_hal_init(void);

mp_uint_t mp_hal_get_milliseconds(void);
void mp_hal_milli_delay(mp_uint_t ms);

void mp_hal_set_interrupt_char(int c);
//int mp_hal_stdin_rx_chr(void);
//void mp_hal_stdout_tx_str(const char *str);
//void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len);
//void mp_hal_stdout_tx_strn_cooked(const char *str, mp_uint_t len);

// RS232 Functions
uint32_t mp_hal_rs232_write(uint8_t const * const buffer, uint32_t size, uint32_t delay);
void mp_hal_rs232_setRxBuffer(uint8_t* pBuffer,uint32_t size,uint32_t timeout, uint8_t finalByte);
void mp_hal_rs232_setConfig(int32_t baud,int32_t stopBits,int32_t parity);
void mp_hal_rs232_resetRxBufferConfig(void);
int32_t mp_hal_rs232_isNewPacketAvailable(void);
uint32_t mp_hal_rs232_getPacketSize(void);
int32_t mp_hal_rs232_getChar(void);
int32_t mp_hal_rs232_charAvailable(void);
void mp_hal_rs232_resetRxPacket(void);

// RS485 Functions
uint32_t mp_hal_rs485_write(uint8_t const * const buffer, uint32_t size, uint32_t delay);
void mp_hal_rs485_setRxBuffer(uint8_t* pBuffer,uint32_t size,uint32_t timeout, uint8_t finalByte);
void mp_hal_rs485_setConfig(int32_t baud,int32_t stopBits,int32_t parity);
void mp_hal_rs485_resetRxBufferConfig(void);
int32_t mp_hal_rs485_isNewPacketAvailable(void);
uint32_t mp_hal_rs485_getPacketSize(void);
int32_t mp_hal_rs485_getChar(void);
int32_t mp_hal_rs485_charAvailable(void);
void mp_hal_rs485_resetRxPacket(void);

// Buttons
void mp_hal_configureButtonCallback(int buttonNumber,void(*function)(void*),void* arg);
int32_t mp_hal_getButtonState(int32_t btnNumber);

// GPIOs
#define GPIO_MODE_INPUT BOARD_GPIO_MODE_INPUT
#define GPIO_MODE_OUTPUT_PP BOARD_GPIO_MODE_OUTPUT_PP
#define GPIO_MODE_OUTPUT_OD BOARD_GPIO_MODE_OUTPUT_OD
#define GPIO_NOPULL BOARD_GPIO_NOPULL
#define GPIO_PULLUP BOARD_GPIO_PULLUP
#define GPIO_PULLDOWN BOARD_GPIO_PULLDOWN

#define GPIO_MODE_IT_RISING 0
#define GPIO_MODE_IT_FALLING 1
#define GPIO_MODE_IT_RISING_FALLING 2

int32_t  mp_hal_configureGPIOs(int32_t gpioNumber,int32_t mode, int32_t pullup);
int32_t mp_hal_readGPIO(int32_t gpioNumber);
int32_t mp_hal_writeGPIO(int32_t gpioNumber, uint8_t value);
bool mp_hal_enableIntCallbackGPIO(int gpioNumber,void(*function)(void*),void* arg, uint8_t flagEdgeLevel, uint8_t flagHighLow);
void mp_hal_disableIntCallbackGPIO(int gpioNumber);

//DAC
int32_t mp_hal_writeDAC(uint32_t value);
void mp_hal_setSampleRateDAC(uint32_t freq);
int32_t mp_hal_writeDMADAC(uint16_t* buffer, uint32_t size, bool flagCyclic);

//LEDs
void mp_hal_setPwmRGBValue(uint8_t pwmNumber,uint8_t value);
uint8_t mp_hal_getPwmRGBValue(uint8_t pwmNumber);
void mp_hal_setLed(uint8_t ledNumber,uint8_t value);
void mp_hal_toggleLed(uint8_t ledNumber);
bool mp_hal_testLed(uint8_t ledNumber);

//TIMERs
int32_t mp_hal_enableTimerAsTimer(uint8_t timerNum, uint32_t presc,uint32_t matchValue,bool flagOnce);
int32_t mp_hal_disableTimer(uint8_t timerNum);
int32_t mp_hal_setTimerCallback(uint8_t timerNum,void(*function)(void*),void* arg);
uint32_t mp_hal_getTimerClockFrequency(void);
int32_t mp_hal_setTimerCounter(uint8_t timerNum,uint32_t value);
uint32_t mp_hal_getTimerCounter(uint8_t timerNum);
int32_t mp_hal_setTimerPrescaler(uint8_t timerNum,uint32_t value);
uint32_t mp_hal_getTimerPrescaler(uint8_t timerNum);
int32_t mp_hal_setTimerMatch(uint8_t timerNum,uint32_t value);
uint32_t mp_hal_getTimerMatch(uint8_t timerNum);

//PWM
void mp_hal_setPWMFequency(uint32_t freq);
void mp_hal_configurePWMOut(uint8_t outNumber);
void mp_hal_setPWMDutyCycle(uint8_t outNumber, uint8_t duty);

//ADC
int32_t mp_hal_enableADCchannel(uint8_t channelNumber);
void mp_hal_startADCconversion(void);
uint16_t mp_hal_readADCchannel(uint8_t channelNumber);

//Keyboard
void mp_hal_initKeyboard(uint8_t rows, uint8_t columns);
uint8_t mp_hal_readMatrixKeyboard(uint8_t row, uint8_t col);

//LCD
void mp_hal_initLCD(uint8_t lines,uint8_t dotFormat);
void mp_hal_clearLCD(void);
void mp_hal_printStringLCD(const char* text);
void mp_hal_gotoXYLCD(uint8_t x,uint8_t y);
void mp_hal_configCursorLCD(uint8_t onOff,uint8_t blinkOnOff);

//EEPROM
int32_t mp_hal_writeByteEEPROM(uint32_t addr,uint8_t value);
int32_t mp_hal_readByteEEPROM(uint32_t addr);

//SPI
void mp_hal_configSPI(uint8_t bits, uint8_t clockMode, uint32_t bitrate);
uint32_t mp_hal_writeSPI(const uint8_t *buffer, uint32_t bufferLen);
uint32_t mp_hal_readSPI(uint8_t *buffer, uint32_t bufferLen);


//RTC
void mp_hal_initRTC(void);
void mp_hal_setTimeRTC(uint32_t hr,uint32_t min, uint32_t sec, uint32_t day, uint32_t mon, uint32_t yr,uint32_t dayOfWeek);
void mp_hal_getTimeRTC(uint32_t* hr,uint32_t* min, uint32_t* sec, uint32_t* day, uint32_t* mon, uint32_t* yr,uint32_t* dayOfWeek);
void mp_hal_setCalibRTC(uint32_t value);
void mp_hal_writeBkpRegisterRTC(uint8_t address,uint32_t value);
uint32_t mp_hal_readBkpRegisterRTC(uint8_t address);
void mp_hal_setAlarmTimeRTC(uint32_t hr,uint32_t min, uint32_t sec, uint32_t day, uint32_t mon, uint32_t yr,uint32_t dayOfWeek,uint32_t alarmMask);
void mp_hal_getAlarmTimeRTC(uint32_t* hr,uint32_t* min, uint32_t* sec, uint32_t* day, uint32_t* mon, uint32_t* yr,uint32_t* dayOfWeek);
void mp_hal_disableAlarmRTC(void);
void mp_hal_setCallbackRTC(void(*function)(void*),void* arg);

//I2C
void mp_hal_setSpeedI2C(uint32_t speed);
uint32_t mp_hal_masterReadI2C(uint8_t slaveAddr, uint8_t *buff, int len);
uint32_t mp_hal_masterWrite(uint8_t slaveAddr, uint8_t *buff, int len);
uint32_t mp_hal_masterWriteCmdAdnRead(uint8_t slaveAddr, uint8_t cmd,uint8_t *buff, int len);


#endif
