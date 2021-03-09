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

void common_hal_rotaryio_incrementalencoder_construct(rotaryio_incrementalencoder_obj_t* self,
    const mcu_pin_obj_t* pin_a, const mcu_pin_obj_t* pin_b) {
    mp_obj_t pins[] = {MP_OBJ_FROM_PTR(pin_a), MP_OBJ_FROM_PTR(pin_b)};
    if (!common_hal_rp2pio_pins_are_sequential(2, pins)) {
        mp_raise_RuntimeError(translate("Pins must be sequential"));
    }

    self->position = 0;
    self->quarter_count = 0;

    common_hal_rp2pio_statemachine_construct(&self->state_machine,
        encoder, MP_ARRAY_SIZE(encoder),
        1000000,
        encoder_init, MP_ARRAY_SIZE(encoder_init), // init
        NULL, 1, 0, 0xffffffff, // out pin
        pin_a, 2, // in pins
        3, 0, // in pulls
        NULL, 0, 0, 0x1f, // set pins
        NULL, 0, 0, 0x1f, // sideset pins
        true, // exclusive pin use
        false, 32, false, // out settings
        false, // Wait for txstall
        false, 32, false); // in settings

    common_hal_rp2pio_statemachine_run(&self->state_machine, encoder_init, MP_ARRAY_SIZE(encoder_init));

    // We're guaranteed by the init code that some output will be available promptly
    uint8_t state;
    common_hal_rp2pio_statemachine_readinto(&self->state_machine, &state, 1, 1);
    // Top two bits of self->last_state don't matter, because they'll be gone as soon as
    // interrupt handler is called.
    self->last_state = state & 3;

    common_hal_rp2pio_statemachine_set_interrupt_handler(&self->state_machine, incrementalencoder_interrupt_handler, self, PIO_IRQ0_INTF_SM0_RXNEMPTY_BITS);
}

bool common_hal_rotaryio_incrementalencoder_deinited(rotaryio_incrementalencoder_obj_t* self) {
    return common_hal_rp2pio_statemachine_deinited(&self->state_machine);
}

void common_hal_rotaryio_incrementalencoder_deinit(rotaryio_incrementalencoder_obj_t* self) {
    if (common_hal_rotaryio_incrementalencoder_deinited(self)) {
        return;
    }
    common_hal_rp2pio_statemachine_deinit(&self->state_machine);
}

mp_int_t common_hal_rotaryio_incrementalencoder_get_position(rotaryio_incrementalencoder_obj_t* self) {
    return self->position;
}

void common_hal_rotaryio_incrementalencoder_set_position(rotaryio_incrementalencoder_obj_t* self,
        mp_int_t new_position) {
    self->position = new_position;
}

STATIC void incrementalencoder_interrupt_handler(void *self_in) {
    rotaryio_incrementalencoder_obj_t* self = self_in;
    // This table also works for detent both at 11 and 00
    // For 11 at detent:
    // Turning cw: 11->01->00->10->11
    // Turning ccw: 11->10->00->01->11
    // For 00 at detent:
    // Turning cw: 00->10->11->10->00
    // Turning ccw: 00->01->11->10->00

    // index table by state <oldA><oldB><newA><newB>
    #define BAD 7
    static const int8_t transitions[16] = {
        0,    // 00 -> 00 no movement
        -1,   // 00 -> 01 3/4 ccw (11 detent) or 1/4 ccw (00 at detent)
        +1,   // 00 -> 10 3/4 cw or 1/4 cw
        BAD,  // 00 -> 11 non-Gray-code transition
        +1,   // 01 -> 00 2/4 or 4/4 cw
        0,    // 01 -> 01 no movement
        BAD,  // 01 -> 10 non-Gray-code transition
        -1,   // 01 -> 11 4/4 or 2/4 ccw
        -1,   // 10 -> 00 2/4 or 4/4 ccw
        BAD,  // 10 -> 01 non-Gray-code transition
        0,    // 10 -> 10 no movement
        +1,   // 10 -> 11 4/4 or 2/4 cw
        BAD,  // 11 -> 00 non-Gray-code transition
        +1,   // 11 -> 01 1/4 or 3/4 cw
        -1,   // 11 -> 10 1/4 or 3/4 ccw
        0,    // 11 -> 11 no movement
    };

    while (common_hal_rp2pio_statemachine_get_in_waiting(&self->state_machine)) {
        // Bypass all the logic of StateMachine.c:_transfer, we need something
        // very simple and fast for an interrupt!
        uint8_t new = self->state_machine.pio->rxf[self->state_machine.state_machine];

        // Shift the old AB bits to the "old" position, and set the new AB bits.
        self->last_state = (self->last_state & 0x3) << 2 | (new & 0x3);

        int8_t quarter_incr = transitions[self->last_state];
        if (quarter_incr == BAD) {
            // Missed a transition. We don't know which way we're going, so do nothing.
            return;
        }

        self->quarter_count += quarter_incr;
        if (self->quarter_count >= 4) {
            self->position += 1;
            self->quarter_count = 0;
        } else if (self->quarter_count <= -4) {
            self->position -= 1;
            self->quarter_count = 0;
        }
    }
}
