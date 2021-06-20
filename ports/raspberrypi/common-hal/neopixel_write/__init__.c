/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
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

#include "shared-bindings/neopixel_write/__init__.h"

#include "bindings/rp2pio/StateMachine.h"
#include "common-hal/rp2pio/StateMachine.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/digitalio/DigitalInOut.h"

#include "supervisor/port.h"

uint64_t next_start_raw_ticks = 0;

// NeoPixels are 800khz bit streams. Zeroes are 1/3 duty cycle (~416ns) and ones
// are 2/3 duty cycle (~833ns). Each of the instructions below take 1/3 duty
// cycle. The first two instructions always run while only one of the two final
// instructions run per bit. We start with the low period because it can be
// longer than 1/3 period while waiting for more data.
const uint16_t neopixel_program[] = {
// bitloop:
//   out x 1        side 0 [1]; Side-set still takes place before instruction stalls
    0x6121,
//   jmp !x do_zero side 1 [1]; Branch on the bit we shifted out after 1/3 duty delay. Positive pulse
    0x1123,
// do_one:
//   jmp  bitloop   side 1 [1]; Continue driving high, for a long pulse
    0x1100,
// do_zero:
//   nop            side 0 [1]; Or drive low, for a short pulse
    0xa142
};

void common_hal_neopixel_write(const digitalio_digitalinout_obj_t *digitalinout, uint8_t *pixels, uint32_t num_bytes) {
    // Set everything up.
    rp2pio_statemachine_obj_t state_machine;

    // TODO: Cache the state machine after we create it once. We'll need a way to
    // change the pins then though.
    uint32_t pins_we_use = 1 << digitalinout->pin->number;
    bool ok = rp2pio_statemachine_construct(&state_machine,
        neopixel_program, sizeof(neopixel_program) / sizeof(neopixel_program[0]),
        800000 * 6, // 800 khz * 6 cycles per bit
        NULL, 0, // init program
        NULL, 1, // out
        NULL, 1, // in
        0, 0, // in pulls
        NULL, 1, // set
        digitalinout->pin, 1, // sideset
        0, pins_we_use, // initial pin state
        pins_we_use, true, false,
        true, 8, false, // TX, auto pull every 8 bits. shift left to output msb first
        true, // Wait for txstall. If we don't, then we'll deinit too quickly.
        false, 32, true, // RX setting we don't use
        false); // claim pins
    if (!ok) {
        // Do nothing. Maybe bitbang?
        return;
    }

    // Wait to make sure we don't append onto the last transmission. This should only be a tick or
    // two.
    while (port_get_raw_ticks(NULL) < next_start_raw_ticks) {
    }

    common_hal_rp2pio_statemachine_write(&state_machine, pixels, num_bytes, 1 /* stride in bytes */);

    // Use a private deinit of the state machine that doesn't reset the pin.
    rp2pio_statemachine_deinit(&state_machine, true);

    // Reset the pin and release it from the PIO
    gpio_init(digitalinout->pin->number);
    common_hal_digitalio_digitalinout_switch_to_output((digitalio_digitalinout_obj_t *)digitalinout, false, DRIVE_MODE_PUSH_PULL);

    // Update the next start.
    next_start_raw_ticks = port_get_raw_ticks(NULL) + 1;
}
