/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017-2018 Scott Shawcroft for Adafruit Industries
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

#include "common-hal/pulseio/PulseIn.h"

#include <stdint.h>

#include "atmel_start_pins.h"
#include "hal/include/hal_gpio.h"

#include "background.h"
#include "eic_handler.h"
#include "mpconfigport.h"
#include "timer_handler.h"
#include "py/gc.h"
#include "py/runtime.h"
#include "samd/external_interrupts.h"
#include "samd/pins.h"
#include "samd/timers.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/pulseio/PulseIn.h"
#include "supervisor/shared/tick.h"
#include "supervisor/shared/translate.h"
#include "supervisor/port.h"

// This timer is shared amongst all PulseIn objects as a higher resolution clock.
static uint8_t refcount = 0;
static uint8_t pulsein_tc_index = 0xff;

volatile static uint32_t overflow_count = 0;
volatile static uint32_t start_overflow = 0;

void pulsein_timer_interrupt_handler(uint8_t index) {
    if (index != pulsein_tc_index) {
        return;
    }
    overflow_count++;
    Tc *tc = tc_insts[index];
    if (!tc->COUNT16.INTFLAG.bit.OVF) {
        return;
    }

    // Clear the interrupt bit.
    tc->COUNT16.INTFLAG.reg = TC_INTFLAG_OVF;
}

static void pulsein_set_config(pulseio_pulsein_obj_t *self, bool first_edge) {
    uint32_t sense_setting;
    if (!first_edge) {
        sense_setting = EIC_CONFIG_SENSE0_BOTH_Val;
        configure_eic_channel(self->channel, sense_setting);
        return;
    } else if (self->idle_state) {
        sense_setting = EIC_CONFIG_SENSE0_FALL_Val;
    } else {
        sense_setting = EIC_CONFIG_SENSE0_RISE_Val;
    }
    set_eic_handler(self->channel, EIC_HANDLER_PULSEIN);
    turn_on_eic_channel(self->channel, sense_setting);
}

void pulsein_interrupt_handler(uint8_t channel) {
    // Turn off interrupts while in handler
    common_hal_mcu_disable_interrupts();
    // Grab the current time first.
    uint32_t current_overflow = overflow_count;
    Tc *tc = tc_insts[pulsein_tc_index];
    #ifdef SAM_D5X_E5X
    tc->COUNT16.CTRLBSET.reg = TC_CTRLBSET_CMD_READSYNC;
    while (tc->COUNT16.SYNCBUSY.bit.COUNT == 1 ||
           tc->COUNT16.CTRLBSET.bit.CMD == TC_CTRLBSET_CMD_READSYNC_Val) {
    }
    #endif
    uint32_t current_count = tc->COUNT16.COUNT.reg;

    pulseio_pulsein_obj_t *self = get_eic_channel_data(channel);
    if (self->len == 0) {
        start_overflow = overflow_count;
    }
    if (self->first_edge) {
        self->first_edge = false;
        pulsein_set_config(self, false);
    } else {
        // Sometimes we beat the overflow interrupt so just fudge overflow in
        // that case.
        if (current_count < self->last_count && current_overflow == self->last_overflow) {
            current_overflow += 1;
        }
        uint32_t total_diff = current_count + 0xffff * (current_overflow - self->last_overflow) - self->last_count;
        // The SAMD21 clock is 48MHz. We prescale it to 3MHz so // 3 here.
        #ifdef SAMD21
        total_diff /= 3;
        #endif
        // Cap duration at 16 bits.
        uint16_t duration = 0xffff;
        if (total_diff < duration) {
            duration = total_diff;
        }
        // check if the input is taking too long, 15 timer overflows is approx 1 second
        if (current_overflow - start_overflow > 15) {
            self->errored_too_fast = true;
            common_hal_pulseio_pulsein_pause(self);
            common_hal_mcu_enable_interrupts();
            return;
        }

        uint16_t i = (self->start + self->len) % self->maxlen;
        self->buffer[i] = duration;
        if (self->len < self->maxlen) {
            self->len++;
        } else {
            self->start++;
        }
    }
    self->last_overflow = current_overflow;
    self->last_count = current_count;
    common_hal_mcu_enable_interrupts();
}

void pulsein_reset() {
    #ifdef SAMD21
    rtc_end_pulse();
    #endif
    refcount = 0;
    pulsein_tc_index = 0xff;
    overflow_count = 0;
}

void common_hal_pulseio_pulsein_construct(pulseio_pulsein_obj_t *self,
    const mcu_pin_obj_t *pin, uint16_t maxlen, bool idle_state) {
    if (!pin->has_extint) {
        mp_raise_RuntimeError(translate("No hardware support on pin"));
    }
    if (eic_get_enable() && !eic_channel_free(pin->extint_channel)) {
        mp_raise_RuntimeError(translate("EXTINT channel already in use"));
    }

    self->buffer = (uint16_t *)m_malloc(maxlen * sizeof(uint16_t), false);
    if (self->buffer == NULL) {
        mp_raise_msg_varg(&mp_type_MemoryError, translate("Failed to allocate RX buffer of %d bytes"), maxlen * sizeof(uint16_t));
    }
    self->channel = pin->extint_channel;
    self->pin = pin->number;
    self->maxlen = maxlen;
    self->idle_state = idle_state;
    self->start = 0;
    self->len = 0;
    self->first_edge = true;
    self->errored_too_fast = false;

    if (refcount == 0) {
        // Find a spare timer.
        Tc *tc = NULL;
        int8_t index = TC_INST_NUM - 1;
        for (; index >= 0; index--) {
            if (tc_insts[index]->COUNT16.CTRLA.bit.ENABLE == 0) {
                tc = tc_insts[index];
                break;
            }
        }
        if (tc == NULL) {
            mp_raise_RuntimeError(translate("All timers in use"));
        }

        pulsein_tc_index = index;

        set_timer_handler(true, index, TC_HANDLER_PULSEIN);
        #ifdef SAMD21
        // We use GCLK0 for SAMD21 which is 48MHz. We prescale it to 3MHz.
        turn_on_clocks(true, index, 0);
        #endif
        #ifdef SAM_D5X_E5X
        // We use GCLK5 for SAMD51 because it runs at 2MHz and we can use it for a 1MHz clock,
        // 1us per tick.
        turn_on_clocks(true, index, 5);
        #endif


        #ifdef SAMD21
        tc->COUNT16.CTRLA.reg = TC_CTRLA_MODE_COUNT16 |
            TC_CTRLA_PRESCALER_DIV16 |
            TC_CTRLA_WAVEGEN_NFRQ;
        #endif
        #ifdef SAM_D5X_E5X
        tc_reset(tc);
        tc_set_enable(tc, false);
        tc->COUNT16.CTRLA.reg = TC_CTRLA_MODE_COUNT16 | TC_CTRLA_PRESCALER_DIV2;
        tc->COUNT16.WAVE.reg = TC_WAVE_WAVEGEN_NFRQ;
        #endif

        tc_set_enable(tc, true);

        // Clear our interrupt in case it was set earlier
        tc->COUNT16.INTFLAG.reg = TC_INTFLAG_OVF;
        tc->COUNT16.INTENSET.reg = TC_INTENSET_OVF;
        tc_enable_interrupts(pulsein_tc_index);
        tc->COUNT16.CTRLBSET.reg = TC_CTRLBSET_CMD_RETRIGGER;

        overflow_count = 0;
    }
    refcount++;
    self->last_overflow = overflow_count;
    self->last_count = 0;

    set_eic_channel_data(pin->extint_channel, (void *)self);

    // Check to see if the EIC is enabled and start it up if its not.'
    if (eic_get_enable() == 0) {
        turn_on_external_interrupt_controller();
    }

    gpio_set_pin_function(pin->number, GPIO_PIN_FUNCTION_A);

    turn_on_cpu_interrupt(self->channel);

    claim_pin(pin);

    // Set config will enable the EIC.
    pulsein_set_config(self, true);
    #ifdef SAMD21
    rtc_start_pulse();
    #endif

}

bool common_hal_pulseio_pulsein_deinited(pulseio_pulsein_obj_t *self) {
    return self->pin == NO_PIN;
}

void common_hal_pulseio_pulsein_deinit(pulseio_pulsein_obj_t *self) {
    if (common_hal_pulseio_pulsein_deinited(self)) {
        return;
    }
    #ifdef SAMD21
    rtc_end_pulse();
    #endif
    set_eic_handler(self->channel, EIC_HANDLER_NO_INTERRUPT);
    turn_off_eic_channel(self->channel);
    reset_pin_number(self->pin);

    refcount--;
    if (refcount == 0) {
        tc_reset(tc_insts[pulsein_tc_index]);
        pulsein_tc_index = 0xff;
    }
    self->pin = NO_PIN;
}

void common_hal_pulseio_pulsein_pause(pulseio_pulsein_obj_t *self) {
    uint32_t mask = 1 << self->channel;
    EIC->INTENCLR.reg = mask << EIC_INTENSET_EXTINT_Pos;
}

void common_hal_pulseio_pulsein_resume(pulseio_pulsein_obj_t *self,
    uint16_t trigger_duration) {
    // Make sure we're paused.
    common_hal_pulseio_pulsein_pause(self);

    // Send the trigger pulse.
    if (trigger_duration > 0) {
        gpio_set_pin_pull_mode(self->pin, GPIO_PULL_OFF);
        gpio_set_pin_direction(self->pin, GPIO_DIRECTION_OUT);
        gpio_set_pin_level(self->pin, !self->idle_state);
        common_hal_mcu_delay_us((uint32_t)trigger_duration);
        gpio_set_pin_level(self->pin, self->idle_state);
    }

    // Reconfigure the pin and make sure its set to detect the first edge.
    self->first_edge = true;
    self->last_overflow = 0;
    self->last_count = 0;
    self->errored_too_fast = false;
    gpio_set_pin_function(self->pin, GPIO_PIN_FUNCTION_A);
    uint32_t mask = 1 << self->channel;
    // Clear previous interrupt state and re-enable it.
    EIC->INTFLAG.reg = mask << EIC_INTFLAG_EXTINT_Pos;
    EIC->INTENSET.reg = mask << EIC_INTENSET_EXTINT_Pos;

    pulsein_set_config(self, true);
}

void common_hal_pulseio_pulsein_clear(pulseio_pulsein_obj_t *self) {
    common_hal_mcu_disable_interrupts();
    self->start = 0;
    self->len = 0;
    common_hal_mcu_enable_interrupts();
}

uint16_t common_hal_pulseio_pulsein_popleft(pulseio_pulsein_obj_t *self) {
    if (self->len == 0) {
        mp_raise_IndexError_varg(translate("pop from empty %q"), MP_QSTR_PulseIn);
    }
    if (self->errored_too_fast) {
        self->errored_too_fast = 0;
        mp_raise_RuntimeError(translate("Input taking too long"));
    }
    common_hal_mcu_disable_interrupts();
    uint16_t value = self->buffer[self->start];
    self->start = (self->start + 1) % self->maxlen;
    self->len--;
    common_hal_mcu_enable_interrupts();
    return value;
}

uint16_t common_hal_pulseio_pulsein_get_maxlen(pulseio_pulsein_obj_t *self) {
    return self->maxlen;
}

uint16_t common_hal_pulseio_pulsein_get_len(pulseio_pulsein_obj_t *self) {
    return self->len;
}

bool common_hal_pulseio_pulsein_get_paused(pulseio_pulsein_obj_t *self) {
    uint32_t mask = 1 << self->channel;
    return (EIC->INTENSET.reg & (mask << EIC_INTENSET_EXTINT_Pos)) == 0;
}

uint16_t common_hal_pulseio_pulsein_get_item(pulseio_pulsein_obj_t *self,
    int16_t index) {
    common_hal_mcu_disable_interrupts();
    if (index < 0) {
        index += self->len;
    }
    if (index < 0 || index >= self->len) {
        common_hal_mcu_enable_interrupts();
        mp_raise_IndexError_varg(translate("%q index out of range"), MP_QSTR_PulseIn);
    }
    uint16_t value = self->buffer[(self->start + index) % self->maxlen];
    common_hal_mcu_enable_interrupts();
    return value;
}
