/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
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
int mp_hal_stdin_rx_chr(void);
void mp_hal_stdout_tx_str(const char *str);
void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len);
void mp_hal_stdout_tx_strn_cooked(const char *str, mp_uint_t len);

// RS232 Functions
uint32_t mp_hal_rs232_write(uint8_t const * const buffer, uint32_t size, uint32_t delay);
void mp_hal_rs232_setRxBuffer(uint8_t* pBuffer,uint32_t size,uint32_t timeout, uint8_t finalByte);
void mp_hal_rs232_setConfig(int32_t baud,int32_t stopBits,int32_t parity);
int32_t mp_hal_rs232_isNewPacketAvailable(void);
uint32_t mp_hal_rs232_getPacketSize(void);
int32_t mp_hal_rs232_getChar(void);
int32_t mp_hal_rs232_charAvailable(void);
void mp_hal_rs232_resetRxPacket(void);

// RS485 Functions
uint32_t mp_hal_rs485_write(uint8_t const * const buffer, uint32_t size, uint32_t delay);
void mp_hal_rs485_setRxBuffer(uint8_t* pBuffer,uint32_t size,uint32_t timeout, uint8_t finalByte);
void mp_hal_rs485_setConfig(int32_t baud,int32_t stopBits,int32_t parity);
int32_t mp_hal_rs485_isNewPacketAvailable(void);
uint32_t mp_hal_rs485_getPacketSize(void);
int32_t mp_hal_rs485_getChar(void);
int32_t mp_hal_rs485_charAvailable(void);
void mp_hal_rs485_resetRxPacket(void);

// Buttons
void mp_hal_configureButtonCallback(int buttonNumber,void(*function)(void*),void* arg);

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

void mp_hal_configureGPIOs(int32_t gpioNumber,int32_t mode, int32_t pullup);
int32_t mp_hal_readGPIO(int32_t gpioNumber);
void mp_hal_writeGPIO(int32_t gpioNumber, uint8_t value);
bool mp_hal_enableIntCallbackGPIO(int gpioNumber,void(*function)(void*),void* arg, uint8_t flagEdgeLevel, uint8_t flagHighLow);
void mp_hal_disableIntCallbackGPIO(int gpioNumber);


#endif
