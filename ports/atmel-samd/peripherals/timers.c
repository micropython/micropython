/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
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

#include <stdbool.h>
#include <stdint.h>

#include "timers.h"

#include "common-hal/pulseio/PulseOut.h"

const uint16_t prescaler[8] = {1, 2, 4, 8, 16, 64, 256, 1024};

Tc* const tc_insts[TC_INST_NUM] = TC_INSTS;
Tcc* const tcc_insts[TCC_INST_NUM] = TCC_INSTS;

IRQn_Type const tc_irq[TC_INST_NUM] = {
#ifdef TC0
    TC0_IRQn,
#endif
#ifdef TC1
    TC1_IRQn,
#endif
#ifdef TC2
    TC2_IRQn,
#endif
#ifdef TC3
    TC3_IRQn,
#endif
#ifdef TC4
    TC4_IRQn,
#endif
#ifdef TC5
    TC5_IRQn,
#endif
#ifdef TC6
    TC6_IRQn,
#endif
#ifdef TC7
    TC7_IRQn,
#endif
};

void tc_enable_interrupts(uint8_t tc_index) {
    NVIC_DisableIRQ(tc_irq[tc_index]);
    NVIC_ClearPendingIRQ(tc_irq[tc_index]);
    NVIC_EnableIRQ(tc_irq[tc_index]);
}

void tc_disable_interrupts(uint8_t tc_index) {
    NVIC_DisableIRQ(tc_irq[tc_index]);
    NVIC_ClearPendingIRQ(tc_irq[tc_index]);
}

void tcc_set_enable(Tcc* tcc, bool enable) {
    tcc->CTRLA.bit.ENABLE = enable;
    while (tcc->SYNCBUSY.bit.ENABLE != 0) {
        /* Wait for sync */
    }
}

void tc_reset(Tc* tc) {
    tc->COUNT16.CTRLA.bit.SWRST = 1;
    while (tc->COUNT16.CTRLA.bit.SWRST == 1) {
    }
}

void shared_timer_handler(bool is_tc, uint8_t index) {
    // Add calls to interrupt handlers for specific functionality here.
    if (is_tc) {
        pulseout_interrupt_handler(index);
    }
}

#ifdef SAMD51
#define TC_OFFSET 0
#endif
#ifdef SAMD21
#define TC_OFFSET 3
#endif

void TCC0_Handler(void) {
    shared_timer_handler(false, 0);
}
void TCC1_Handler(void) {
    shared_timer_handler(false, 1);
}
void TCC2_Handler(void) {
    shared_timer_handler(false, 2);
}
// TC0 - TC2 only exist on the SAMD51
#ifdef TC0
void TC0_Handler(void) {
    shared_timer_handler(true, 0);
}
#endif
#ifdef TC1
void TC1_Handler(void) {
    shared_timer_handler(true, 1);
}
#endif
#ifdef TC2
void TC2_Handler(void) {
    shared_timer_handler(true, 2);
}
#endif
void TC3_Handler(void) {
    shared_timer_handler(true, 3 - TC_OFFSET);
}
void TC4_Handler(void) {
    shared_timer_handler(true, 4 - TC_OFFSET);
}
void TC5_Handler(void) {
    shared_timer_handler(true, 5 - TC_OFFSET);
}
#ifdef TC6
void TC6_Handler(void) {
    shared_timer_handler(true, 6 - TC_OFFSET);
}
#endif
#ifdef TC7
void TC7_Handler(void) {
    shared_timer_handler(true, 7 - TC_OFFSET);
}
#endif
