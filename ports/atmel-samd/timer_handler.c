// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdbool.h>
#include <stdint.h>

#include "timer_handler.h"

#include "samd/clocks.h"
#include "samd/timers.h"

#include "common-hal/pulseio/PulseIn.h"
#include "common-hal/pulseio/PulseOut.h"
#include "common-hal/_pew/PewPew.h"
#include "common-hal/frequencyio/FrequencyIn.h"

extern void _PM_IRQ_HANDLER(void);

static uint8_t tc_handler[TC_INST_NUM];

// Find the APB mask register and bit that clock a timer. turn_on_clocks() sets this bit.
static volatile uint32_t *apb_mask(timer_type_t timer_type, uint8_t index, uint32_t *bit) {
    #ifdef SAM_D5X_E5X
    if (timer_type == TIMER_TYPE_TC) {
        switch (index) {
            case 0:
                *bit = MCLK_APBAMASK_TC0;
                return &MCLK->APBAMASK.reg;
            case 1:
                *bit = MCLK_APBAMASK_TC1;
                return &MCLK->APBAMASK.reg;
            case 2:
                *bit = MCLK_APBBMASK_TC2;
                return &MCLK->APBBMASK.reg;
            case 3:
                *bit = MCLK_APBBMASK_TC3;
                return &MCLK->APBBMASK.reg;
            case 4:
                *bit = MCLK_APBCMASK_TC4;
                return &MCLK->APBCMASK.reg;
            case 5:
                *bit = MCLK_APBCMASK_TC5;
                return &MCLK->APBCMASK.reg;
            case 6:
                *bit = MCLK_APBDMASK_TC6;
                return &MCLK->APBDMASK.reg;
            default:
                *bit = MCLK_APBDMASK_TC7;
                return &MCLK->APBDMASK.reg;
        }
    }
    switch (index) {
        case 0:
            *bit = MCLK_APBBMASK_TCC0;
            return &MCLK->APBBMASK.reg;
        case 1:
            *bit = MCLK_APBBMASK_TCC1;
            return &MCLK->APBBMASK.reg;
        case 2:
            *bit = MCLK_APBCMASK_TCC2;
            return &MCLK->APBCMASK.reg;
        case 3:
            *bit = MCLK_APBCMASK_TCC3;
            return &MCLK->APBCMASK.reg;
        default:
            *bit = MCLK_APBDMASK_TCC4;
            return &MCLK->APBDMASK.reg;
    }
    #else
    // TCC0 is APBC slot 8. TC3, which is TC index 0, is slot 11.
    *bit = 1 << (8 + index + (timer_type == TIMER_TYPE_TC ? 3 : 0));
    return &PM->APBCMASK.reg;
    #endif
}

static bool apb_clock_enabled(timer_type_t timer_type, uint8_t index) {
    uint32_t bit;
    return (*apb_mask(timer_type, index, &bit) & bit) != 0;
}

void turn_off_clocks(timer_type_t timer_type, uint8_t index) {
    uint32_t bit;
    *apb_mask(timer_type, index, &bit) &= ~bit;

    // Most timers share a GCLK peripheral channel with one other timer. Leave the channel on while
    // any timer on it still has its APB clock.
    uint8_t gclk_id = timer_type == TIMER_TYPE_TC ? tc_gclk_ids[index] : tcc_gclk_ids[index];
    for (uint8_t i = 0; i < TC_INST_NUM; i++) {
        if (tc_gclk_ids[i] == gclk_id && apb_clock_enabled(TIMER_TYPE_TC, i)) {
            return;
        }
    }
    for (uint8_t i = 0; i < TCC_INST_NUM; i++) {
        if (tcc_gclk_ids[i] == gclk_id && apb_clock_enabled(TIMER_TYPE_TCC, i)) {
            return;
        }
    }
    disconnect_gclk_from_peripheral(0, gclk_id);
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
