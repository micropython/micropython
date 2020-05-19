 /*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Lucian Copeland for Adafruit Industries
 * Copyright (c) 2019 Artur Pacholec
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
#include "mimxrt10xx/pins.h"

const mcu_pin_obj_t pin_GPIO_00 = PIN(GPIO1, 0, 0, GPIO_00, NO_ADC, 0, 0x00000005, 0x000010A0);
const mcu_pin_obj_t pin_GPIO_01 = PIN(GPIO1, 0, 1, GPIO_01, NO_ADC, 0, 0x00000005, 0x000010A0);
const mcu_pin_obj_t pin_GPIO_02 = PIN(GPIO1, 0, 2, GPIO_02, NO_ADC, 0, 0x00000005, 0x000010A0);
const mcu_pin_obj_t pin_GPIO_03 = PIN(GPIO1, 0, 3, GPIO_03, NO_ADC, 0, 0x00000005, 0x000010A0);
const mcu_pin_obj_t pin_GPIO_04 = PIN(GPIO1, 0, 4, GPIO_04, NO_ADC, 0, 0x00000005, 0x000010A0);
const mcu_pin_obj_t pin_GPIO_05 = PIN(GPIO1, 0, 5, GPIO_05, NO_ADC, 0, 0x00000005, 0x000010A0);
const mcu_pin_obj_t pin_GPIO_06 = PIN(GPIO1, 0, 6, GPIO_06, NO_ADC, 0, 0x00000005, 0x000010A0);
const mcu_pin_obj_t pin_GPIO_07 = PIN(GPIO1, 0, 7, GPIO_07, NO_ADC, 0, 0x00000005, 0x000010A0);
const mcu_pin_obj_t pin_GPIO_08 = PIN(GPIO1, 0, 8, GPIO_08, NO_ADC, 0, 0x00000005, 0x000010A0);
const mcu_pin_obj_t pin_GPIO_09 = PIN(GPIO1, 0, 9, GPIO_09, NO_ADC, 0, 0x00000005, 0x000010A0);
const mcu_pin_obj_t pin_GPIO_10 = PIN(GPIO1, 0, 10, GPIO_10, NO_ADC, 0, 0x00000005, 0x000010A0);
const mcu_pin_obj_t pin_GPIO_11 = PIN(GPIO1, 0, 11, GPIO_11, NO_ADC, 0, 0x00000005, 0x000010A0);
const mcu_pin_obj_t pin_GPIO_12 = PIN(GPIO1, 0, 12, GPIO_12, NO_ADC, 0, 0x00000005, 0x000010A0);
const mcu_pin_obj_t pin_GPIO_13 = PIN(GPIO1, 0, 13, GPIO_13, NO_ADC, 0, 0x00000005, 0x000010A0);

const mcu_pin_obj_t pin_GPIO_AD_00 = PIN(GPIO1, 0, 14, GPIO_AD_00, ADC1, 0, 0x00000005, 0x000010A0);
const mcu_pin_obj_t pin_GPIO_AD_01 = PIN(GPIO1, 0, 15, GPIO_AD_01, ADC1, 1, 0x00000005, 0x000010A0);
const mcu_pin_obj_t pin_GPIO_AD_02 = PIN(GPIO1, 0, 16, GPIO_AD_02, ADC1, 2, 0x00000005, 0x000010A0);
const mcu_pin_obj_t pin_GPIO_AD_03 = PIN(GPIO1, 0, 17, GPIO_AD_03, ADC1, 3, 0x00000005, 0x000010A0);
const mcu_pin_obj_t pin_GPIO_AD_04 = PIN(GPIO1, 0, 18, GPIO_AD_04, ADC1, 4, 0x00000005, 0x000010A0);
const mcu_pin_obj_t pin_GPIO_AD_05 = PIN(GPIO1, 0, 19, GPIO_AD_05, ADC1, 5, 0x00000005, 0x000010A0);
const mcu_pin_obj_t pin_GPIO_AD_06 = PIN(GPIO1, 0, 20, GPIO_AD_06, ADC1, 6, 0x00000005, 0x000010A0);
const mcu_pin_obj_t pin_GPIO_AD_07 = PIN(GPIO1, 0, 21, GPIO_AD_07, ADC1, 7, 0x00000005, 0x000010A0);
const mcu_pin_obj_t pin_GPIO_AD_08 = PIN(GPIO1, 0, 22, GPIO_AD_08, ADC1, 8, 0x00000007, 0x000070A0);
const mcu_pin_obj_t pin_GPIO_AD_09 = PIN(GPIO1, 0, 23, GPIO_AD_09, ADC1, 9, 0x00000007, 0x000090B1);
const mcu_pin_obj_t pin_GPIO_AD_10 = PIN(GPIO1, 0, 24, GPIO_AD_10, ADC1, 10, 0x00000007, 0x000070A0);
const mcu_pin_obj_t pin_GPIO_AD_11 = PIN(GPIO1, 0, 25, GPIO_AD_11, ADC1, 11, 0x00000007, 0x000030A0);
const mcu_pin_obj_t pin_GPIO_AD_12 = PIN(GPIO1, 0, 26, GPIO_AD_12, ADC1, 12, 0x00000007, 0x000030A0);
const mcu_pin_obj_t pin_GPIO_AD_13 = PIN(GPIO1, 0, 27, GPIO_AD_13, ADC1, 13, 0x00000007, 0x000070A0);
const mcu_pin_obj_t pin_GPIO_AD_14 = PIN(GPIO1, 0, 28, GPIO_AD_14, ADC1, 14, 0x00000005, 0x000010A0);

const mcu_pin_obj_t pin_GPIO_SD_00 = PIN(GPIO2, 1, 0, GPIO_SD_00, NO_ADC, 0, 0x00000005, 0x000010A0);
const mcu_pin_obj_t pin_GPIO_SD_01 = PIN(GPIO2, 1, 1, GPIO_SD_01, NO_ADC, 0, 0x00000005, 0x000010A0);
const mcu_pin_obj_t pin_GPIO_SD_02 = PIN(GPIO2, 1, 2, GPIO_SD_02, NO_ADC, 0, 0x00000005, 0x000010A0);
const mcu_pin_obj_t pin_GPIO_SD_03 = PIN(GPIO2, 1, 3, GPIO_SD_03, NO_ADC, 0, 0x00000006, 0x000030A0);
const mcu_pin_obj_t pin_GPIO_SD_04 = PIN(GPIO2, 1, 4, GPIO_SD_04, NO_ADC, 0, 0x00000006, 0x000030A0);
const mcu_pin_obj_t pin_GPIO_SD_05 = PIN(GPIO2, 1, 5, GPIO_SD_05, NO_ADC, 0, 0x00000005, 0x000010A0);
const mcu_pin_obj_t pin_GPIO_SD_06 = PIN(GPIO2, 1, 6, GPIO_SD_06, NO_ADC, 0, 0x00000005, 0x000010A0);
const mcu_pin_obj_t pin_GPIO_SD_07 = PIN(GPIO2, 1, 7, GPIO_SD_07, NO_ADC, 0, 0x00000005, 0x000010A0);
const mcu_pin_obj_t pin_GPIO_SD_08 = PIN(GPIO2, 1, 8, GPIO_SD_08, NO_ADC, 0, 0x00000005, 0x000010A0);
const mcu_pin_obj_t pin_GPIO_SD_09 = PIN(GPIO2, 1, 9, GPIO_SD_09, NO_ADC, 0, 0x00000005, 0x000010A0);
const mcu_pin_obj_t pin_GPIO_SD_10 = PIN(GPIO2, 1, 10, GPIO_SD_10, NO_ADC, 0, 0x00000005, 0x000010A0);
const mcu_pin_obj_t pin_GPIO_SD_11 = PIN(GPIO2, 1, 11, GPIO_SD_11, NO_ADC, 0, 0x00000005, 0x000010A0);
const mcu_pin_obj_t pin_GPIO_SD_12 = PIN(GPIO2, 1, 12, GPIO_SD_12, NO_ADC, 0, 0x00000005, 0x000010A0);
const mcu_pin_obj_t pin_GPIO_SD_13 = PIN(GPIO2, 1, 13, GPIO_SD_13, NO_ADC, 0, 0x00000005, 0x000010A0);
const mcu_pin_obj_t pin_GPIO_SD_14 = PIN(GPIO2, 1, 14, GPIO_SD_14, NO_ADC, 0, 0x00000000, 0x000010A0);
// pin_GPIO_SD_14 isn't actually used as a pad but we include it anyway to make resetting easier
