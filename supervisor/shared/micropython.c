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

#include <string.h>

#include "supervisor/serial.h"
#include "lib/oofatfs/ff.h"
#include "py/mpconfig.h"
#include "py/mpstate.h"
#include "py/runtime.h"
#include "py/stream.h"

#include "supervisor/shared/status_leds.h"

#if CIRCUITPY_WATCHDOG
#include "shared-bindings/watchdog/__init__.h"
#define WATCHDOG_EXCEPTION_CHECK() (MP_STATE_VM(mp_pending_exception) == &mp_watchdog_timeout_exception)
#else
#define WATCHDOG_EXCEPTION_CHECK() 0
#endif

int mp_hal_stdin_rx_chr(void) {
    for (;;) {
        #ifdef MICROPY_VM_HOOK_LOOP
        MICROPY_VM_HOOK_LOOP
        #endif
        mp_handle_pending(true);
        if (serial_bytes_available()) {
            toggle_rx_led();
            return serial_read();
        }
    }
}

void mp_hal_stdout_tx_strn(const char *str, size_t len) {
    toggle_tx_led();

    #ifdef CIRCUITPY_BOOT_OUTPUT_FILE
    if (boot_output_file != NULL) {
        UINT bytes_written = 0;
        f_write(boot_output_file, str, len, &bytes_written);
    }
    #endif

    serial_write_substring(str, len);
}

uintptr_t mp_hal_stdio_poll(uintptr_t poll_flags) {
    uintptr_t ret = 0;
    if ((poll_flags & MP_STREAM_POLL_RD) && serial_bytes_available()) {
        ret |= MP_STREAM_POLL_RD;
    }
    return ret;
}
