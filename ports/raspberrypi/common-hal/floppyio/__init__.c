// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "bindings/rp2pio/StateMachine.h"
#include "py/runtime.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/floppyio/__init__.h"
#include "common-hal/floppyio/__init__.h"
#include "shared-bindings/time/__init__.h"
#include "supervisor/shared/tick.h"

static const uint16_t fluxread_program[] = {
    // ; Count flux pulses and watch for index pin
    // ; flux input is the 'jmp pin'.  index is "pin zero".
    // ; Counts are in units 3 / F_pio, so e.g., at 30MHz 1 count = 0.1us
    // ; Count down while waiting for the counter to go HIGH
    // ; The only counting is down, so C code will just have to negate the
    // count!
    // ; Each 'wait one' loop takes 3 instruction-times
    // wait_one:
    0x0041, //     jmp x--, wait_one_next ; acts as a non-conditional decrement
            //     of x
            // wait_one_next:
    0x00c3, //     jmp pin wait_zero
    0x0000, //     jmp wait_one
            // ; Each 'wait zero' loop takes 3 instruction-times, needing one
            // instruction delay
            // ; (it has to match the 'wait one' timing exactly)
            // wait_zero:
    0x0044, //     jmp x--, wait_zero_next ; acts as a non-conditional decrement
            //     of x
            // wait_zero_next:
    0x01c3, //     jmp pin wait_zero [1]
            // ; Top bit is index status, bottom 15 bits are inverse of counts
            // ; Combined FIFO gives 16 entries (8 32-bit entries) so with the
    // ; smallest plausible pulse of 2us there are 250 CPU cycles available
    // @125MHz
    0x4001, //     in pins, 1
    0x402f, //     in x, 15
    // ; Three cycles for the end of loop, so we need to decrement x to make
    // everything
    // ; come out right. This has constant timing whether we actually jump back
    // vs wrapping.
    0x0040, //     jmp x--, wait_one
};

typedef struct {
    PIO pio;
    uint8_t sm;
    bool word_available;
    uint16_t half;
} floppy_reader;

static bool data_available(floppy_reader *reader) {
    return reader->word_available || !pio_sm_is_rx_fifo_empty(reader->pio, reader->sm);
}

static uint16_t read_fifo(floppy_reader *reader) {
    if (reader->word_available) {
        reader->word_available = false;
        return reader->half;
    }
    uint32_t value = pio_sm_get_blocking(reader->pio, reader->sm);
    reader->half = value >> 16;
    reader->word_available = true;
    return value & 0xffff;
}


int common_hal_floppyio_flux_readinto(void *buf, size_t len, digitalio_digitalinout_obj_t *data, digitalio_digitalinout_obj_t *index, mp_int_t index_wait_ms) {
#define READ_INDEX() (!!(*index_port & index_mask))
    uint32_t index_mask;
    volatile uint32_t *index_port = common_hal_digitalio_digitalinout_get_reg(index, DIGITALINOUT_REG_READ, &index_mask);

    memset(buf, 0, len);

    uint32_t pins_we_use = 1 << data->pin->number;

    rp2pio_statemachine_obj_t state_machine;
    bool ok = rp2pio_statemachine_construct(&state_machine,
        fluxread_program, MP_ARRAY_SIZE(fluxread_program),
        FLOPPYIO_SAMPLERATE * 3, // 3 PIO cycles per sample count
        NULL, 0, // init program
        NULL, 0, // out
        index->pin, 1, // in
        1, 0, // in pull up/down
        NULL, 0, // set
        NULL, 0, // sideset
        0, 0, // initial pin state
        data->pin, // jump pin
        pins_we_use, false, true,
        true, 32, false, // TX setting we don't use
        true, // Wait for txstall. If we don't, then we'll deinit too quickly.
        true, 32, true, // move 32 bits at a time
        false, // claim pins
        false, // Not user-interruptible.
        false, // No sideset enable
        0, -1, // wrap
        PIO_ANY_OFFSET  // offset
        );
    if (!ok) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("All state machines in use"));
    }

    floppy_reader reader = { .pio = state_machine.pio, .sm = state_machine.state_machine, };

    uint8_t *ptr = buf, *end = ptr + len;

    uint64_t index_deadline_us = time_us_64() + index_wait_ms * 1000;

    common_hal_mcu_disable_interrupts();

    // check if flux is arriving
    uint64_t flux_deadline_us = time_us_64() + 20;
    while (pio_sm_is_rx_fifo_empty(reader.pio, reader.sm)) {
        if (time_us_64() > flux_deadline_us) {
            common_hal_mcu_enable_interrupts();
            common_hal_rp2pio_statemachine_deinit(&state_machine);
            mp_raise_RuntimeError(MP_ERROR_TEXT("timeout waiting for flux"));
        }
    }

    // wait for index pulse low
    while (READ_INDEX()) {
        if (time_us_64() > index_deadline_us) {
            common_hal_mcu_enable_interrupts();
            common_hal_rp2pio_statemachine_deinit(&state_machine);
            mp_raise_RuntimeError(MP_ERROR_TEXT("timeout waiting for index pulse"));
        }
    }

    pio_sm_clear_fifos(reader.pio, reader.sm);

    // if another index doesn't show up ...
    index_deadline_us = time_us_64() + index_wait_ms * 1000;

    int last = read_fifo(&reader);
    bool last_index = READ_INDEX();
    while (ptr != end) {

        /* Handle index */
        bool now_index = READ_INDEX();

        if (!now_index && last_index) {
            break;
        }
        last_index = now_index;

        if (!data_available(&reader)) {
            // no flux is arriving? is ANY flux arriving or has a full revoulution gone by?
            if (time_us_64() > index_deadline_us) {
                break;
            }
            continue;
        }

        int timestamp = read_fifo(&reader);
        int delta = last - timestamp;
        if (delta < 0) {
            delta += 65536;
        }
        delta /= 2;

        last = timestamp;
        *ptr++ = delta > 255 ? 255 : delta;
    }

    common_hal_mcu_enable_interrupts();
    common_hal_rp2pio_statemachine_deinit(&state_machine);

    return ptr - (uint8_t *)buf;
}
