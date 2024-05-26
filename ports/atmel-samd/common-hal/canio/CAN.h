// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "shared-bindings/canio/__init__.h"
#include "shared-bindings/canio/CAN.h"
#include "component/can.h"
#include "common-hal/microcontroller/Pin.h"
#include "common-hal/canio/__init__.h"
#include "shared-module/canio/Message.h"

#define COMMON_HAL_CAN_RX_FIFO_LEN (2)
#define COMMON_HAL_CAN_TX_FIFO_LEN (2)

typedef struct canio_can_obj {
    mp_obj_base_t base;
    Can *hw;
    canio_can_state_t *state;
    int baudrate;
    uint8_t rx_pin_number : 8;
    uint8_t tx_pin_number : 8;
    bool loopback : 1;
    bool silent : 1;
    bool auto_restart : 1;
    bool fifo0_in_use : 1;
    bool fifo1_in_use : 1;
} canio_can_obj_t;
