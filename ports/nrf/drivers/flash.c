/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Ayke van Laethem
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

#include "py/mpconfig.h"

#if MICROPY_MBFS && BLUETOOTH_SD

#include "drivers/flash.h"
#include "drivers/bluetooth/ble_drv.h"
#include "nrf_soc.h"

// Rotates bits in `value` left `shift` times.
STATIC inline uint32_t rotate_left(uint32_t value, uint32_t shift) {
    return (value << shift) | (value >> (32 - shift));
}

STATIC volatile flash_state_t flash_operation_state = FLASH_STATE_BUSY;

STATIC void operation_init() {
    flash_operation_state = FLASH_STATE_BUSY;
}

void flash_operation_finished(flash_state_t result) {
    flash_operation_state = result;
}

STATIC bool operation_wait(uint32_t result) {
    if (ble_drv_stack_enabled() != 1) {
        // SoftDevice is not enabled, no event will be generated.
        return result == NRF_SUCCESS;
    }

    if (result != NRF_SUCCESS) {
        // In all other (non-success) cases, the command hasn't been
        // started and no event will be generated.
        return false;
    }

    // Wait until the event has been generated.
    while (flash_operation_state == FLASH_STATE_BUSY) {
        sd_app_evt_wait();
    }

    // Now we can safely continue, flash operation has completed.
    return flash_operation_state == FLASH_STATE_SUCCESS;
}

void flash_write_byte(uint32_t address, uint8_t b) {
    uint32_t address_aligned = address & ~3;

    // Value to write - leave all bits that should not change at 0xff.
    uint32_t value = 0xffffff00 | b;

    // Rotate bits in value to an aligned position.
    value = rotate_left(value, (address & 3) * 8);

    while (1) {
        operation_init();
        uint32_t result = sd_flash_write((uint32_t*)address_aligned, &value, 1);
        if (operation_wait(result)) break;
    }
}

void flash_page_erase(uint32_t pageaddr) {
    while (1) {
        operation_init();
        uint32_t result = sd_flash_page_erase(pageaddr / FLASH_PAGESIZE);
        if (operation_wait(result)) break;
    }
}

void flash_write_bytes(uint32_t dst, const uint8_t *src, uint32_t num_bytes) {
    const uint8_t *src_end = src + num_bytes;

    // sd_flash_write does not accept unaligned addresses so we have to
    // work around that by writing all unaligned addresses byte-by-byte.

    // Write first bytes to align the write address.
    while (src != src_end && (dst & 0b11)) {
        flash_write_byte(dst, *src);
        dst++;
        src++;
    }

    // Write as many words as possible.
    // dst is now aligned, src possibly not.
    while (src_end - src >= 4) {
        uint8_t buf[4] __attribute__((aligned(4)));
        for (int i = 0; i < 4; i++) {
            buf[i] = ((uint8_t*)src)[i];
        }
        operation_init();
        uint32_t result = sd_flash_write((uint32_t*)dst, (const uint32_t*)&buf, 1);
        if (operation_wait(result)) {
            // If it is successfully written, go to the next word.
            src += 4;
            dst += 4;
        }
    }

    // Write remaining unaligned bytes.
    while (src != src_end) {
        flash_write_byte(dst, *src);
        dst++;
        src++;
    }
}

#endif // MICROPY_MBFS
