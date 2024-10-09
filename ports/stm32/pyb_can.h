/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Damien P. George
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
#ifndef MICROPY_INCLUDED_STM32_PYB_CAN_H
#define MICROPY_INCLUDED_STM32_PYB_CAN_H

#include "py/mphal.h"

#if MICROPY_HW_ENABLE_CAN

#include "py/obj.h"
#include "can.h"

typedef struct _pyb_can_obj_t {
    mp_obj_base_t base;
    mp_obj_t rxcallback0;
    mp_obj_t rxcallback1;
    mp_uint_t can_id : 8;
    bool is_enabled : 1;
    byte rx_state0;
    byte rx_state1;
    uint16_t num_error_warning;
    uint16_t num_error_passive;
    uint16_t num_bus_off;
    CAN_HandleTypeDef can;
} pyb_can_obj_t;

extern const mp_obj_type_t pyb_can_type;

void pyb_can_deinit_all(void);
void pyb_can_init0(void);

void pyb_can_irq_handler(uint can_id, can_rx_fifo_t fifo, can_int_t interrupt);

#endif
#endif
