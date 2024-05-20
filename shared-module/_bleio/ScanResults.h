// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2018 Artur Pacholec
//
// SPDX-License-Identifier: MIT

#pragma once

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
    const uint8_t *peer_addr,
    uint8_t addr_type,
    const uint8_t *data,
    uint16_t len);
