// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "shared-bindings/canio/__init__.h"
#include "shared-bindings/canio/CAN.h"
#include "common-hal/microcontroller/Pin.h"
#include "common-hal/canio/__init__.h"
#include "shared-module/canio/Message.h"

#include "driver/twai.h"

#define FILTER_BANK_COUNT (28)

typedef struct canio_can_obj {
    mp_obj_base_t base;
    int baudrate;
    const mcu_pin_obj_t *rx_pin;
    const mcu_pin_obj_t *tx_pin;
    bool loopback : 1;
    bool silent : 1;
    bool auto_restart : 1;
    bool fifo_in_use : 1;
} canio_can_obj_t;
