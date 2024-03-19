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

#ifndef MP_PORT_MCX_DRV_PIN_H
#define MP_PORT_MCX_DRV_PIN_H

#include "py/obj.h"

typedef enum {
    DRV_PIN_PULL_NONE,
    DRV_PIN_PULL_UP,
    DRV_PIN_PULL_DOWN,

    DRV_PIN_PULL_END,
} drv_pin_pull_type_t;

typedef enum {
    DRV_PIN_DRIVE_LOW,
    DRV_PIN_DRIVE_HIGH,

    DRV_PIN_DRIVE_END,
} drv_pin_drive_t;

typedef enum {
    DRV_PIN_MODE_ANALOG = 0U,
    DRV_PIN_MODE_INPUT,
    DRV_PIN_MODE_OUTPUT_PP,
    DRV_PIN_MODE_OUTPUT_OD,
    DRV_PIN_MODE_AF_PP,
    DRV_PIN_MODE_AF_OD,

    DRV_PIN_MODE_END,
} drv_pin_mode_t;

typedef struct drv_pin_config_type {
    drv_pin_mode_t mode;
    drv_pin_pull_type_t pull;
    drv_pin_drive_t drive;

    uint8_t af;
} drv_pin_config_t;

typedef struct drv_pin_type {
    uint8_t port;
    uint8_t pin;
} drv_pin_t;

int drv_pin_init(drv_pin_t *pin);
int drv_pin_config_set(drv_pin_t *pin, drv_pin_config_t *cfg);
int drv_pin_config_get(drv_pin_t *pin, drv_pin_config_t *cfg);
int drv_pin_drive_set(drv_pin_t *pin, drv_pin_drive_t drive);
int drv_pin_drive_get(drv_pin_t *pin, drv_pin_drive_t *drive);
int drv_pin_pull_set(drv_pin_t *pin, drv_pin_pull_type_t pull);
int drv_pin_pull_get(drv_pin_t *pin, drv_pin_pull_type_t *pull);
int drv_pin_mode_set(drv_pin_t *pin, drv_pin_mode_t mode, uint8_t af);
int drv_pin_mode_get(drv_pin_t *pin, drv_pin_mode_t *mode, uint8_t *af);
int drv_pin_read(drv_pin_t *pin);
int drv_pin_write(drv_pin_t *pin, bool value);

#endif
