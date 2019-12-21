/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Musumeci Salvatore
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
#ifndef MICROPY_INCLUDED_ESP32_CAN_H
#define MICROPY_INCLUDED_ESP32_CAN_H

#include "modmachine.h"

#include "py/obj.h"

#if MICROPY_HW_ENABLE_CAN

#define DEVICE_NAME "CAN"

#define CAN_BAUDRATE_25k 25
#define CAN_BAUDRATE_50k 50
#define CAN_BAUDRATE_100k 100
#define CAN_BAUDRATE_125k 125
#define CAN_BAUDRATE_250k 250
#define CAN_BAUDRATE_500k 500
#define CAN_BAUDRATE_800k 800
#define CAN_BAUDRATE_1M 1000

typedef enum _filter_mode_t{
    FILTER_RAW_SINGLE = 0,
    FILTER_RAW_DUAL,
    FILTER_ADDR_SINGLE,
    FILTER_ADDR_DUAL
} filter_mode_t;

typedef struct _machine_can_config_t {
    const can_timing_config_t *timing;
    can_filter_config_t *filter; //FIXME: point to this variable for setting
    const can_general_config_t *general;
    uint16_t baudrate;
    bool initialized;
} machine_can_config_t;

typedef struct _machine_can_obj_t {
    mp_obj_base_t base;
    machine_can_config_t *config;
    mp_obj_t rxcallback;
    byte rx_state;
    bool extframe : 1;
    bool loopback : 1;
    uint16_t num_error_warning; //FIXME: populate this value somewhere
    uint16_t num_error_passive;
    uint16_t num_bus_off;
} machine_can_obj_t;

typedef enum _rx_state_t {
    RX_STATE_FIFO_EMPTY = 0,
    RX_STATE_MESSAGE_PENDING,
    RX_STATE_FIFO_FULL,
    RX_STATE_FIFO_OVERFLOW,
} rx_state_t;

extern const mp_obj_type_t machine_can_type;

#endif // MICROPY_HW_ENABLE_CAN
#endif // MICROPY_INCLUDED_ESP32_CAN_H
