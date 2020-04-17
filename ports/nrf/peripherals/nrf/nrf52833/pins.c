/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Dan Halbert for Adafruit Industries
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

#include "py/obj.h"
#include "py/mphal.h"
#include "nrf/pins.h"

const mcu_pin_obj_t pin_P0_00 = PIN(P0_00, 0, 0, 0);
const mcu_pin_obj_t pin_P0_01 = PIN(P0_01, 0, 1, 0);
const mcu_pin_obj_t pin_P0_02 = PIN(P0_02, 0, 2, SAADC_CH_PSELP_PSELP_AnalogInput0);
const mcu_pin_obj_t pin_P0_03 = PIN(P0_03, 0, 3, SAADC_CH_PSELP_PSELP_AnalogInput1);
const mcu_pin_obj_t pin_P0_04 = PIN(P0_04, 0, 4, SAADC_CH_PSELP_PSELP_AnalogInput2);
const mcu_pin_obj_t pin_P0_05 = PIN(P0_05, 0, 5, SAADC_CH_PSELP_PSELP_AnalogInput3);
const mcu_pin_obj_t pin_P0_06 = PIN(P0_06, 0, 6, 0);
const mcu_pin_obj_t pin_P0_07 = PIN(P0_07, 0, 7, 0);
const mcu_pin_obj_t pin_P0_08 = PIN(P0_08, 0, 8, 0);
const mcu_pin_obj_t pin_P0_09 = PIN(P0_09, 0, 9, 0);
const mcu_pin_obj_t pin_P0_10 = PIN(P0_10, 0, 10, 0);
const mcu_pin_obj_t pin_P0_11 = PIN(P0_11, 0, 11, 0);
const mcu_pin_obj_t pin_P0_12 = PIN(P0_12, 0, 12, 0);
const mcu_pin_obj_t pin_P0_13 = PIN(P0_13, 0, 13, 0);
const mcu_pin_obj_t pin_P0_14 = PIN(P0_14, 0, 14, 0);
const mcu_pin_obj_t pin_P0_15 = PIN(P0_15, 0, 15, 0);
const mcu_pin_obj_t pin_P0_16 = PIN(P0_16, 0, 16, 0);
const mcu_pin_obj_t pin_P0_17 = PIN(P0_17, 0, 17, 0);
const mcu_pin_obj_t pin_P0_18 = PIN(P0_18, 0, 18, 0);
const mcu_pin_obj_t pin_P0_19 = PIN(P0_19, 0, 19, 0);
const mcu_pin_obj_t pin_P0_20 = PIN(P0_20, 0, 20, 0);
const mcu_pin_obj_t pin_P0_21 = PIN(P0_21, 0, 21, 0);
const mcu_pin_obj_t pin_P0_22 = PIN(P0_22, 0, 22, 0);
const mcu_pin_obj_t pin_P0_23 = PIN(P0_23, 0, 23, 0);
const mcu_pin_obj_t pin_P0_24 = PIN(P0_24, 0, 24, 0);
const mcu_pin_obj_t pin_P0_25 = PIN(P0_25, 0, 25, 0);
const mcu_pin_obj_t pin_P0_26 = PIN(P0_26, 0, 26, 0);
const mcu_pin_obj_t pin_P0_27 = PIN(P0_27, 0, 27, 0);
const mcu_pin_obj_t pin_P0_28 = PIN(P0_28, 0, 28, SAADC_CH_PSELP_PSELP_AnalogInput4);
const mcu_pin_obj_t pin_P0_29 = PIN(P0_29, 0, 29, SAADC_CH_PSELP_PSELP_AnalogInput5);
const mcu_pin_obj_t pin_P0_30 = PIN(P0_30, 0, 30, SAADC_CH_PSELP_PSELP_AnalogInput6);
const mcu_pin_obj_t pin_P0_31 = PIN(P0_31, 0, 31, SAADC_CH_PSELP_PSELP_AnalogInput7);
const mcu_pin_obj_t pin_P1_00 = PIN(P1_00, 1, 0, 0);
const mcu_pin_obj_t pin_P1_01 = PIN(P1_01, 1, 1, 0);
const mcu_pin_obj_t pin_P1_02 = PIN(P1_02, 1, 2, 0);
const mcu_pin_obj_t pin_P1_03 = PIN(P1_03, 1, 3, 0);
const mcu_pin_obj_t pin_P1_04 = PIN(P1_04, 1, 4, 0);
const mcu_pin_obj_t pin_P1_05 = PIN(P1_05, 1, 5, 0);
const mcu_pin_obj_t pin_P1_06 = PIN(P1_06, 1, 6, 0);
const mcu_pin_obj_t pin_P1_07 = PIN(P1_07, 1, 7, 0);
const mcu_pin_obj_t pin_P1_08 = PIN(P1_08, 1, 8, 0);
const mcu_pin_obj_t pin_P1_09 = PIN(P1_09, 1, 9, 0);
