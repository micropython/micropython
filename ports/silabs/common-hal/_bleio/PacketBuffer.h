/*
 * This file is part of Adafruit for EFR32 project
 *
 * The MIT License (MIT)
 *
 * Copyright 2023 Silicon Laboratories Inc. www.silabs.com
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

#ifndef MICROPY_INCLUDED_EFR_COMMON_HAL_BLEIO_PACKETBUFFER_H
#define MICROPY_INCLUDED_EFR_COMMON_HAL_BLEIO_PACKETBUFFER_H

#include "py/ringbuf.h"
#include "shared-bindings/_bleio/Characteristic.h"

#define MAX_NUMBER_PACKET_BUFFER 64
typedef struct {
    mp_obj_base_t base;
    bleio_characteristic_obj_t *characteristic;
    // Ring buffer storing consecutive incoming values.
    ringbuf_t ringbuf;
    // Two outgoing buffers to alternate between.
    // One will be queued for transmission by the SD and
    // the other is waiting to be queued and can be extended.
    uint32_t *outgoing[2];
    volatile uint16_t pending_size;
    // We remember the conn_handle so we can do a NOTIFY/INDICATE to a client.
    // We can find out the conn_handle on a Characteristic write
    // or a CCCD write (but not a read).
    volatile uint16_t conn_handle;
    uint16_t max_packet_size;
    uint8_t pending_index;
    uint8_t write_type;
    bool client;
    bool packet_queued;
} bleio_packet_buffer_obj_t;

typedef struct {
    bleio_packet_buffer_obj_t *data[MAX_NUMBER_PACKET_BUFFER];
    uint8_t len;
} bleio_packet_buffer_obj_list_t;

// Unused
typedef void *ble_event_handler_t;

extern bool packet_buffer_on_ble_evt(uint16_t attribute,
    uint8_t *data,
    uint16_t len);
extern void reset_packet_buffer_list();
#endif // MICROPY_INCLUDED_EFR_COMMON_HAL_BLEIO_PACKETBUFFER_H
