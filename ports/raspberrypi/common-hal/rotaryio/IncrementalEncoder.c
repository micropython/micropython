/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
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

#include "py/runtime.h"

#include <hardware/regs/pio.h>
#include "common-hal/rotaryio/IncrementalEncoder.h"
#include "shared-module/rotaryio/IncrementalEncoder.h"
#include "bindings/rp2pio/__init__.h"
#include "bindings/rp2pio/StateMachine.h"

STATIC const uint16_t encoder[] = {
    //  again:
    //      in pins, 2
    0x4002,
    //      mov x, isr
    0xa026,
    //      jmp x!=y, push_data
    0x00a5,
    //      mov isr, null
    0xa0c3,
    //      jmp again
    0x0000,
    //  push_data:
    //      push
    0x8020,
    //      mov y, x
    0xa041,
};

STATIC const uint16_t encoder_init[] = {
    //      set y, 31
    0xe05f,
};

STATIC void incrementalencoder_interrupt_handler(void *self_in);

void common_hal_rotaryio_incrementalencoder_construct(rotaryio_incrementalencoder_obj_t *self,
    const mcu_pin_obj_t *pin_a, const mcu_pin_obj_t *pin_b) {
    mp_obj_t pins[] = {MP_OBJ_FROM_PTR(pin_a), MP_OBJ_FROM_PTR(pin_b)};
    // Start out with swapped to match behavior with other ports.
    self->swapped = true;
    if (!common_hal_rp2pio_pins_are_sequential(2, pins)) {
        pins[0] = MP_OBJ_FROM_PTR(pin_b);
        pins[1] = MP_OBJ_FROM_PTR(pin_a);
        self->swapped = false;
        if (!common_hal_rp2pio_pins_are_sequential(2, pins)) {
            mp_raise_RuntimeError(translate("Pins must be sequential"));
        }
    }

    self->position = 0;
    self->quarter_count = 0;

    common_hal_rp2pio_statemachine_construct(&self->state_machine,
        encoder, MP_ARRAY_SIZE(encoder),
        1000000,
        encoder_init, MP_ARRAY_SIZE(encoder_init), // init
        NULL, 0, 0, 0, // out pin
        pins[0], 2, // in pins
        3, 0, // in pulls
        NULL, 0, 0, 0x1f, // set pins
        NULL, 0, 0, 0x1f, // sideset pins
        0, // wait gpio pins
        true, // exclusive pin use
        false, 32, false, // out settings
        false, // Wait for txstall
        false, 32, false); // in settings

    common_hal_rp2pio_statemachine_run(&self->state_machine, encoder_init, MP_ARRAY_SIZE(encoder_init));

    // We're guaranteed by the init code that some output will be available promptly
    uint8_t quiescent_state;
    common_hal_rp2pio_statemachine_readinto(&self->state_machine, &quiescent_state, 1, 1);

    shared_module_softencoder_state_init(self, quiescent_state & 3);
    common_hal_rp2pio_statemachine_set_interrupt_handler(&self->state_machine, incrementalencoder_interrupt_handler, self, PIO_IRQ0_INTF_SM0_RXNEMPTY_BITS);
}

bool common_hal_rotaryio_incrementalencoder_deinited(rotaryio_incrementalencoder_obj_t *self) {
    return common_hal_rp2pio_statemachine_deinited(&self->state_machine);
}

void common_hal_rotaryio_incrementalencoder_deinit(rotaryio_incrementalencoder_obj_t *self) {
    if (common_hal_rotaryio_incrementalencoder_deinited(self)) {
        return;
    }
    common_hal_rp2pio_statemachine_set_interrupt_handler(&self->state_machine, NULL, NULL, 0);
    common_hal_rp2pio_statemachine_deinit(&self->state_machine);
}

STATIC void incrementalencoder_interrupt_handler(void *self_in) {
    rotaryio_incrementalencoder_obj_t *self = self_in;

    while (common_hal_rp2pio_statemachine_get_in_waiting(&self->state_machine)) {
        // Bypass all the logic of StateMachine.c:_transfer, we need something
        // very simple and fast for an interrupt!
        uint8_t new_state = self->state_machine.pio->rxf[self->state_machine.state_machine];
        if (self->swapped) {
            if (new_state == 0x1) {
                new_state = 0x2;
            } else if (new_state == 0x2) {
                new_state = 0x1;
            }
        }
        shared_module_softencoder_state_update(self, new_state);
    }
}
