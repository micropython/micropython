/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 Phil Howard
 *                    Mike Bell
 *                    Kirk D. Benell
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

#include "py/mphal.h"

#if MICROPY_HW_ENABLE_PSRAM

#include "hardware/gpio.h"
#include "hardware/flash.h"
#include "hardware/psram.h"
#include "hardware/sync.h"
#include "rp2_psram.h"

// Bring up the PSRAM on the given chip-select pin using the SDK's
// hardware_psram library, and return its size in bytes (0 if none found).
//
// This wraps the SDK API so the port's existing call sites are unchanged. It
// intentionally does not rely on the SDK's runtime_init auto-setup, because
// MicroPython also needs to re-run this after the system clock changes (see
// machine.freq()) and after flash programming resets the QMI timings.
size_t __no_inline_not_in_flash_func(psram_init)(uint cs_pin) {
    gpio_set_function(cs_pin, GPIO_FUNC_XIP_CS1);
    flash_devinfo_set_cs_gpio(1, cs_pin);

    uint32_t intr_stash = save_and_disable_interrupts();

    // Detect the chip (reads KGD/EID via the bootrom); psram_detect_size()
    // handles the APS6404 and ISSI EID-to-size mappings.
    size_t psram_size = psram_detect_size();
    if (!psram_size) {
        flash_devinfo_set_cs_size(1, FLASH_DEVINFO_SIZE_NONE);
        restore_interrupts(intr_stash);
        return 0;
    }
    flash_devinfo_set_cs_size(1, flash_devinfo_bytes_to_size(psram_size));

    // Compute timing for the current system clock and apply it (enables QPI
    // mode, sets read/write formats, and makes PSRAM writable).
    if (psram_configure_params(PICO_DEFAULT_PSRAM_MAX_FREQ,
        PICO_DEFAULT_PSRAM_MAX_SELECT, PICO_DEFAULT_PSRAM_MIN_DESELECT) != PICO_OK ||
        psram_reinitialize() != PICO_OK) {
        restore_interrupts(intr_stash);
        return 0;
    }

    restore_interrupts(intr_stash);

    return psram_size;
}

#endif
