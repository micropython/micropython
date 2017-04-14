/*
 * This file is part of the Micro Python project, http://micropython.org/
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

#include "common-hal/pulseio/PulseIn.h"

#include <stdint.h>

#include "asf/common2/services/delay/delay.h"
#include "asf/sam0/drivers/extint/extint.h"
#include "asf/sam0/drivers/extint/extint_callback.h"
#include "asf/sam0/drivers/port/port.h"

#include "mpconfigport.h"
#include "py/gc.h"
#include "py/runtime.h"
#include "samd21_pins.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/pulseio/PulseIn.h"

#include "tick.h"

static pulseio_pulsein_obj_t *active_pulseins[EIC_NUMBER_OF_INTERRUPTS];
static uint64_t last_ms[EIC_NUMBER_OF_INTERRUPTS];
static uint16_t last_us[EIC_NUMBER_OF_INTERRUPTS];

void pulsein_reset(void) {
    for (int i = 0; i < EIC_NUMBER_OF_INTERRUPTS; i++) {
        if (active_pulseins[i] != NULL) {
            extint_chan_disable_callback(i, EXTINT_CALLBACK_TYPE_DETECT);
        }
        active_pulseins[i] = NULL;
        last_ms[i] = 0;
        last_us[i] = 0;
    }
}

static void pulsein_set_config(pulseio_pulsein_obj_t* self, bool first_edge) {
    struct extint_chan_conf config;
    extint_chan_get_config_defaults(&config);
    config.gpio_pin            = self->pin;
    config.gpio_pin_pull       = EXTINT_PULL_NONE;
    config.filter_input_signal = true;

    if (!first_edge) {
        config.detection_criteria  = EXTINT_DETECT_BOTH;
    } else if (self->idle_state) {
        config.detection_criteria  = EXTINT_DETECT_FALLING;
    } else {
        config.detection_criteria  = EXTINT_DETECT_RISING;
    }
    extint_chan_disable_callback(self->channel, EXTINT_CALLBACK_TYPE_DETECT);
    extint_chan_set_config(self->channel, &config);
    // Clear any interrupts that may have triggered without notifying the CPU.
    EIC->INTFLAG.reg |=  (1UL << self->channel);
    extint_chan_enable_callback(self->channel, EXTINT_CALLBACK_TYPE_DETECT);
}

static void pulsein_callback(void) {
    // Grab the current time first.
    uint16_t current_us = tc_get_count_value(&ms_timer);
    // Add the overflow flag to account for tick interrupts that are blocked by
    // this interrupt.
    uint64_t current_ms = ticks_ms + TC5->COUNT16.INTFLAG.bit.OVF;
    pulseio_pulsein_obj_t* self = active_pulseins[extint_get_current_channel()];
    current_us = current_us * 1000 / self->ticks_per_ms;
    if (self->first_edge) {
        self->first_edge = false;
        pulsein_set_config(self, false);
    } else {
        uint32_t ms_diff = current_ms - last_ms[self->channel];
        uint16_t us_diff = current_us - last_us[self->channel];
        uint32_t total_diff = us_diff;
        if (last_us[self->channel] > current_us) {
            total_diff = 1000 + current_us - last_us[self->channel];
            if (ms_diff > 1) {
                total_diff += (ms_diff - 1) * 1000;
            }
        } else {
            total_diff += ms_diff * 1000;
        }
        uint16_t duration = 0xffff;
        if (total_diff < duration) {
            duration = total_diff;
        }

        uint16_t i = (self->start + self->len) % self->maxlen;
        self->buffer[i] = duration;
        if (self->len < self->maxlen) {
            self->len++;
        } else {
            self->start++;
        }
    }
    last_ms[self->channel] = current_ms;
    last_us[self->channel] = current_us;
}

void common_hal_pulseio_pulsein_construct(pulseio_pulsein_obj_t* self,
        const mcu_pin_obj_t* pin, uint16_t maxlen, bool idle_state) {
    if (!pin->has_extint) {
        mp_raise_RuntimeError("No hardware support on pin");
    }
    // TODO(tannewt): Switch to checking actual extint peripheral state when other
    // classes use extints.
    if (active_pulseins[pin->extint_channel] != NULL) {
        mp_raise_RuntimeError("EXTINT channel already in use");
    }

    self->buffer = (uint16_t *) gc_alloc(maxlen * sizeof(uint16_t), false);
    if (self->buffer == NULL) {
        mp_raise_msg_varg(&mp_type_MemoryError, "Failed to allocate RX buffer of %d bytes", maxlen * sizeof(uint16_t));
    }
    self->channel = pin->extint_channel;
    self->pin = pin->pin;
    self->maxlen = maxlen;
    self->idle_state = idle_state;
    self->start = 0;
    self->len = 0;
    self->first_edge = true;
    self->ticks_per_ms = (system_cpu_clock_get_hz() / 1000 - 1);

    active_pulseins[pin->extint_channel] = self;

    pulsein_set_config(self, true);
    extint_register_callback(
        pulsein_callback,
        self->channel,
        EXTINT_CALLBACK_TYPE_DETECT);
    extint_chan_enable_callback(self->channel, EXTINT_CALLBACK_TYPE_DETECT);
}

void common_hal_pulseio_pulsein_deinit(pulseio_pulsein_obj_t* self) {
    extint_chan_disable_callback(self->channel, EXTINT_CALLBACK_TYPE_DETECT);
    active_pulseins[self->channel] = NULL;
    reset_pin(self->pin);
}

void common_hal_pulseio_pulsein_pause(pulseio_pulsein_obj_t* self) {
    extint_chan_disable_callback(self->channel, EXTINT_CALLBACK_TYPE_DETECT);
}

void common_hal_pulseio_pulsein_resume(pulseio_pulsein_obj_t* self,
        uint16_t trigger_duration) {
    // Send the trigger pulse.
    if (trigger_duration > 0) {
        struct port_config pin_conf;
        port_get_config_defaults(&pin_conf);

        pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
        pin_conf.input_pull = PORT_PIN_PULL_NONE;
        port_pin_set_config(self->pin, &pin_conf);
        port_pin_set_output_level(self->pin, !self->idle_state);
        delay_us(trigger_duration);
        port_pin_set_output_level(self->pin, self->idle_state);
    }

    // Reconfigure the pin and make sure its set to detect the first edge.
    last_ms[self->channel] = 0;
    last_us[self->channel] = 0;
    self->first_edge = true;
    pulsein_set_config(self, true);
}

void common_hal_pulseio_pulsein_clear(pulseio_pulsein_obj_t* self) {
    common_hal_mcu_disable_interrupts();
    self->start = 0;
    self->len = 0;
    common_hal_mcu_enable_interrupts();
}

uint16_t common_hal_pulseio_pulsein_popleft(pulseio_pulsein_obj_t* self) {
    if (self->len == 0) {
        mp_raise_IndexError("pop from an empty PulseIn");
    }
    common_hal_mcu_disable_interrupts();
    uint16_t value = self->buffer[self->start];
    self->start = (self->start + 1) % self->maxlen;
    self->len--;
    common_hal_mcu_enable_interrupts();

    return value;
}

uint16_t common_hal_pulseio_pulsein_get_maxlen(pulseio_pulsein_obj_t* self) {
    return self->maxlen;
}

uint16_t common_hal_pulseio_pulsein_get_len(pulseio_pulsein_obj_t* self) {
    return self->len;
}

uint16_t common_hal_pulseio_pulsein_get_item(pulseio_pulsein_obj_t* self,
        int16_t index) {
    common_hal_mcu_disable_interrupts();
    if (index < 0) {
        index += self->len;
    }
    if (index < 0 || index >= self->len) {
        common_hal_mcu_enable_interrupts();
        mp_raise_IndexError("index out of range");
    }
    uint16_t value = self->buffer[(self->start + index) % self->maxlen];
    common_hal_mcu_enable_interrupts();
    return value;
}
