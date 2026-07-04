/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Nicko van Someren
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

// machine.WDT for the UEFI port, over BootServices->SetWatchdogTimer. Included
// into extmod/machine_wdt.c via MICROPY_PY_MACHINE_WDT_INCLUDEFILE (not compiled
// standalone). Note: efi_main() disarms the platform watchdog at startup; creating
// a WDT re-arms it. There is a single platform watchdog, so this is a singleton.

#include "efi.h"
#include "uefi_port.h"

typedef struct _machine_wdt_obj_t {
    mp_obj_base_t base;
    mp_int_t timeout_ms;
} machine_wdt_obj_t;

static machine_wdt_obj_t machine_wdt_default = {{&machine_wdt_type}, 5000};

static void wdt_arm(mp_int_t timeout_ms) {
    UINTN secs = (timeout_ms + 999) / 1000;
    if (secs == 0) {
        secs = 1;
    }
    mp_uefi_st->BootServices->SetWatchdogTimer(secs, 0, 0, NULL);
}

static machine_wdt_obj_t *mp_machine_wdt_make_new_instance(mp_obj_t id, mp_int_t timeout_ms) {
    (void)id;
    if (timeout_ms <= 0) {
        timeout_ms = 5000;
    }
    machine_wdt_default.timeout_ms = timeout_ms;
    wdt_arm(timeout_ms);
    return &machine_wdt_default;
}

static void mp_machine_wdt_feed(machine_wdt_obj_t *self) {
    wdt_arm(self->timeout_ms);
}

static void mp_machine_wdt_timeout_ms_set(machine_wdt_obj_t *self, mp_int_t timeout_ms) {
    self->timeout_ms = timeout_ms;
    wdt_arm(timeout_ms);
}
