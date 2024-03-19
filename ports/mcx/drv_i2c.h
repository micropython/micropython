/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 NXP
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

#ifndef MP_PORT_MCX_DRV_I2C_H
#define MP_PORT_MCX_DRV_I2C_H

#include "py/obj.h"

typedef enum {
    DRV_I2CDirectionRead,
    DRV_I2CDirectionWrite,
} drv_i2c_xfer_direction_t;

typedef struct {
    uint32_t frequency;
} drv_i2c_config_t;

typedef struct {
    uint8_t id;
    void *instance;
} drv_i2c_t;

int drv_i2c_init(drv_i2c_t *i2c, uint8_t id, drv_i2c_config_t *cfg);
int drv_i2c_write(drv_i2c_t *i2c, uint16_t addr, uint8_t *data, uint32_t len, uint32_t timeout, bool stop);
int drv_i2c_read(drv_i2c_t *i2c, uint16_t addr, uint8_t *data, uint32_t len, uint32_t timeout, bool stop);

#endif
