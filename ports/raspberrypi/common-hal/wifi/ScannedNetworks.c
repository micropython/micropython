/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Dan Halbert for Adafruit Industries
 * Copyright (c) 2018 Artur Pacholec
 * Copyright (c) 2017 Glenn Ruben Bakke
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

#include "shared/runtime/interrupt_char.h"
#include "py/gc.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "common-hal/wifi/__init__.h"
#include "shared-bindings/wifi/__init__.h"
#include "shared-bindings/wifi/Network.h"
#include "shared-bindings/wifi/Radio.h"
#include "shared-bindings/wifi/ScannedNetworks.h"

#define NUM_SCAN (16)
static cyw43_ev_scan_result_t scan_results[NUM_SCAN];
static uint8_t scan_put, scan_get;
static bool scan_full;


static void scan_result_put(const cyw43_ev_scan_result_t *result) {
    if (!scan_full) {
        scan_results[scan_put] = *result;
        scan_put = (scan_put + 1) % NUM_SCAN;
        scan_full = (scan_put == scan_get);
    }
}

static bool scan_result_available(void) {
    return scan_full || (scan_get != scan_put);
}

static cyw43_ev_scan_result_t *scan_result_get(cyw43_ev_scan_result_t *result) {
    if (!scan_result_available()) {
        return NULL;
    }

    *result = scan_results[scan_get];
    scan_get = (scan_get + 1) % NUM_SCAN;
    scan_full = false;
    return result;
}

// Note: It's not OK to allocate memory in here, we can be called at a bad time
// which messes up the gc allocator
static int scan_result(void *env, const cyw43_ev_scan_result_t *result) {
    wifi_scannednetworks_obj_t *self = common_hal_wifi_radio_obj.current_scan;
    // scan ended or something
    if (!self) {
        return 0;
    }

    scan_result_put(result);

    return 0;
}

mp_obj_t common_hal_wifi_scannednetworks_next(wifi_scannednetworks_obj_t *self) {
    // no results available, wait for some
    while (!scan_result_available() && cyw43_wifi_scan_active(&cyw43_state)) {
        RUN_BACKGROUND_TASKS;
        if (mp_hal_is_interrupted()) {
            return mp_const_none;
        }
        cyw43_arch_poll();
    }

    if (!scan_result_available()) {
        common_hal_wifi_radio_obj.current_scan = NULL;
        return mp_const_none;
    }


    wifi_network_obj_t *entry = mp_obj_malloc(wifi_network_obj_t, &wifi_network_type);
    scan_result_get(&entry->record);

    return MP_OBJ_FROM_PTR(entry);
}

void wifi_scannednetworks_deinit(wifi_scannednetworks_obj_t *self) {
    // there's actually no way to stop an ongoing scan in cyw43!
    common_hal_wifi_radio_obj.current_scan = NULL;
}

void wifi_scannednetworks_start_scan(wifi_scannednetworks_obj_t *self) {
    cyw43_wifi_scan_options_t scan_options = {0};
    CHECK_CYW_RESULT(cyw43_wifi_scan(&cyw43_state, &scan_options, NULL, scan_result));
}
