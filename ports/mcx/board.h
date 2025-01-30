/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 NXP
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

#ifndef MP_PORT_MCX_BOARD_H
#define MP_PORT_MCX_BOARD_H

#include "py/mphal.h"

typedef void (*mcx_board_isr_t)(void *param);

void MCX_BoardEarlyInit(void);

/* UUID */
int MCX_BoardGetUniqueID(uint64_t *uuid);

/* GPIO and Port */
void *MCX_BoardGetPORTInstance(uint8_t id);
void *MCX_BoardGetGPIOInstance(uint8_t id);
int MCX_BoardConfigurePORTClock(uint8_t id);
int MCX_BoardConfigureGPIOClock(uint8_t id);

/* ADC */
void *MCX_BoardGetADCInstance(uint8_t id);
int MCX_BoardConfigureADCClock(uint8_t id);

/* UART */
void *MCX_BoardGetUARTInstance(uint8_t id);
int MCX_BoardConfigureUARTClock(uint8_t id);
int MCX_BoardConfigureUARTISR(uint8_t id, mcx_board_isr_t isr, void *param);

/* I2C */
void *MCX_BoardGetI2CInstance(uint8_t id);
int MCX_BoardConfigureI2CClock(uint8_t id);

/* SPI */
void *MCX_BoardGetSPIInstance(uint8_t id);
int MCX_BoardConfigureSPIClock(uint8_t id);

/* CT32 */
void *MCX_BoardGetCT32Instance(uint8_t id);
int MCX_BoardConfigureCT32Clock(uint8_t id);

/* USB HS */
int MCX_BoardConfigureUSBClock(uint8_t id);
int MCX_BoardConfigureUSBPHY(uint8_t id);
int MCX_BoardConfigureUSBISR(uint8_t id, mcx_board_isr_t isr, void *param);

/* OSTimer */
#ifdef MICROPY_HW_HAS_OSTIMER
void *MCX_BoardGetOSTimerInstance(uint8_t id);
int MCX_BoardConfigureOSTimerClock(uint8_t id);
#endif

#endif
