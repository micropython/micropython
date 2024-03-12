/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Paul Sokolovsky
 * Copyright (c) 2017 Eric Poulsen
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

#include "esp_task_wdt.h"

typedef struct _machine_wdt_obj_t {
    mp_obj_base_t base;
    esp_task_wdt_user_handle_t twdt_user_handle;
} machine_wdt_obj_t;

static machine_wdt_obj_t wdt_default = {
    {&machine_wdt_type}, 0
};

static machine_wdt_obj_t *mp_machine_wdt_make_new_instance(mp_int_t id, mp_int_t timeout_ms) {
    if (id != 0) {
        mp_raise_ValueError(NULL);
    }

    if (timeout_ms <= 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("WDT timeout too short"));
    }

    esp_task_wdt_config_t config = {
        .timeout_ms = timeout_ms,
        .idle_core_mask = 0,
        .trigger_panic = true,
    };
    mp_int_t rs_code = esp_task_wdt_reconfigure(&config);
    if (rs_code != ESP_OK) {
        mp_raise_OSError(rs_code);
    }

    if (wdt_default.twdt_user_handle == NULL) {
        rs_code = esp_task_wdt_add_user("mpy_machine_wdt", &wdt_default.twdt_user_handle);
        if (rs_code != ESP_OK) {
            mp_raise_OSError(rs_code);
        }
    }

    return &wdt_default;
}

static void mp_machine_wdt_feed(machine_wdt_obj_t *self) {
    (void)self;
    mp_int_t rs_code = esp_task_wdt_reset_user(wdt_default.twdt_user_handle);
    if (rs_code != ESP_OK) {
        mp_raise_OSError(rs_code);
    }
}
