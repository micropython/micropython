/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017-2018 Scott Shawcroft for Adafruit Industries
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

#include "common-hal/pulseio/PulseIn.h"

#include <stdint.h>

#include "background.h"
#include "mpconfigport.h"
#include "py/gc.h"
#include "py/runtime.h"

#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/pulseio/PulseIn.h"
#include "supervisor/shared/translate.h"

// TODO
// static void pulsein_set_config(pulseio_pulsein_obj_t* self, bool first_edge) {
//    uint32_t sense_setting;
//    if (!first_edge) {
//        sense_setting = EIC_CONFIG_SENSE0_BOTH_Val;
//        configure_eic_channel(self->channel, sense_setting);
//        return;
//    } else if (self->idle_state) {
//        sense_setting = EIC_CONFIG_SENSE0_FALL_Val;
//    } else {
//        sense_setting = EIC_CONFIG_SENSE0_RISE_Val;
//    }
//    set_eic_handler(self->channel, EIC_HANDLER_PULSEIN);
//    turn_on_eic_channel(self->channel, sense_setting);
// }

// void pulsein_interrupt_handler(uint8_t channel) {
//    // Grab the current time first.
//    uint32_t current_us;
//    uint64_t current_ms;
//    current_tick(&current_ms, &current_us);
//
//    // current_tick gives us the remaining us until the next tick but we want the number since the
//    // last ms.
//    current_us = 1000 - current_us;
//    pulseio_pulsein_obj_t* self = get_eic_channel_data(channel);
//    if (!supervisor_background_tasks_ok() || self->errored_too_fast) {
//        self->errored_too_fast = true;
//        common_hal_pulseio_pulsein_pause(self);
//        return;
//    }
//    if (self->first_edge) {
//        self->first_edge = false;
//        pulsein_set_config(self, false);
//    } else {
//        uint32_t ms_diff = current_ms - self->last_ms;
//        uint16_t us_diff = current_us - self->last_us;
//        uint32_t total_diff = us_diff;
//        if (self->last_us > current_us) {
//            total_diff = 1000 + current_us - self->last_us;
//            if (ms_diff > 1) {
//                total_diff += (ms_diff - 1) * 1000;
//            }
//        } else {
//            total_diff += ms_diff * 1000;
//        }
//        uint16_t duration = 0xffff;
//        if (total_diff < duration) {
//            duration = total_diff;
//        }
//
//        uint16_t i = (self->start + self->len) % self->maxlen;
//        self->buffer[i] = duration;
//        if (self->len < self->maxlen) {
//            self->len++;
//        } else {
//            self->start++;
//        }
//    }
//    self->last_ms = current_ms;
//    self->last_us = current_us;
// }

void common_hal_pulseio_pulsein_construct(pulseio_pulsein_obj_t *self,
    const mcu_pin_obj_t *pin, uint16_t maxlen, bool idle_state) {
//    if (!pin->has_extint) {
//        mp_raise_RuntimeError(translate("No hardware support on pin"));
//    }
//    if (eic_get_enable() && !eic_channel_free(pin->extint_channel)) {
//        mp_raise_RuntimeError(translate("EXTINT channel already in use"));
//    }
//
//    self->buffer = (uint16_t *) m_malloc(maxlen * sizeof(uint16_t), false);
//    if (self->buffer == NULL) {
//        mp_raise_msg_varg(&mp_type_MemoryError, translate("Failed to allocate RX buffer of %d bytes"), maxlen * sizeof(uint16_t));
//    }
//    self->channel = pin->extint_channel;
//    self->pin = pin->number;
//    self->maxlen = maxlen;
//    self->idle_state = idle_state;
//    self->start = 0;
//    self->len = 0;
//    self->first_edge = true;
//    self->last_us = 0;
//    self->last_ms = 0;
//    self->errored_too_fast = false;
//
//    set_eic_channel_data(pin->extint_channel, (void*) self);
//
//    // Check to see if the EIC is enabled and start it up if its not.'
//    if (eic_get_enable() == 0) {
//        turn_on_external_interrupt_controller();
//    }
//
//    gpio_set_pin_function(pin->number, GPIO_PIN_FUNCTION_A);
//
//    turn_on_cpu_interrupt(self->channel);
//
//    claim_pin(pin);
//
//    // Set config will enable the EIC.
//    pulsein_set_config(self, true);
}

bool common_hal_pulseio_pulsein_deinited(pulseio_pulsein_obj_t *self) {
//    return self->pin == NO_PIN;
    return true;
}

void common_hal_pulseio_pulsein_deinit(pulseio_pulsein_obj_t *self) {
//    if (common_hal_pulseio_pulsein_deinited(self)) {
//        return;
//    }
//    set_eic_handler(self->channel, EIC_HANDLER_NO_INTERRUPT);
//    turn_off_eic_channel(self->channel);
//    reset_pin_number(self->pin);
//    self->pin = NO_PIN;
}

void common_hal_pulseio_pulsein_pause(pulseio_pulsein_obj_t *self) {
//    uint32_t mask = 1 << self->channel;
//    EIC->INTENCLR.reg = mask << EIC_INTENSET_EXTINT_Pos;
}

void common_hal_pulseio_pulsein_resume(pulseio_pulsein_obj_t *self,
    uint16_t trigger_duration) {
//    // Make sure we're paused.
//    common_hal_pulseio_pulsein_pause(self);
//
//    // Reset erroring
//    self->errored_too_fast = false;
//
//    // Send the trigger pulse.
//    if (trigger_duration > 0) {
//        gpio_set_pin_pull_mode(self->pin, GPIO_PULL_OFF);
//        gpio_set_pin_direction(self->pin, GPIO_DIRECTION_OUT);
//        gpio_set_pin_level(self->pin, !self->idle_state);
//        common_hal_mcu_delay_us((uint32_t)trigger_duration);
//        gpio_set_pin_level(self->pin, self->idle_state);
//    }
//
//    // Reconfigure the pin and make sure its set to detect the first edge.
//    self->first_edge = true;
//    self->last_ms = 0;
//    self->last_us = 0;
//    gpio_set_pin_function(self->pin, GPIO_PIN_FUNCTION_A);
//    uint32_t mask = 1 << self->channel;
//    // Clear previous interrupt state and re-enable it.
//    EIC->INTFLAG.reg = mask << EIC_INTFLAG_EXTINT_Pos;
//    EIC->INTENSET.reg = mask << EIC_INTENSET_EXTINT_Pos;
//
//    pulsein_set_config(self, true);
}

void common_hal_pulseio_pulsein_clear(pulseio_pulsein_obj_t *self) {
//    common_hal_mcu_disable_interrupts();
//    self->start = 0;
//    self->len = 0;
//    common_hal_mcu_enable_interrupts();
}

uint16_t common_hal_pulseio_pulsein_popleft(pulseio_pulsein_obj_t *self) {
//    if (self->len == 0) {
//        mp_raise_IndexError_varg(translate("pop from empty %q"), MP_QSTR_PulseIn);
//    }
//    common_hal_mcu_disable_interrupts();
//    uint16_t value = self->buffer[self->start];
//    self->start = (self->start + 1) % self->maxlen;
//    self->len--;
//    common_hal_mcu_enable_interrupts();
//
//    return value;
    return 0;
}

uint16_t common_hal_pulseio_pulsein_get_maxlen(pulseio_pulsein_obj_t *self) {
//    return self->maxlen;
    return 0;
}

uint16_t common_hal_pulseio_pulsein_get_len(pulseio_pulsein_obj_t *self) {
//    return self->len;
    return 0;
}

bool common_hal_pulseio_pulsein_get_paused(pulseio_pulsein_obj_t *self) {
//    uint32_t mask = 1 << self->channel;
//    return (EIC->INTENSET.reg & (mask << EIC_INTENSET_EXTINT_Pos)) == 0;
    return true;
}

uint16_t common_hal_pulseio_pulsein_get_item(pulseio_pulsein_obj_t *self,
    int16_t index) {
//    common_hal_mcu_disable_interrupts();
//    if (index < 0) {
//        index += self->len;
//    }
//    if (index < 0 || index >= self->len) {
//        common_hal_mcu_enable_interrupts();
//        mp_raise_IndexError_varg(translate("%q index out of range"), MP_QSTR_PulseIn);
//    }
//    uint16_t value = self->buffer[(self->start + index) % self->maxlen];
//    common_hal_mcu_enable_interrupts();
//    return value;
    return 0;
}
