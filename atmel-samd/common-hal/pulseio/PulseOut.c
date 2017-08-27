/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Damien P. George
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

#include "asf/sam0/drivers/tc/tc_interrupt.h"
#include "asf/sam0/drivers/port/port.h"

#include "mpconfigport.h"
#include "py/gc.h"
#include "py/runtime.h"
#include "shared-bindings/pulseio/PulseOut.h"

#undef ENABLE

// This timer is shared amongst all PulseOut objects under the assumption that
// the code is single threaded. Its stored in MICROPY_PORT_ROOT_POINTERS so it
// doesn't get garbage collected.
static uint8_t refcount = 0;

static __IO PORT_PINCFG_Type *active_pincfg = NULL;
static uint16_t *pulse_buffer = NULL;
static volatile uint16_t pulse_index = 0;
static uint16_t pulse_length;
static volatile uint32_t current_compare = 0;

static void turn_on(__IO PORT_PINCFG_Type * pincfg) {
    pincfg->reg = PORT_PINCFG_PMUXEN;
}

static void turn_off(__IO PORT_PINCFG_Type * pincfg) {
    pincfg->reg = PORT_PINCFG_RESETVALUE;
}

void pulse_finish(struct tc_module *const module) {
    pulse_index++;

    if (active_pincfg == NULL) {
        return;
    }
    // Always turn it off.
    turn_off(active_pincfg);
    if (pulse_index >= pulse_length) {
        return;
    }
    current_compare = (current_compare + pulse_buffer[pulse_index] * 3 / 4) & 0xffff;
    tc_set_compare_value(MP_STATE_VM(pulseout_tc_instance), TC_COMPARE_CAPTURE_CHANNEL_0, current_compare);
    if (pulse_index % 2 == 0) {
        turn_on(active_pincfg);
    }
}

void pulseout_reset() {
    refcount = 0;
    MP_STATE_VM(pulseout_tc_instance) = NULL;
    active_pincfg = NULL;
}

void common_hal_pulseio_pulseout_construct(pulseio_pulseout_obj_t* self,
                                            const pulseio_pwmout_obj_t* carrier) {
    if (refcount == 0) {
        // Find a spare timer.
        Tc *t = NULL;
        Tc *tcs[TC_INST_NUM] = TC_INSTS;
        for (uint8_t i = TC_INST_NUM; i > 0; i--) {
            if (tcs[i - 1]->COUNT16.CTRLA.bit.ENABLE == 0) {
                t = tcs[i - 1];
                break;
            }
        }
        if (t == NULL) {
            mp_raise_RuntimeError("All timers in use");
        }
        MP_STATE_VM(pulseout_tc_instance) = gc_alloc(sizeof(struct tc_module), false);
        if (t == NULL) {
            mp_raise_msg(&mp_type_MemoryError, "");
        }

        struct tc_config config_tc;
        tc_get_config_defaults(&config_tc);

        config_tc.counter_size    = TC_COUNTER_SIZE_16BIT;
        config_tc.clock_prescaler = TC_CTRLA_PRESCALER_DIV64;
        config_tc.wave_generation = TC_WAVE_GENERATION_NORMAL_FREQ;

        tc_init(MP_STATE_VM(pulseout_tc_instance), t, &config_tc);
        tc_register_callback(MP_STATE_VM(pulseout_tc_instance), pulse_finish, TC_CALLBACK_CC_CHANNEL0);
        tc_enable(MP_STATE_VM(pulseout_tc_instance));
        tc_stop_counter(MP_STATE_VM(pulseout_tc_instance));
    }
    refcount++;

    self->pin = carrier->pin->pin;

    PortGroup *const port_base = port_get_group_from_gpio_pin(self->pin);
    self->pincfg = &port_base->PINCFG[self->pin % 32];

    // Set the port to output a zero.
    port_base->OUTCLR.reg = 1 << (self->pin % 32);
    port_base->DIRSET.reg = 1 << (self->pin % 32);

    // Turn off the pinmux which should connect the port output.
    turn_off(self->pincfg);
}

void common_hal_pulseio_pulseout_deinit(pulseio_pulseout_obj_t* self) {
    PortGroup *const port_base = port_get_group_from_gpio_pin(self->pin);
    port_base->DIRCLR.reg = 1 << (self->pin % 32);

    turn_on(self->pincfg);

    refcount--;
    if (refcount == 0) {
        tc_reset(MP_STATE_VM(pulseout_tc_instance));
        gc_free(MP_STATE_VM(pulseout_tc_instance));
        MP_STATE_VM(pulseout_tc_instance) = NULL;
    }
}

void common_hal_pulseio_pulseout_send(pulseio_pulseout_obj_t* self, uint16_t* pulses, uint16_t length) {
    if (active_pincfg != NULL) {
        mp_raise_RuntimeError("Another send is already active");
    }
    active_pincfg = self->pincfg;
    pulse_buffer = pulses;
    pulse_index = 0;
    pulse_length = length;

    current_compare = pulses[0] * 3 / 4;
    tc_set_compare_value(MP_STATE_VM(pulseout_tc_instance), TC_COMPARE_CAPTURE_CHANNEL_0, current_compare);

    tc_enable_callback(MP_STATE_VM(pulseout_tc_instance), TC_CALLBACK_CC_CHANNEL0);
    turn_on(active_pincfg);
    tc_start_counter(MP_STATE_VM(pulseout_tc_instance));

    while(pulse_index < length) {
        // Do other things while we wait. The interrupts will handle sending the
        // signal.
        #ifdef MICROPY_VM_HOOK_LOOP
            MICROPY_VM_HOOK_LOOP
        #endif
    }

    tc_stop_counter(MP_STATE_VM(pulseout_tc_instance));
    tc_disable_callback(MP_STATE_VM(pulseout_tc_instance), TC_CALLBACK_CC_CHANNEL0);
    active_pincfg = NULL;
}
