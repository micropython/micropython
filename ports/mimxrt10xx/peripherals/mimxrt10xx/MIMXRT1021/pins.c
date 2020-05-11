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

const mcu_pin_obj_t pin_GPIO_AD_B0_00 = PIN(GPIO1, 0, 0, GPIO_AD_B0_00, NO_ADC, 0, 0x00000000, 0x000070A0);
const mcu_pin_obj_t pin_GPIO_AD_B0_01 = PIN(GPIO1, 0, 1, GPIO_AD_B0_01, NO_ADC, 0, 0x00000000, 0x000030A0);
const mcu_pin_obj_t pin_GPIO_AD_B0_02 = PIN(GPIO1, 0, 2, GPIO_AD_B0_02, NO_ADC, 0, 0x00000000, 0x000030A0);
const mcu_pin_obj_t pin_GPIO_AD_B0_03 = PIN(GPIO1, 0, 3, GPIO_AD_B0_03, NO_ADC, 0, 0x00000000, 0x000070A0);
const mcu_pin_obj_t pin_GPIO_AD_B0_04 = PIN(GPIO1, 0, 4, GPIO_AD_B0_04, NO_ADC, 0, 0x00000000, 0x000090B1);
const mcu_pin_obj_t pin_GPIO_AD_B0_05 = PIN(GPIO1, 0, 5, GPIO_AD_B0_05, NO_ADC, 0, 0x00000000, 0x000070A0);
const mcu_pin_obj_t pin_GPIO_AD_B0_06 = PIN(GPIO1, 0, 6, GPIO_AD_B0_06, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_AD_B0_07 = PIN(GPIO1, 0, 7, GPIO_AD_B0_07, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_AD_B0_08 = PIN(GPIO1, 0, 8, GPIO_AD_B0_08, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_AD_B0_09 = PIN(GPIO1, 0, 9, GPIO_AD_B0_09, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_AD_B0_10 = PIN(GPIO1, 0, 10, GPIO_AD_B0_10, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_AD_B0_11 = PIN(GPIO1, 0, 11, GPIO_AD_B0_11, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_AD_B0_12 = PIN(GPIO1, 0, 12, GPIO_AD_B0_12, ADC1, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_AD_B0_13 = PIN(GPIO1, 0, 13, GPIO_AD_B0_13, ADC2, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_AD_B0_14 = PIN(GPIO1, 0, 14, GPIO_AD_B0_14, ADC2, 1, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_AD_B0_15 = PIN(GPIO1, 0, 15, GPIO_AD_B0_15, ADC1, 2, 0x00000005, 0x000010B0);

const mcu_pin_obj_t pin_GPIO_AD_B1_00 = PIN(GPIO1, 0, 16, GPIO_AD_B1_00, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_AD_B1_01 = PIN(GPIO1, 0, 17, GPIO_AD_B1_01, ADC1, 3, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_AD_B1_02 = PIN(GPIO1, 0, 18, GPIO_AD_B1_02, ADC2, 3, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_AD_B1_03 = PIN(GPIO1, 0, 19, GPIO_AD_B1_03, ADC1, 4, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_AD_B1_04 = PIN(GPIO1, 0, 20, GPIO_AD_B1_04, ADC2, 4, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_AD_B1_05 = PIN(GPIO1, 0, 21, GPIO_AD_B1_05, ADC2, 5, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_AD_B1_06 = PIN(GPIO1, 0, 22, GPIO_AD_B1_06, ADC2, 6, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_AD_B1_07 = PIN(GPIO1, 0, 23, GPIO_AD_B1_07, ADC2, 7, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_AD_B1_08 = PIN(GPIO1, 0, 24, GPIO_AD_B1_08, ADC2, 8, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_AD_B1_09 = PIN(GPIO1, 0, 25, GPIO_AD_B1_09, ADC2, 9, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_AD_B1_10 = PIN(GPIO1, 0, 26, GPIO_AD_B1_10, ADC2, 10, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_AD_B1_11 = PIN(GPIO1, 0, 27, GPIO_AD_B1_11, ADC2, 11, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_AD_B1_12 = PIN(GPIO1, 0, 28, GPIO_AD_B1_12, ADC2, 12, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_AD_B1_13 = PIN(GPIO1, 0, 29, GPIO_AD_B1_13, ADC2, 13, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_AD_B1_14 = PIN(GPIO1, 0, 30, GPIO_AD_B1_14, ADC2, 14, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_AD_B1_15 = PIN(GPIO1, 0, 31, GPIO_AD_B1_15, ADC2, 15, 0x00000005, 0x000010B0);

const mcu_pin_obj_t pin_GPIO_EMC_00 = PIN(GPIO2, 1, 0, GPIO_EMC_00, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_EMC_01 = PIN(GPIO2, 1, 1, GPIO_EMC_01, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_EMC_02 = PIN(GPIO2, 1, 2, GPIO_EMC_02, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_EMC_03 = PIN(GPIO2, 1, 3, GPIO_EMC_03, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_EMC_04 = PIN(GPIO2, 1, 4, GPIO_EMC_04, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_EMC_05 = PIN(GPIO2, 1, 5, GPIO_EMC_05, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_EMC_06 = PIN(GPIO2, 1, 6, GPIO_EMC_06, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_EMC_07 = PIN(GPIO2, 1, 7, GPIO_EMC_07, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_EMC_08 = PIN(GPIO2, 1, 8, GPIO_EMC_08, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_EMC_09 = PIN(GPIO2, 1, 9, GPIO_EMC_09, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_EMC_10 = PIN(GPIO2, 1, 10, GPIO_EMC_10, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_EMC_11 = PIN(GPIO2, 1, 11, GPIO_EMC_11, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_EMC_12 = PIN(GPIO2, 1, 12, GPIO_EMC_12, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_EMC_13 = PIN(GPIO2, 1, 13, GPIO_EMC_13, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_EMC_14 = PIN(GPIO2, 1, 14, GPIO_EMC_14, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_EMC_15 = PIN(GPIO2, 1, 15, GPIO_EMC_15, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_EMC_16 = PIN(GPIO2, 1, 16, GPIO_EMC_16, NO_ADC, 0, 0x00000006, 0x000030B0);
const mcu_pin_obj_t pin_GPIO_EMC_17 = PIN(GPIO2, 1, 17, GPIO_EMC_17, NO_ADC, 0, 0x00000006, 0x000030B0);
const mcu_pin_obj_t pin_GPIO_EMC_18 = PIN(GPIO2, 1, 18, GPIO_EMC_18, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_EMC_19 = PIN(GPIO2, 1, 19, GPIO_EMC_19, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_EMC_20 = PIN(GPIO2, 1, 20, GPIO_EMC_20, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_EMC_21 = PIN(GPIO2, 1, 21, GPIO_EMC_21, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_EMC_22 = PIN(GPIO2, 1, 22, GPIO_EMC_22, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_EMC_23 = PIN(GPIO2, 1, 23, GPIO_EMC_23, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_EMC_24 = PIN(GPIO2, 1, 24, GPIO_EMC_24, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_EMC_25 = PIN(GPIO2, 1, 25, GPIO_EMC_25, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_EMC_26 = PIN(GPIO2, 1, 26, GPIO_EMC_26, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_EMC_27 = PIN(GPIO2, 1, 27, GPIO_EMC_27, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_EMC_28 = PIN(GPIO2, 1, 28, GPIO_EMC_28, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_EMC_29 = PIN(GPIO2, 1, 29, GPIO_EMC_29, NO_ADC, 0, 0x00000005, 0x000030B0);
const mcu_pin_obj_t pin_GPIO_EMC_30 = PIN(GPIO2, 1, 30, GPIO_EMC_30, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_EMC_31 = PIN(GPIO2, 1, 31, GPIO_EMC_31, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_EMC_32 = PIN(GPIO3, 1, 0, GPIO_EMC_32, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_EMC_33 = PIN(GPIO3, 1, 1, GPIO_EMC_33, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_EMC_34 = PIN(GPIO3, 1, 2, GPIO_EMC_34, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_EMC_35 = PIN(GPIO3, 1, 3, GPIO_EMC_35, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_EMC_36 = PIN(GPIO3, 1, 4, GPIO_EMC_36, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_EMC_37 = PIN(GPIO3, 1, 5, GPIO_EMC_37, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_EMC_38 = PIN(GPIO3, 1, 6, GPIO_EMC_38, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_EMC_39 = PIN(GPIO3, 1, 7, GPIO_EMC_39, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_EMC_40 = PIN(GPIO3, 1, 8, GPIO_EMC_40, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_EMC_41 = PIN(GPIO3, 1, 9, GPIO_EMC_41, NO_ADC, 0, 0x00000005, 0x000010B0);

const mcu_pin_obj_t pin_GPIO_SD_B0_00 = PIN(GPIO3, 2, 13, GPIO_SD_B0_00, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_SD_B0_01 = PIN(GPIO3, 2, 14, GPIO_SD_B0_01, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_SD_B0_02 = PIN(GPIO3, 2, 15, GPIO_SD_B0_02, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_SD_B0_03 = PIN(GPIO3, 2, 16, GPIO_SD_B0_03, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_SD_B0_04 = PIN(GPIO3, 2, 17, GPIO_SD_B0_04, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_SD_B0_05 = PIN(GPIO3, 2, 18, GPIO_SD_B0_05, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_SD_B0_06 = PIN(GPIO3, 2, 19, GPIO_SD_B0_06, NO_ADC, 0, 0x00000005, 0x000010B0);

const mcu_pin_obj_t pin_GPIO_SD_B1_00 = PIN(GPIO3, 2, 20, GPIO_SD_B1_00, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_SD_B1_01 = PIN(GPIO3, 2, 21, GPIO_SD_B1_01, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_SD_B1_02 = PIN(GPIO3, 2, 22, GPIO_SD_B1_02, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_SD_B1_03 = PIN(GPIO3, 2, 23, GPIO_SD_B1_03, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_SD_B1_04 = PIN(GPIO3, 2, 24, GPIO_SD_B1_04, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_SD_B1_05 = PIN(GPIO3, 2, 25, GPIO_SD_B1_05, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_SD_B1_06 = PIN(GPIO3, 2, 26, GPIO_SD_B1_06, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_SD_B1_07 = PIN(GPIO3, 2, 27, GPIO_SD_B1_07, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_SD_B1_08 = PIN(GPIO3, 2, 28, GPIO_SD_B1_08, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_SD_B1_09 = PIN(GPIO3, 2, 29, GPIO_SD_B1_09, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_SD_B1_10 = PIN(GPIO3, 2, 30, GPIO_SD_B1_10, NO_ADC, 0, 0x00000005, 0x000010B0);
const mcu_pin_obj_t pin_GPIO_SD_B1_11 = PIN(GPIO3, 2, 31, GPIO_SD_B1_11, NO_ADC, 0, 0x00000005, 0x000010B0);

const mcu_pin_obj_t mcu_pin_list[IOMUXC_SW_PAD_CTL_PAD_COUNT] = {
    pin_GPIO_EMC_00,
    pin_GPIO_EMC_01,
    pin_GPIO_EMC_02,
    pin_GPIO_EMC_03,
    pin_GPIO_EMC_04,
    pin_GPIO_EMC_05,
    pin_GPIO_EMC_06,
    pin_GPIO_EMC_07,
    pin_GPIO_EMC_08,
    pin_GPIO_EMC_09,
    pin_GPIO_EMC_10,
    pin_GPIO_EMC_11,
    pin_GPIO_EMC_12,
    pin_GPIO_EMC_13,
    pin_GPIO_EMC_14,
    pin_GPIO_EMC_15,
    pin_GPIO_EMC_16,
    pin_GPIO_EMC_17,
    pin_GPIO_EMC_18,
    pin_GPIO_EMC_19,
    pin_GPIO_EMC_20,
    pin_GPIO_EMC_21,
    pin_GPIO_EMC_22,
    pin_GPIO_EMC_23,
    pin_GPIO_EMC_24,
    pin_GPIO_EMC_25,
    pin_GPIO_EMC_26,
    pin_GPIO_EMC_27,
    pin_GPIO_EMC_28,
    pin_GPIO_EMC_29,
    pin_GPIO_EMC_30,
    pin_GPIO_EMC_31,
    pin_GPIO_EMC_32,
    pin_GPIO_EMC_33,
    pin_GPIO_EMC_34,
    pin_GPIO_EMC_35,
    pin_GPIO_EMC_36,
    pin_GPIO_EMC_37,
    pin_GPIO_EMC_38,
    pin_GPIO_EMC_39,
    pin_GPIO_EMC_40,
    pin_GPIO_EMC_41,
    pin_GPIO_AD_B0_00,
    pin_GPIO_AD_B0_01,
    pin_GPIO_AD_B0_02,
    pin_GPIO_AD_B0_03,
    pin_GPIO_AD_B0_04,
    pin_GPIO_AD_B0_05,
    pin_GPIO_AD_B0_06,
    pin_GPIO_AD_B0_07,
    pin_GPIO_AD_B0_08,
    pin_GPIO_AD_B0_09,
    pin_GPIO_AD_B0_10,
    pin_GPIO_AD_B0_11,
    pin_GPIO_AD_B0_12,
    pin_GPIO_AD_B0_13,
    pin_GPIO_AD_B0_14,
    pin_GPIO_AD_B0_15,
    pin_GPIO_AD_B1_00,
    pin_GPIO_AD_B1_01,
    pin_GPIO_AD_B1_02,
    pin_GPIO_AD_B1_03,
    pin_GPIO_AD_B1_04,
    pin_GPIO_AD_B1_05,
    pin_GPIO_AD_B1_06,
    pin_GPIO_AD_B1_07,
    pin_GPIO_AD_B1_08,
    pin_GPIO_AD_B1_09,
    pin_GPIO_AD_B1_10,
    pin_GPIO_AD_B1_11,
    pin_GPIO_AD_B1_12,
    pin_GPIO_AD_B1_13,
    pin_GPIO_AD_B1_14,
    pin_GPIO_AD_B1_15,
    pin_GPIO_SD_B0_00,
    pin_GPIO_SD_B0_01,
    pin_GPIO_SD_B0_02,
    pin_GPIO_SD_B0_03,
    pin_GPIO_SD_B0_04,
    pin_GPIO_SD_B0_05,
    pin_GPIO_SD_B0_06,
    pin_GPIO_SD_B1_00,
    pin_GPIO_SD_B1_01,
    pin_GPIO_SD_B1_02,
    pin_GPIO_SD_B1_03,
    pin_GPIO_SD_B1_04,
    pin_GPIO_SD_B1_05,
    pin_GPIO_SD_B1_06,
    pin_GPIO_SD_B1_07,
    pin_GPIO_SD_B1_08,
    pin_GPIO_SD_B1_09,
    pin_GPIO_SD_B1_10,
    pin_GPIO_SD_B1_11,
};
