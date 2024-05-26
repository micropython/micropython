// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/_bleio/PacketBuffer.h"

extern const mp_obj_type_t bleio_packet_buffer_type;

void common_hal_bleio_packet_buffer_construct(
    bleio_packet_buffer_obj_t *self, bleio_characteristic_obj_t *characteristic,
    size_t buffer_size, size_t max_packet_size);
// Allocation free
void _common_hal_bleio_packet_buffer_construct(
    bleio_packet_buffer_obj_t *self, bleio_characteristic_obj_t *characteristic,
    uint32_t *incoming_buffer, size_t incoming_buffer_size,
    uint32_t *outgoing_buffer1, uint32_t *outgoing_buffer2, size_t outgoing_buffer_size,
    void *static_handler_entry);
mp_int_t common_hal_bleio_packet_buffer_write(bleio_packet_buffer_obj_t *self, const uint8_t *data, size_t len, uint8_t *header, size_t header_len);
mp_int_t common_hal_bleio_packet_buffer_readinto(bleio_packet_buffer_obj_t *self, uint8_t *data, size_t len);
mp_int_t common_hal_bleio_packet_buffer_get_incoming_packet_length(bleio_packet_buffer_obj_t *self);
mp_int_t common_hal_bleio_packet_buffer_get_outgoing_packet_length(bleio_packet_buffer_obj_t *self);
void common_hal_bleio_packet_buffer_flush(bleio_packet_buffer_obj_t *self);
bool common_hal_bleio_packet_buffer_deinited(bleio_packet_buffer_obj_t *self);
void common_hal_bleio_packet_buffer_deinit(bleio_packet_buffer_obj_t *self);
