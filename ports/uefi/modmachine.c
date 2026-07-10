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

// Port machine-module glue for UEFI. This file is *included* into
// extmod/modmachine.c via MICROPY_PY_MACHINE_INCLUDEFILE (it is not compiled on
// its own). It supplies mp_machine_idle() and adds Timer to the module globals.
// The Timer type itself lives in machine_timer.c (compiled directly, so its
// MP_REGISTER_ROOT_POINTER is collected).

#include <string.h>

#include "py/runtime.h"
#include "py/mphal.h"
#include "efi.h"
#include "uefi_port.h"

// machine.idle(): wait for the next interrupt/event (our WFE primitive).
static void mp_machine_idle(void) {
    mp_event_wait_indefinite();
}

// machine.reset()/reset_cause(): cold reset via RuntimeServices.
MP_NORETURN static void mp_machine_reset(void) {
    mp_uefi_st->RuntimeServices->ResetSystem(EfiResetCold, 0, 0, NULL);
    for (;;) {
    }
}
static mp_int_t mp_machine_reset_cause(void) {
    return 0; // unknown
}

// machine.freq(): the monotonic counter frequency. set_freq is unsupported.
static mp_obj_t mp_machine_get_freq(void) {
    return mp_obj_new_int_from_ull(mp_uefi_counter_hz());
}
static void mp_machine_set_freq(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    (void)args;
    mp_raise_NotImplementedError(NULL);
}

// machine.lightsleep([ms]) yields (timers/callbacks keep flowing); deepsleep
// shuts the platform down (closest UEFI analogue).
static void mp_machine_lightsleep(size_t n_args, const mp_obj_t *args) {
    if (n_args > 0) {
        mp_hal_delay_ms(mp_obj_get_int(args[0]));
    } else {
        mp_event_wait_indefinite();
    }
}
MP_NORETURN static void mp_machine_deepsleep(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    (void)args;
    mp_uefi_st->RuntimeServices->ResetSystem(EfiResetShutdown, 0, 0, NULL);
    for (;;) {
    }
}

// machine.unique_id(): the SMBIOS System Information (type 1) UUID, else zeros.
static void uefi_smbios_uuid(uint8_t out[16]) {
    EFI_GUID g3 = SMBIOS3_TABLE_GUID, g1 = SMBIOS_TABLE_GUID;
    void *entry = NULL;
    int v3 = 0;
    for (UINTN i = 0; i < mp_uefi_st->NumberOfTableEntries; i++) {
        EFI_CONFIGURATION_TABLE *t = &mp_uefi_st->ConfigurationTable[i];
        if (memcmp(&t->VendorGuid, &g3, sizeof(EFI_GUID)) == 0) {
            entry = t->VendorTable;
            v3 = 1;
            break;
        }
        if (memcmp(&t->VendorGuid, &g1, sizeof(EFI_GUID)) == 0) {
            entry = t->VendorTable;
        }
    }
    if (entry == NULL) {
        return;
    }
    uint8_t *e = entry, *tbl;
    uint32_t len;
    if (v3) { // SMBIOS 3 entry point: 64-bit table addr @0x10, max size @0x0C
        memcpy(&tbl, e + 0x10, sizeof(tbl));
        memcpy(&len, e + 0x0C, sizeof(len));
    } else { // SMBIOS 2.x entry point: 32-bit table addr @0x18, length @0x16
        uint32_t addr;
        uint16_t l16;
        memcpy(&addr, e + 0x18, sizeof(addr));
        memcpy(&l16, e + 0x16, sizeof(l16));
        tbl = (uint8_t *)(uintptr_t)addr;
        len = l16;
    }
    uint8_t *p = tbl, *end = tbl + len;
    while (p + 4 <= end) {
        uint8_t type = p[0], slen = p[1];
        if (type == 127) { // end-of-table
            break;
        }
        if (type == 1 && slen >= 0x18) {
            memcpy(out, p + 0x08, 16); // System UUID at offset 8
            return;
        }
        uint8_t *s = p + slen; // skip formatted area, then the string set (\0\0)
        while (s + 1 < end && !(s[0] == 0 && s[1] == 0)) {
            s++;
        }
        p = s + 2;
    }
}
static mp_obj_t mp_machine_unique_id(void) {
    uint8_t id[16] = {0};
    uefi_smbios_uuid(id);
    return mp_obj_new_bytes(id, sizeof(id));
}

extern const mp_obj_type_t machine_timer_type;
extern const mp_obj_type_t machine_rtc_type;

// Add Timer and RTC to the machine module's globals.
#define MICROPY_PY_MACHINE_EXTRA_GLOBALS \
    { MP_ROM_QSTR(MP_QSTR_Timer), MP_ROM_PTR(&machine_timer_type) }, \
    { MP_ROM_QSTR(MP_QSTR_RTC), MP_ROM_PTR(&machine_rtc_type) },
