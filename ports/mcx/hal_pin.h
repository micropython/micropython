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

#ifndef MP_PORT_MCX_HAL_PIN_H
#define MP_PORT_MCX_HAL_PIN_H

#include "drv_pin.h"

#include "machine_pin.h"

#define mp_hal_pin_obj_t machine_pin_obj_t *
#define mp_hal_get_pin_obj(p) (p)

#define MP_HAL_PIN_MODE_ANALOG          (DRV_PIN_MODE_ANALOG)
#define MP_HAL_PIN_MODE_INPUT           (DRV_PIN_MODE_INPUT)
#define MP_HAL_PIN_MODE_OUTPUT          (DRV_PIN_MODE_OUTPUT_PP)
#define MP_HAL_PIN_MODE_OPEN_DRAIN      (DRV_PIN_MODE_OUTPUT_OD)
#define MP_HAL_PIN_MODE_ALT             (DRV_PIN_MODE_AF_PP)
#define MP_HAL_PIN_MODE_ALT_OPEN_DRAIN  (DRV_PIN_MODE_AF_OD)

#define MP_HAL_PIN_PULL_NONE            (DRV_PIN_PULL_NONE)
#define MP_HAL_PIN_PULL_UP              (DRV_PIN_PULL_UP)
#define MP_HAL_PIN_PULL_DOWN            (DRV_PIN_PULL_DOWN)

#define MP_HAL_PIN_TRIGGER_FALLING      (GPIO_IRQ_FALLING)
#define MP_HAL_PIN_TRIGGER_RISING       (GPIO_IRQ_RISING)
#define MP_HAL_PIN_TRIGGER_LOWLEVEL     (GPIO_IRQ_LOWLEVEL)
#define MP_HAL_PIN_TRIGGER_HIGHLEVEL    (GPIO_IRQ_HIGHLEVEL)

#define MP_HAL_PIN_DRIVE_0              (DRV_PIN_DRIVE_LOW)
#define MP_HAL_PIN_DRIVE_1              (DRV_PIN_DRIVE_HIGH)

const mp_hal_pin_obj_t mp_hal_pin_find_named_pin(const mp_obj_dict_t *named_pins, mp_obj_t name);
const machine_pin_af_t *mp_hal_pin_find_af(const mp_hal_pin_obj_t pin, machine_pin_af_type_t type,
    uint8_t idx, machine_pin_af_attr_t attr);
const machine_pin_adc_t *mp_hal_pin_find_adc(const mp_hal_pin_obj_t pin);
const machine_pin_af_t *mp_hal_pin_find_pwm(const mp_hal_pin_obj_t pin);

void mp_hal_pin_high(mp_hal_pin_obj_t pin);
void mp_hal_pin_od_high(mp_hal_pin_obj_t pin);

void mp_hal_pin_low(mp_hal_pin_obj_t pin);
void mp_hal_pin_od_low(mp_hal_pin_obj_t pin);

bool mp_hal_pin_read(mp_hal_pin_obj_t pin);
void mp_hal_pin_write(mp_hal_pin_obj_t pin, bool value);

void mp_hal_pin_open_drain(mp_hal_pin_obj_t pin);
void mp_hal_pin_output(mp_hal_pin_obj_t pin);
void mp_hal_pin_input(mp_hal_pin_obj_t pin);
void mp_hal_pin_analog(mp_hal_pin_obj_t pin);

void mp_hal_pin_af(mp_hal_pin_obj_t pin, uint8_t af_num);
void mp_hal_pin_af_od(mp_hal_pin_obj_t pin, uint8_t af_num);

void mp_hal_pin_pull_up(mp_hal_pin_obj_t pin);
void mp_hal_pin_pull_down(mp_hal_pin_obj_t pin);
void mp_hal_pin_pull_none(mp_hal_pin_obj_t pin);


#endif
