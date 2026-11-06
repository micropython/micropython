// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdbool.h>
#include <stdint.h>

#include "timer_handler.h"

#include "samd/timers.h"
#ifndef SAM_D5X_E5X
#include "hpl/gclk/hpl_gclk_base.h"
#include "hpl/pm/hpl_pm_base.h"
#endif

#include "common-hal/pulseio/PulseIn.h"
#include "common-hal/pulseio/PulseOut.h"
#include "common-hal/_pew/PewPew.h"
#include "common-hal/frequencyio/FrequencyIn.h"

extern void _PM_IRQ_HANDLER(void);

static uint8_t tc_handler[TC_INST_NUM];

extern const uint8_t tc_gclk_ids[TC_INST_NUM];
extern const uint8_t tcc_gclk_ids[TCC_INST_NUM];

void turn_off_clocks(bool is_tc, uint8_t index, uint32_t gclk_index) {
    uint8_t gclk_id;
    if (is_tc) {
        gclk_id = tc_gclk_ids[index];
    } else {
        gclk_id = tcc_gclk_ids[index];
    }
    #ifdef SAM_D5X_E5X
    // Keep the generator selection but disable the peripheral clock channel.
    GCLK->PCHCTRL[gclk_id].reg = gclk_index;
    if (is_tc) {
        switch (index) {
            case 0:
                MCLK->APBAMASK.reg &= ~MCLK_APBAMASK_TC0;
                break;
            case 1:
                MCLK->APBAMASK.reg &= ~MCLK_APBAMASK_TC1;
                break;
            case 2:
                MCLK->APBBMASK.reg &= ~MCLK_APBBMASK_TC2;
                break;
            case 3:
                MCLK->APBBMASK.reg &= ~MCLK_APBBMASK_TC3;
                break;
            case 4:
                MCLK->APBCMASK.reg &= ~MCLK_APBCMASK_TC4;
                break;
            case 5:
                MCLK->APBCMASK.reg &= ~MCLK_APBCMASK_TC5;
                break;
            case 6:
                MCLK->APBDMASK.reg &= ~MCLK_APBDMASK_TC6;
                break;
            case 7:
                MCLK->APBDMASK.reg &= ~MCLK_APBDMASK_TC7;
                break;
            default:
                break;
        }
    } else {
        switch (index) {
            case 0:
                MCLK->APBBMASK.reg &= ~MCLK_APBBMASK_TCC0;
                break;
            case 1:
                MCLK->APBBMASK.reg &= ~MCLK_APBBMASK_TCC1;
                break;
            case 2:
                MCLK->APBCMASK.reg &= ~MCLK_APBCMASK_TCC2;
                break;
            case 3:
                MCLK->APBCMASK.reg &= ~MCLK_APBCMASK_TCC3;
                break;
            case 4:
                MCLK->APBDMASK.reg &= ~MCLK_APBDMASK_TCC4;
                break;
            default:
                break;
        }
    }
    #else
    // Disable the peripheral clock channel (this write also clears CLKEN),
    // then the APB clock slot.
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(gclk_id) | GCLK_CLKCTRL_GEN(gclk_index);
    while (GCLK->STATUS.bit.SYNCBUSY != 0) {
    }
    // Determine the clock slot on the APBC bus. TCC0 is the first and 8 slots in.
    uint8_t clock_slot = 8 + index;
    // We index TCs starting at zero but in memory they begin at three so we have to add three.
    if (is_tc) {
        clock_slot += 3;
    }
    PM->APBCMASK.reg &= ~(1 << clock_slot);
    #endif
}

void set_timer_handler(bool is_tc, uint8_t index, uint8_t timer_handler) {
    if (is_tc) {
        tc_handler[index] = timer_handler;
    }
}

void shared_timer_handler(bool is_tc, uint8_t index) {
    // Add calls to interrupt handlers for specific functionality here.
    // Make sure to add the handler #define to timer_handler.h
    if (is_tc) {
        uint8_t handler = tc_handler[index];
        switch (handler) {
            case TC_HANDLER_PULSEIN:
                #if CIRCUITPY_PULSEIO
                pulsein_timer_interrupt_handler(index);
                #endif
                break;
            case TC_HANDLER_PULSEOUT:
                #if CIRCUITPY_PULSEIO
                pulseout_interrupt_handler(index);
                #endif
                break;
            case TC_HANDLER_PEW:
                #if CIRCUITPY_PEW
                pewpew_interrupt_handler(index);
                #endif
                break;
            case TC_HANDLER_FREQUENCYIN:
                #if CIRCUITPY_FREQUENCYIO
                frequencyin_interrupt_handler(index);
                #endif
                break;
            case TC_HANDLER_RGBMATRIX:
                #if CIRCUITPY_RGBMATRIX
                _PM_IRQ_HANDLER();
                #endif
                break;
            default:
                break;
        }
    }
}
