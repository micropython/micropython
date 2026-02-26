/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Robert Hammelrath
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

#include <string.h>
#include "py/mpconfig.h"
#include "sam.h"
#include "tc_manager.h"

#if MICROPY_HW_TC_MANAGER

// List of channel flags: true: channel used, false: channel available
// Two Tc instances are used by the usec counter and cannot be assigned.
#if defined(MCU_SAMD21)
static bool instance_flag[TC_INST_NUM] = {false, true, true};
#elif defined(MCU_SAMD51)
static bool instance_flag[TC_INST_NUM] = {true, true};
#endif
Tc *tc_instance_list[TC_INST_NUM] = TC_INSTS;
extern const uint16_t prescaler_table[];

// allocate_tc_instance(): retrieve an available instance. Return the pointer or NULL
int allocate_tc_instance(void) {
    for (int i = 0; i < MP_ARRAY_SIZE(instance_flag); i++) {
        if (instance_flag[i] == false) { // available
            instance_flag[i] = true;
            return i;
        }
    }
    mp_raise_ValueError(MP_ERROR_TEXT("no Timer available"));
}

// free_tc_instance(n): Declare instance as free
void free_tc_instance(int tc_index) {
    if (tc_index >= 0 && tc_index < MP_ARRAY_SIZE(instance_flag)) {
        instance_flag[tc_index] = false;
    }
}

int configure_tc(int tc_index, int freq, int event) {
    uint32_t clock = DFLL48M_FREQ;  // Use the fixed 48M clock

    Tc *tc;
    if (tc_index < MP_ARRAY_SIZE(instance_flag)) {
        tc = tc_instance_list[tc_index];
    } else {
        return -1;
    }
    // Check for the right prescaler
    uint8_t index;
    uint32_t period;

    for (index = 0; index < 8; index++) {
        period = clock / prescaler_table[index] / freq;
        if (period < (1 << 16)) {
            break;
        }
    }

    #if defined(MCU_SAMD21)

    // Set up the clocks
    if (tc == TC3) {
        PM->APBCMASK.bit.TC3_ = 1; // Enable TC3 clock
        GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK5 | GCLK_CLKCTRL_ID_TCC2_TC3;
    #if TC_INST_NUM > 3
    } else {
        if (tc == TC6) {
            PM->APBCMASK.bit.TC6_ = 1; // Enable TC6 clock
        } else if (tc == TC7) {
            PM->APBCMASK.bit.TC7_ = 1; // Enable TC7 clock
        }
        // Select multiplexer generic clock source and enable.
        GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK5 | GCLK_CLKCTRL_ID_TC6_TC7;
    #endif // TC_INST_NUM > 3
    }
    // Wait while it updates synchronously.
    while (GCLK->STATUS.bit.SYNCBUSY) {
    }
    // Configure the timer.
    tc->COUNT16.CTRLA.reg = TC_CTRLA_SWRST;
    while (tc->COUNT16.CTRLA.bit.SWRST || tc->COUNT16.STATUS.bit.SYNCBUSY) {
    }
    tc->COUNT16.CTRLA.reg = TC_CTRLA_PRESCALER(index) |
        TC_CTRLA_MODE_COUNT16 | TC_CTRLA_RUNSTDBY |
        TC_CTRLA_WAVEGEN_MFRQ;
    tc->COUNT16.CC[0].reg = period;
    if (event) {
        tc->COUNT16.EVCTRL.reg = event;
    }

    tc->COUNT16.CTRLA.bit.ENABLE = 1;
    while (tc->COUNT16.STATUS.bit.SYNCBUSY) {
    }

    #elif defined(MCU_SAMD51)

    int gclk_id = TC2_GCLK_ID;
    // Enable MCLK
    switch (tc_index) {
        case 2:
            MCLK->APBBMASK.bit.TC2_ = 1; // Enable TC2 clock
            gclk_id = TC2_GCLK_ID;
            break;
        case 3:
            MCLK->APBBMASK.bit.TC3_ = 1; // Enable TC3 clock
            gclk_id = TC3_GCLK_ID;
            break;
        #if TC_INST_NUM > 4
        case 4:
            MCLK->APBCMASK.bit.TC4_ = 1; // Enable TC4 clock
            gclk_id = TC4_GCLK_ID;
            break;
        case 5:
            MCLK->APBCMASK.bit.TC5_ = 1; // Enable TC5 clock
            gclk_id = TC5_GCLK_ID;
            break;
        #if TC_INST_NUM > 6
        case 6:
            MCLK->APBDMASK.bit.TC6_ = 1; // Enable TC6 clock
            gclk_id = TC6_GCLK_ID;
            break;
        case 7:
            MCLK->APBDMASK.bit.TC7_ = 1; // Enable TC7 clock
            gclk_id = TC7_GCLK_ID;
            break;
        #endif // TC_INST_NUM > 6
        #endif // TC_INST_NUM > 4
    }
    // Enable the 48Mhz clock.
    GCLK->PCHCTRL[gclk_id].reg = GCLK_PCHCTRL_GEN_GCLK5 | GCLK_PCHCTRL_CHEN;
    while (GCLK->PCHCTRL[gclk_id].bit.CHEN == 0) {
    }
    // Configure the timer.
    tc->COUNT16.CTRLA.reg = TC_CTRLA_SWRST;
    while (tc->COUNT16.SYNCBUSY.bit.SWRST) {
    }
    tc->COUNT16.CTRLA.reg = TC_CTRLA_PRESCALER(index) |
        TC_CTRLA_MODE_COUNT16 | TC_CTRLA_RUNSTDBY | TC_CTRLA_PRESCSYNC_PRESC;
    tc->COUNT16.WAVE.reg = TC_WAVE_WAVEGEN_MFRQ;
    tc->COUNT16.CC[0].reg = period;
    tc->COUNT16.CTRLA.bit.ENABLE = 1;
    while (tc->COUNT16.SYNCBUSY.bit.ENABLE) {
    }

    #endif  // SAMD21 or SAMD51

    return 0;
}

void tc_deinit(void) {
    memset((uint8_t *)instance_flag, 0, sizeof(instance_flag));
    // The tc instances used by the us counter have to be locked.
    // That's TC4 and TC5 for SAMD21 with the list starting at TC3
    // and TC0 and TC1 for SAMD51, with the list starting at TC0
    #if defined(MCU_SAMD21)
    instance_flag[1] = instance_flag[2] = true;
    #elif defined(MCU_SAMD51)
    instance_flag[0] = instance_flag[1] = true;
    #endif
}

#endif
