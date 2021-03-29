/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017, 2018 Scott Shawcroft for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_DISPLAYBUSIO_FOURWIRE_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_DISPLAYBUSIO_FOURWIRE_H

#include "shared-module/displayio/FourWire.h"

#include "shared-bindings/displayio/__init__.h"
#include "common-hal/microcontroller/Pin.h"

#include "shared-module/displayio/Group.h"

extern const mp_obj_type_t displayio_fourwire_type;

void common_hal_displayio_fourwire_construct(displayio_fourwire_obj_t *self,
    busio_spi_obj_t *spi, const mcu_pin_obj_t *command,
    const mcu_pin_obj_t *chip_select, const mcu_pin_obj_t *reset, uint32_t baudrate,
    uint8_t polarity, uint8_t phase);

void common_hal_displayio_fourwire_deinit(displayio_fourwire_obj_t *self);

bool common_hal_displayio_fourwire_reset(mp_obj_t self);
bool common_hal_displayio_fourwire_bus_free(mp_obj_t self);

bool common_hal_displayio_fourwire_begin_transaction(mp_obj_t self);

void common_hal_displayio_fourwire_send(mp_obj_t self, display_byte_type_t byte_type,
    display_chip_select_behavior_t chip_select, const uint8_t *data, uint32_t data_length);

void common_hal_displayio_fourwire_end_transaction(mp_obj_t self);

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_DISPLAYBUSIO_FOURWIRE_H
