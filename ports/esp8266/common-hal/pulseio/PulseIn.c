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

#include <stdint.h>

#include <user_interface.h>
#include <eagle_soc.h>
#include "esp_mphal.h"

#include "mpconfigport.h"
#include "py/gc.h"
#include "py/runtime.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/pulseio/PulseIn.h"
#include "common-hal/microcontroller/__init__.h"

// XXX map gpio pins to pulsein objects: kinda clumsy.
static pulseio_pulsein_obj_t *pulseio_pulsein_objs[GPIO_PIN_COUNT] = {0};

static void pulsein_set_interrupt(pulseio_pulsein_obj_t *self, bool rising, bool falling) {
    ETS_GPIO_INTR_DISABLE();
    // Set interrupt mode
    GPIO_REG_WRITE(
        GPIO_PIN_ADDR(self->pin->gpio_number),
        (GPIO_REG_READ(GPIO_PIN_ADDR(self->pin->gpio_number) & ~GPIO_PIN_INT_TYPE_MASK)) |
        GPIO_PIN_INT_TYPE_SET(
            (rising ? GPIO_PIN_INTR_POSEDGE : 0) | (falling ? GPIO_PIN_INTR_NEGEDGE : 0)
        )
    );
    // Clear interrupt status
    GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, 1 << self->pin->gpio_number);
    ETS_GPIO_INTR_ENABLE();
}

void pulseio_pulsein_interrupt_handler(pulseio_pulsein_obj_t *self, uint32_t time_us) {
    if (self->first_edge) {
        self->first_edge = false;
        pulsein_set_interrupt(self, true, true);
    } else {
        uint16_t elapsed_us = (uint16_t)(time_us - self->last_us);
        uint16_t i = (self->start + self->len) % self->maxlen;
        self->buffer[i] = elapsed_us;
        if (self->len < self->maxlen) {
            self->len++;
        } else {
            self->start++;
        }
    }
    self->last_us = time_us;
}

// XXX needs a better name, or a better abstraction
// XXX called from intr.c:pin_intr_handler_iram ... inelegantly
void pulsein_interrupt_handler(uint32_t status) {
    uint32_t time_us = system_get_time();
    for (int i=0; i<GPIO_PIN_COUNT; i++) {
        if (status & 1<<i) {
            pulseio_pulsein_obj_t *self = pulseio_pulsein_objs[i];
            if (self) pulseio_pulsein_interrupt_handler(self, time_us);
        }
    }
}

void common_hal_pulseio_pulsein_construct(pulseio_pulsein_obj_t* self,
        const mcu_pin_obj_t* pin, uint16_t maxlen, bool idle_state) {
    if (pin->gpio_number == NO_GPIO || pin->gpio_function == SPECIAL_CASE) {
        mp_raise_msg_varg(&mp_type_ValueError, "No PulseIn support for %q", pin->name );
    }
    PIN_FUNC_SELECT(pin->peripheral, pin->gpio_function);
    PIN_PULLUP_DIS(pin->peripheral);
    self->pin = pin;

    self->buffer = (uint16_t *) m_malloc(maxlen * sizeof(uint16_t), false);
    if (self->buffer == NULL) {
        mp_raise_msg_varg(&mp_type_MemoryError, "Failed to allocate RX buffer of %d bytes", maxlen * sizeof(uint16_t));
    }

    self->maxlen = maxlen;
    self->idle_state = idle_state;
    self->start = 0;
    self->len = 0;
    self->first_edge = true;
    self->last_us = 0;
    pulseio_pulsein_objs[self->pin->gpio_number] = self;
    pulsein_set_interrupt(self, !idle_state, idle_state);
}

bool common_hal_pulseio_pulsein_deinited(pulseio_pulsein_obj_t* self) {
    return pulseio_pulsein_objs[self->pin->gpio_number] == NULL;
}

void common_hal_pulseio_pulsein_deinit(pulseio_pulsein_obj_t* self) {
    pulsein_set_interrupt(self, false, false);
    pulseio_pulsein_objs[self->pin->gpio_number] = NULL;
    PIN_FUNC_SELECT(self->pin->peripheral, 0);
    m_free(self->buffer);
}

void common_hal_pulseio_pulsein_pause(pulseio_pulsein_obj_t* self) {
    pulsein_set_interrupt(self, false, false);
}

void common_hal_pulseio_pulsein_resume(pulseio_pulsein_obj_t* self,
        uint16_t trigger_duration) {
    // Make sure we're paused.
    common_hal_pulseio_pulsein_pause(self);

    // Send the trigger pulse.
    if (trigger_duration > 0) {
        uint32_t mask = 1 << self->pin->gpio_number;
        // switch pin to an output with state opposite idle state
        gpio_output_set(self->idle_state ? 0 : mask, self->idle_state ? mask : 0, 0, 0);
        gpio_output_set(0, 0, mask, 0);
        common_hal_mcu_delay_us((uint32_t)trigger_duration);
        // switch pin back to an open input
        gpio_output_set(0, 0, 0, mask);
    }

    common_hal_mcu_disable_interrupts();
    self->first_edge = true;
    pulsein_set_interrupt(self, !self->idle_state, self->idle_state);
    common_hal_mcu_enable_interrupts();
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
