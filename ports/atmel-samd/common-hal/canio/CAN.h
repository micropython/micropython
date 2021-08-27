/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Jeff Epler for Adafruit Industries
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
