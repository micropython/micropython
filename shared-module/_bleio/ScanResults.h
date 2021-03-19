/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Dan Halbert for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_SHARED_MODULE_BLEIO_SCANRESULTS_H
#define MICROPY_INCLUDED_SHARED_MODULE_BLEIO_SCANRESULTS_H

#include <stdint.h>

#include "py/obj.h"
#include "py/ringbuf.h"

typedef struct {
    mp_obj_base_t base;
    // Pointers that needs to live until the scan is done.
    void *common_hal_data;
    ringbuf_t buf;
    // Prefixes is a length encoded array of prefixes.
    uint8_t *prefixes;
    size_t prefix_length;
    mp_int_t minimum_rssi;
    bool active;
    bool done;
} bleio_scanresults_obj_t;

bleio_scanresults_obj_t *shared_module_bleio_new_scanresults(size_t buffer_size, uint8_t *prefixes, size_t prefixes_len, mp_int_t minimum_rssi);

bool shared_module_bleio_scanresults_get_done(bleio_scanresults_obj_t *self);
void shared_module_bleio_scanresults_set_done(bleio_scanresults_obj_t *self, bool done);

void shared_module_bleio_scanresults_append(bleio_scanresults_obj_t *self,
    uint64_t ticks_ms,
    bool connectable,
    bool scan_result,
    int8_t rssi,
    uint8_t *peer_addr,
    uint8_t addr_type,
    uint8_t *data,
    uint16_t len);

#endif // MICROPY_INCLUDED_SHARED_MODULE_BLEIO_SCANRESULTS_H
