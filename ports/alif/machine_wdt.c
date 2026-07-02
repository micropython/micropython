/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 OpenMV LLC.
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

// This file is never compiled standalone, it's included directly from
// extmod/machine_wdt.c via MICROPY_PY_MACHINE_WDT_INCLUDEFILE.

#include "cgu_ext.h"
#include "wdt.h"
#include "se_services.h"

#if CORE_M55_HP
#define WDT_CTRL (WDT_CTRL_Type *)(WDT_HP_CTRL_BASE)
#elif CORE_M55_HE
#define WDT_CTRL (WDT_CTRL_Type *)(WDT_HE_CTRL_BASE)
#endif

#define WDT_COUNTER_MAX (0xffffffffU)

typedef struct _machine_wdt_obj_t {
    mp_obj_base_t base;
    uint32_t timeout_ticks;
} machine_wdt_obj_t;

static machine_wdt_obj_t machine_wdt = {{&machine_wdt_type}, 0};

// When the WDT fires it triggers the NMI interrupt.
void NMI_Handler(void) {
    wdt_unlock(WDT_CTRL);
    wdt_stop(WDT_CTRL);
    se_services_reset_soc();
}

static machine_wdt_obj_t *mp_machine_wdt_make_new_instance(mp_obj_t id_obj, mp_int_t timeout_ms) {
    // Verify the WDT id.
    mp_int_t id = mp_obj_get_int(id_obj);
    if (id != 0) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("WDT(%d) doesn't exist"), id);
    }

    // Convert milliseconds to ticks.
    uint64_t timeout_ticks = (uint64_t)timeout_ms * (uint64_t)cgu_get_rtss_hx_clk_khz();

    // Check that the timeout is within range of the peripheral limits.
    if (timeout_ticks <= 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("WDT timeout too short"));
    } else if (timeout_ticks > WDT_COUNTER_MAX) {
        unsigned int max_timeout_ms = WDT_COUNTER_MAX / cgu_get_rtss_hx_clk_khz();
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("WDT timeout too long, max %ums"), max_timeout_ms);
    }

    // Start the watchdog.
    machine_wdt.timeout_ticks = timeout_ticks;
    wdt_unlock(WDT_CTRL);
    wdt_start(WDT_CTRL, timeout_ticks);
    wdt_lock(WDT_CTRL);

    return &machine_wdt;
}

static void mp_machine_wdt_feed(machine_wdt_obj_t *self) {
    wdt_unlock(WDT_CTRL);
    wdt_feed(WDT_CTRL, self->timeout_ticks);
    wdt_lock(WDT_CTRL);
}
