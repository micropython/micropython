/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Noralf Trønnes
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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_BUSIO_I2C_SLAVE_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_BUSIO_I2C_SLAVE_H

#include "py/obj.h"

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/i2cperipheral/I2CPeripheral.h"

typedef struct {
    mp_obj_base_t base;
    i2cperipheral_i2c_peripheral_obj_t *peripheral;
    uint16_t address;
    bool is_read;
    bool is_restart;
} i2cperipheral_i2c_peripheral_request_obj_t;

extern const mp_obj_type_t i2cperipheral_i2c_peripheral_request_type;

extern const mp_obj_type_t i2cperipheral_i2c_peripheral_type;

extern void common_hal_i2cperipheral_i2c_peripheral_construct(i2cperipheral_i2c_peripheral_obj_t *self,
    const mcu_pin_obj_t *scl, const mcu_pin_obj_t *sda,
    uint8_t *addresses, unsigned int num_addresses, bool smbus);
extern void common_hal_i2cperipheral_i2c_peripheral_deinit(i2cperipheral_i2c_peripheral_obj_t *self);
extern bool common_hal_i2cperipheral_i2c_peripheral_deinited(i2cperipheral_i2c_peripheral_obj_t *self);

extern int common_hal_i2cperipheral_i2c_peripheral_is_addressed(i2cperipheral_i2c_peripheral_obj_t *self,
    uint8_t *address, bool *is_read, bool *is_restart);
extern int common_hal_i2cperipheral_i2c_peripheral_read_byte(i2cperipheral_i2c_peripheral_obj_t *self, uint8_t *data);
extern int common_hal_i2cperipheral_i2c_peripheral_write_byte(i2cperipheral_i2c_peripheral_obj_t *self, uint8_t data);
extern void common_hal_i2cperipheral_i2c_peripheral_ack(i2cperipheral_i2c_peripheral_obj_t *self, bool ack);
extern void common_hal_i2cperipheral_i2c_peripheral_close(i2cperipheral_i2c_peripheral_obj_t *self);

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_BUSIO_I2C_SLAVE_H
