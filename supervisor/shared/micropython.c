// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "lib/oofatfs/ff.h"
#include "py/mpconfig.h"
#include "py/mphal.h"
#include "py/mpstate.h"
#include "py/runtime.h"
#include "py/stream.h"

#include "supervisor/shared/serial.h"
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

mp_uint_t mp_hal_stdout_tx_strn(const char *str, size_t len) {
    toggle_tx_led();

    #ifdef CIRCUITPY_BOOT_OUTPUT_FILE
    if (boot_output != NULL) {
        // Ensure boot_out.txt is capped at 1 filesystem block and ends with a newline
        #define TRUNCATED MP_ERROR_TEXT("[truncated due to length]")
        size_t truncated_message_len = decompress_length(TRUNCATED);
        size_t maxlen = 512 - truncated_message_len; // includes trailing '\0' so we do not need to account for trailing newline '\n' in vstr_add_byte
        if (len + boot_output->len > maxlen) {
            size_t remaining_len = maxlen - boot_output->len;
            vstr_add_strn(boot_output, str, remaining_len);
            char buf[truncated_message_len];
            vstr_add_str(boot_output, decompress(TRUNCATED, buf));
            vstr_add_byte(boot_output, '\n');
            boot_output = NULL;
        } else {
            vstr_add_strn(boot_output, str, len);
        }
    }
    #endif

    return serial_write_substring(str, len);
}

uintptr_t mp_hal_stdio_poll(uintptr_t poll_flags) {
    uintptr_t ret = 0;
    if ((poll_flags & MP_STREAM_POLL_RD) && serial_bytes_available()) {
        ret |= MP_STREAM_POLL_RD;
    }
    return ret;
}
