// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_BLE_HCI_COMMON_HAL_CHARACTERISTICBUFFER_H
#define MICROPY_INCLUDED_BLE_HCI_COMMON_HAL_CHARACTERISTICBUFFER_H

#include "py/ringbuf.h"
#include "shared-bindings/_bleio/Characteristic.h"

typedef struct {
    mp_obj_base_t base;
    bleio_characteristic_obj_t *characteristic;
    uint32_t timeout_ms;
    // Ring buffer storing consecutive incoming values.
    ringbuf_t ringbuf;
} bleio_characteristic_buffer_obj_t;

void bleio_characteristic_buffer_update(bleio_characteristic_buffer_obj_t *self, mp_buffer_info_t *bufinfo);

#endif // MICROPY_INCLUDED_BLE_HCI_COMMON_HAL_CHARACTERISTICBUFFER_H
