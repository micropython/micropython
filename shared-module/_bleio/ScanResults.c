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

#include "lib/utils/interrupt_char.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "shared-bindings/_bleio/ScanEntry.h"
#include "shared-bindings/_bleio/ScanResults.h"

bleio_scanresults_obj_t *shared_module_bleio_new_scanresults(size_t buffer_size, uint8_t *prefixes, size_t prefixes_len, mp_int_t minimum_rssi) {
    bleio_scanresults_obj_t *self = m_new_obj(bleio_scanresults_obj_t);
    self->base.type = &bleio_scanresults_type;
    ringbuf_alloc(&self->buf, buffer_size, false);
    self->prefixes = prefixes;
    self->prefix_length = prefixes_len;
    self->minimum_rssi = minimum_rssi;
    return self;
}

mp_obj_t common_hal_bleio_scanresults_next(bleio_scanresults_obj_t *self) {
    while (ringbuf_num_filled(&self->buf) == 0 && !self->done && !mp_hal_is_interrupted()) {
        RUN_BACKGROUND_TASKS;
    }
    if (ringbuf_num_filled(&self->buf) == 0 || mp_hal_is_interrupted()) {
        return mp_const_none;
    }

    // Create a ScanEntry out of the data on the buffer.
    uint8_t type = ringbuf_get(&self->buf);
    bool connectable = (type & (1 << 0)) != 0;
    bool scan_response = (type & (1 << 1)) != 0;
    uint64_t ticks_ms;
    ringbuf_get_n(&self->buf, (uint8_t *)&ticks_ms, sizeof(ticks_ms));
    uint8_t rssi = ringbuf_get(&self->buf);
    uint8_t peer_addr[NUM_BLEIO_ADDRESS_BYTES];
    ringbuf_get_n(&self->buf, peer_addr, sizeof(peer_addr));
    uint8_t addr_type = ringbuf_get(&self->buf);
    uint16_t len;
    ringbuf_get_n(&self->buf, (uint8_t *)&len, sizeof(len));

    mp_obj_str_t *o = MP_OBJ_TO_PTR(mp_obj_new_bytes_of_zeros(len));
    ringbuf_get_n(&self->buf, (uint8_t *)o->data, len);

    bleio_scanentry_obj_t *entry = m_new_obj(bleio_scanentry_obj_t);
    entry->base.type = &bleio_scanentry_type;
    entry->rssi = rssi;

    bleio_address_obj_t *address = m_new_obj(bleio_address_obj_t);
    address->base.type = &bleio_address_type;
    common_hal_bleio_address_construct(MP_OBJ_TO_PTR(address), peer_addr, addr_type);
    entry->address = address;

    entry->data = o;
    entry->time_received = ticks_ms;
    entry->connectable = connectable;
    entry->scan_response = scan_response;

    return MP_OBJ_FROM_PTR(entry);
}


void shared_module_bleio_scanresults_append(bleio_scanresults_obj_t *self,
    uint64_t ticks_ms,
    bool connectable,
    bool scan_response,
    int8_t rssi,
    uint8_t *peer_addr,
    uint8_t addr_type,
    uint8_t *data,
    uint16_t len) {
    int32_t packet_size = sizeof(uint8_t) + sizeof(ticks_ms) + sizeof(rssi) + NUM_BLEIO_ADDRESS_BYTES +
        sizeof(addr_type) + sizeof(len) + len;
    int32_t empty_space = self->buf.size - ringbuf_num_filled(&self->buf);
    if (packet_size >= empty_space) {
        // We can't fit the packet so skip it.
        return;
    }
    // Filter the packet.
    if (rssi < self->minimum_rssi) {
        return;
    }

    // If any prefixes are provided, then only include packets that include at least one of them.
    if (!bleio_scanentry_data_matches(data, len, self->prefixes, self->prefix_length, true)) {
        return;
    }
    uint8_t type = 0;
    if (connectable) {
        type |= 1 << 0;
    }
    if (scan_response) {
        type |= 1 << 1;
    }

    // Add the packet to the buffer.
    ringbuf_put(&self->buf, type);
    ringbuf_put_n(&self->buf, (uint8_t *)&ticks_ms, sizeof(ticks_ms));
    ringbuf_put(&self->buf, rssi);
    ringbuf_put_n(&self->buf, peer_addr, NUM_BLEIO_ADDRESS_BYTES);
    ringbuf_put(&self->buf, addr_type);
    ringbuf_put_n(&self->buf, (uint8_t *)&len, sizeof(len));
    ringbuf_put_n(&self->buf, data, len);
}

bool shared_module_bleio_scanresults_get_done(bleio_scanresults_obj_t *self) {
    return self->done;
}

void shared_module_bleio_scanresults_set_done(bleio_scanresults_obj_t *self, bool done) {
    self->done = done;
    self->common_hal_data = NULL;
}
