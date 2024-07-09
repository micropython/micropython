// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Dave Putz for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "src/rp2_common/hardware_gpio/include/hardware/gpio.h"

#include <stdint.h>

#include "py/runtime.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/pulseio/PulseIn.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "bindings/rp2pio/StateMachine.h"
#include "common-hal/pulseio/PulseIn.h"

#define NO_PIN 0xff
#define MAX_PULSE 65535
#define MIN_PULSE 0

static const uint16_t pulsein_program[] = {
    0x4001, //  1: in     pins, 1
};

void common_hal_pulseio_pulsein_construct(pulseio_pulsein_obj_t *self,
    const mcu_pin_obj_t *pin, uint16_t maxlen, bool idle_state) {

    self->buffer = (uint16_t *)m_malloc(maxlen * sizeof(uint16_t));
    if (self->buffer == NULL) {
        m_malloc_fail(maxlen * sizeof(uint16_t));
    }
    self->pin = pin->number;
    self->maxlen = maxlen;
    self->idle_state = idle_state;
    self->start = 0;
    self->len = 0;

    common_hal_rp2pio_statemachine_construct(&self->state_machine,
        pulsein_program, MP_ARRAY_SIZE(pulsein_program),
        1000000, // frequency
        NULL, 0, // init, init_len
        NULL, 0, // may_exec
        NULL, 0, 0, 0, // first out pin, # out pins, initial_out_pin_state
        pin, 1, 0, 0, // first in pin, # in pins
        NULL, 0, 0, 0, // first set pin
        NULL, 0, 0, 0, // first sideset pin
        false, // No sideset enable
        NULL, PULL_NONE, // jump pin, jmp_pull
        0, // wait gpio pins
        true, // exclusive pin usage
        false, 8, false, // TX, setting we don't use
        false, // wait for TX stall
        true, 32, true, // RX auto pull every 32 bits. shift left to output msb first
        false, // Not user-interruptible.
        0, -1, // wrap settings
        PIO_ANY_OFFSET);

    common_hal_pulseio_pulsein_pause(self);

    common_hal_rp2pio_statemachine_set_interrupt_handler(&(self->state_machine), &common_hal_pulseio_pulsein_interrupt, self, PIO_IRQ0_INTE_SM0_RXNEMPTY_BITS);

    common_hal_pulseio_pulsein_resume(self, 0);
}

bool common_hal_pulseio_pulsein_deinited(pulseio_pulsein_obj_t *self) {
    return self->pin == NO_PIN;
}

void common_hal_pulseio_pulsein_deinit(pulseio_pulsein_obj_t *self) {
    if (common_hal_pulseio_pulsein_deinited(self)) {
        return;
    }
    pio_sm_set_enabled(self->state_machine.pio, self->state_machine.state_machine, false);
    common_hal_rp2pio_statemachine_deinit(&self->state_machine);
    m_free(self->buffer);
    reset_pin_number(self->pin);
    self->pin = NO_PIN;
}

void common_hal_pulseio_pulsein_pause(pulseio_pulsein_obj_t *self) {
    pio_sm_restart(self->state_machine.pio, self->state_machine.state_machine);
    pio_sm_set_enabled(self->state_machine.pio, self->state_machine.state_machine, false);
    pio_sm_clear_fifos(self->state_machine.pio, self->state_machine.state_machine);
    self->last_level = self->idle_state;
    self->level_count = 0;
    self->paused = true;
}
void common_hal_pulseio_pulsein_interrupt(void *self_in) {
    pulseio_pulsein_obj_t *self = self_in;

    uint32_t rxfifo = 0;

    rxfifo = pio_sm_get_blocking(self->state_machine.pio, self->state_machine.state_machine);
    // translate from fifo to buffer
    if ((rxfifo == 0 && self->last_level == false) || (rxfifo == 0xffffffff && self->last_level == true)) {
        self->level_count = self->level_count + 32;
    } else {
        for (uint i = 0; i < 32; i++) {
            bool level = (rxfifo & (1 << i)) >> i;
            if (level == self->last_level) {
                self->level_count++;
            } else {
                uint32_t result = self->level_count;
                self->last_level = level;
                self->level_count = 1;
                // Pulses that are longer than MAX_PULSE will return MAX_PULSE
                if (result > MAX_PULSE) {
                    result = MAX_PULSE;
                }
                // return  pulses that are not too short
                if (result > MIN_PULSE) {
                    size_t buf_index = (self->start + self->len) % self->maxlen;
                    self->buffer[buf_index] = (uint16_t)result;
                    if (self->len < self->maxlen) {
                        self->len++;
                    } else {
                        self->start = (self->start + 1) % self->maxlen;
                    }
                }
            }
        }
    }
}
void common_hal_pulseio_pulsein_resume(pulseio_pulsein_obj_t *self,
    uint16_t trigger_duration) {

    common_hal_pulseio_pulsein_pause(self);
    // Send the trigger pulse.
    if (trigger_duration > 0) {
        gpio_set_function(self->pin, GPIO_FUNC_SIO);
        gpio_set_dir(self->pin, true);
        gpio_put(self->pin, !self->idle_state);
        common_hal_mcu_delay_us((uint32_t)trigger_duration);
        gpio_set_function(self->pin, GPIO_FUNC_PIO0);
    }

    // exec a wait for the selected pin to change state
    if (self->idle_state == true) {
        pio_sm_exec(self->state_machine.pio, self->state_machine.state_machine, 0x2020);
    } else {
        pio_sm_exec(self->state_machine.pio, self->state_machine.state_machine, 0x20a0);
    }
    pio_sm_set_enabled(self->state_machine.pio, self->state_machine.state_machine, true);
    self->paused = false;
}

void common_hal_pulseio_pulsein_clear(pulseio_pulsein_obj_t *self) {
    self->len = 0;
}

uint16_t common_hal_pulseio_pulsein_popleft(pulseio_pulsein_obj_t *self) {
    if (self->len == 0) {
        mp_raise_IndexError_varg(MP_ERROR_TEXT("pop from empty %q"), MP_QSTR_PulseIn);
    }
    uint16_t value = self->buffer[self->start];
    common_hal_mcu_disable_interrupts();
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
    return self->paused;
}

uint16_t common_hal_pulseio_pulsein_get_item(pulseio_pulsein_obj_t *self,
    int16_t index) {
    if (index < 0) {
        index += self->len;
    }
    if (index < 0 || index >= self->len) {
        mp_arg_validate_index_range(index, 0, self->len, MP_QSTR_index);
    }
    uint16_t value = self->buffer[(self->start + index) % self->maxlen];
    return value;
}
