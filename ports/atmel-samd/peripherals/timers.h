/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
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
#ifndef MICROPY_INCLUDED_ATMEL_SAMD_TIMERS_H
#define MICROPY_INCLUDED_ATMEL_SAMD_TIMERS_H

#include "include/sam.h"

const uint16_t prescaler[8];

#ifdef SAMD21
const uint8_t tcc_cc_num[3];
const uint8_t tc_gclk_ids[TC_INST_NUM];
const uint8_t tcc_gclk_ids[3];
#endif
#ifdef SAMD51
const uint8_t tcc_cc_num[5];
const uint8_t tc_gclk_ids[TC_INST_NUM];
const uint8_t tcc_gclk_ids[TCC_INST_NUM];
#endif
Tc* const tc_insts[TC_INST_NUM];
Tcc* const tcc_insts[TCC_INST_NUM];

void turn_on_clocks(bool is_tc, uint8_t index, uint32_t gclk_index);
void tc_set_enable(Tc* tc, bool enable);
void tcc_set_enable(Tcc* tcc, bool enable);
void tc_wait_for_sync(Tc* tc);
void tc_reset(Tc* tc);

void tc_enable_interrupts(uint8_t tc_index);
void tc_disable_interrupts(uint8_t tc_index);

// Handlers
void TCC0_Handler(void);
void TCC1_Handler(void);
void TCC2_Handler(void);
void TC3_Handler(void);
void TC4_Handler(void);
void TC5_Handler(void);
#ifdef TC6
void TC6_Handler(void);
#endif
#ifdef TC7
void TC7_Handler(void);
#endif

#endif  // MICROPY_INCLUDED_ATMEL_SAMD_TIMERS_H
