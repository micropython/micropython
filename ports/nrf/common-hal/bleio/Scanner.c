/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Glenn Ruben Bakke
 * Copyright (c) 2018 Artur Pacholec
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

#include "ble_drv.h"
#include "py/mphal.h"
#include "shared-bindings/bleio/ScanEntry.h"
#include "shared-bindings/bleio/Scanner.h"
#include "shared-module/bleio/ScanEntry.h"

STATIC void adv_event_handler(bleio_scanner_obj_t *self, bleio_scanentry_obj_t *entry) {
    // TODO: Don't add new entry for each item, group by address and update

    mp_obj_list_append(self->adv_reports, entry);

    ble_drv_scan_continue();
}

void common_hal_bleio_scanner_scan(bleio_scanner_obj_t *self, mp_int_t timeout) {
    ble_drv_adv_report_handler_set(self, adv_event_handler);

    ble_drv_scan_start(self->interval, self->window);

    mp_hal_delay_ms(timeout);

    ble_drv_scan_stop();
}
