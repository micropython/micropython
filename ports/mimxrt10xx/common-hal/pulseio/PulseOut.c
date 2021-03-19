/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * SPDX-FileCopyrightText: Copyright (c) 2016 Damien P. George
 * Copyright (c) 2019 Artur Pacholec
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

#include "common-hal/pulseio/PulseOut.h"

#include <stdint.h>

#include "mpconfigport.h"
#include "py/gc.h"
#include "py/runtime.h"
#include "shared-bindings/pulseio/PulseOut.h"
#include "supervisor/shared/translate.h"

// TODO

// This timer is shared amongst all PulseOut objects under the assumption that
// the code is single threaded.
// static uint8_t refcount = 0;
//
// static uint8_t pulseout_tc_index = 0xff;
//
// static __IO PORT_PINCFG_Type *active_pincfg = NULL;
// static uint16_t *pulse_buffer = NULL;
// static volatile uint16_t pulse_index = 0;
// static uint16_t pulse_length;
// static volatile uint32_t current_compare = 0;
//
// static void turn_on(__IO PORT_PINCFG_Type * pincfg) {
//    pincfg->reg = PORT_PINCFG_PMUXEN;
// }
//
// static void turn_off(__IO PORT_PINCFG_Type * pincfg) {
//    pincfg->reg = PORT_PINCFG_RESETVALUE;
// }
//
// void pulse_finish(void) {
//    pulse_index++;
//
//    if (active_pincfg == NULL) {
//        return;
//    }
//    // Always turn it off.
//    turn_off(active_pincfg);
//    if (pulse_index >= pulse_length) {
//        return;
//    }
//    current_compare = (current_compare + pulse_buffer[pulse_index] * 3 / 4) & 0xffff;
//    Tc* tc = tc_insts[pulseout_tc_index];
//    tc->COUNT16.CC[0].reg = current_compare;
//    if (pulse_index % 2 == 0) {
//        turn_on(active_pincfg);
//    }
// }

void pulseout_interrupt_handler(uint8_t index) {
//    if (index != pulseout_tc_index) return;
//    Tc* tc = tc_insts[index];
//    if (!tc->COUNT16.INTFLAG.bit.MC0) return;
//
//    pulse_finish();
//
//    // Clear the interrupt bit.
//    tc->COUNT16.INTFLAG.reg = TC_INTFLAG_MC0;
}

void pulseout_reset() {
//    refcount = 0;
//    pulseout_tc_index = 0xff;
//    active_pincfg = NULL;
}

void common_hal_pulseio_pulseout_construct(pulseio_pulseout_obj_t *self,
    const pwmio_pwmout_obj_t *carrier,
    const mcu_pin_obj_t *pin,
    uint32_t frequency,
    uint16_t duty_cycle) {
//    if (refcount == 0) {
//        // Find a spare timer.
//        Tc *tc = NULL;
//        int8_t index = TC_INST_NUM - 1;
//        for (; index >= 0; index--) {
//            if (tc_insts[index]->COUNT16.CTRLA.bit.ENABLE == 0) {
//                tc = tc_insts[index];
//                break;
//            }
//        }
//        if (tc == NULL) {
//            mp_raise_RuntimeError(translate("All timers in use"));
//        }
//
//        pulseout_tc_index = index;
//
//        set_timer_handler(true, index, TC_HANDLER_PULSEOUT);
//        // We use GCLK0 for SAMD21 and GCLK1 for SAMD51 because they both run at 48mhz making our
//        // math the same across the boards.
//        #ifdef SAMD21
//        turn_on_clocks(true, index, 0);
//        #endif
//        #ifdef SAMD51
//        turn_on_clocks(true, index, 1);
//        #endif
//
//
//        #ifdef SAMD21
//        tc->COUNT16.CTRLA.reg = TC_CTRLA_MODE_COUNT16 |
//                                TC_CTRLA_PRESCALER_DIV64 |
//                                TC_CTRLA_WAVEGEN_NFRQ;
//        #endif
//        #ifdef SAMD51
//        tc_reset(tc);
//        tc_set_enable(tc, false);
//        tc->COUNT16.CTRLA.reg = TC_CTRLA_MODE_COUNT16 | TC_CTRLA_PRESCALER_DIV64;
//        tc->COUNT16.WAVE.reg = TC_WAVE_WAVEGEN_NFRQ;
//        #endif
//
//        tc_set_enable(tc, true);
//        tc->COUNT16.CTRLBSET.reg = TC_CTRLBSET_CMD_STOP;
//    }
//    refcount++;
//
//    self->pin = carrier->pin->number;
//
//    PortGroup *const port_base = &PORT->Group[GPIO_PORT(self->pin)];
//    self->pincfg = &port_base->PINCFG[self->pin % 32];
//
//    // Set the port to output a zero.
//    port_base->OUTCLR.reg = 1 << (self->pin % 32);
//    port_base->DIRSET.reg = 1 << (self->pin % 32);
//
//    // Turn off the pinmux which should connect the port output.
//    turn_off(self->pincfg);
}

bool common_hal_pulseio_pulseout_deinited(pulseio_pulseout_obj_t *self) {
//    return self->pin == NO_PIN;
    return false;
}

void common_hal_pulseio_pulseout_deinit(pulseio_pulseout_obj_t *self) {
//    if (common_hal_pulseio_pulseout_deinited(self)) {
//        return;
//    }
//    PortGroup *const port_base = &PORT->Group[GPIO_PORT(self->pin)];
//    port_base->DIRCLR.reg = 1 << (self->pin % 32);
//
//    turn_on(self->pincfg);
//
//    refcount--;
//    if (refcount == 0) {
//        tc_reset(tc_insts[pulseout_tc_index]);
//        pulseout_tc_index = 0xff;
//    }
//    self->pin = NO_PIN;
}

void common_hal_pulseio_pulseout_send(pulseio_pulseout_obj_t *self, uint16_t *pulses, uint16_t length) {
//    if (active_pincfg != NULL) {
//        mp_raise_RuntimeError(translate("Another send is already active"));
//    }
//    active_pincfg = self->pincfg;
//    pulse_buffer = pulses;
//    pulse_index = 0;
//    pulse_length = length;
//
//    current_compare = pulses[0] * 3 / 4;
//    Tc* tc = tc_insts[pulseout_tc_index];
//    tc->COUNT16.CC[0].reg = current_compare;
//
//    // Clear our interrupt in case it was set earlier
//    tc->COUNT16.INTFLAG.reg = TC_INTFLAG_MC0;
//    tc->COUNT16.INTENSET.reg = TC_INTENSET_MC0;
//    tc_enable_interrupts(pulseout_tc_index);
//    turn_on(active_pincfg);
//    tc->COUNT16.CTRLBSET.reg = TC_CTRLBSET_CMD_RETRIGGER;
//
//    while(pulse_index < length) {
//        // Do other things while we wait. The interrupts will handle sending the
//        // signal.
//        RUN_BACKGROUND_TASKS;
//    }
//
//    tc->COUNT16.CTRLBSET.reg = TC_CTRLBSET_CMD_STOP;
//    tc->COUNT16.INTENCLR.reg = TC_INTENCLR_MC0;
//    tc_disable_interrupts(pulseout_tc_index);
//    active_pincfg = NULL;
}
