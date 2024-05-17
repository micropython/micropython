// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "py/mphal.h"
#include "peripherals/pins.h"

const mcu_pin_obj_t pin_PA00 = PIN(0, 0, ADC_INPUT(ADC_1, 0));
const mcu_pin_obj_t pin_PA01 = PIN(0, 1, ADC_INPUT(ADC_1, 1));
const mcu_pin_obj_t pin_PA02 = PIN(0, 2, ADC_INPUT(ADC_1, 2));
const mcu_pin_obj_t pin_PA03 = PIN(0, 3, ADC_INPUT(ADC_1, 3));
const mcu_pin_obj_t pin_PA04 = PIN(0, 4, ADC_INPUT(ADC_1, 4));
const mcu_pin_obj_t pin_PA05 = PIN(0, 5, ADC_INPUT(ADC_1, 5));
const mcu_pin_obj_t pin_PA06 = PIN(0, 6, ADC_INPUT(ADC_1, 6));
const mcu_pin_obj_t pin_PA07 = PIN(0, 7, ADC_INPUT(ADC_1, 7));
const mcu_pin_obj_t pin_PA08 = PIN(0, 8, NO_ADC);
const mcu_pin_obj_t pin_PA09 = PIN(0, 9, NO_ADC);
const mcu_pin_obj_t pin_PA10 = PIN(0, 10, NO_ADC);
const mcu_pin_obj_t pin_PA11 = PIN(0, 11, NO_ADC);
const mcu_pin_obj_t pin_PA12 = PIN(0, 12, NO_ADC);
const mcu_pin_obj_t pin_PA13 = PIN(0, 13, NO_ADC);
const mcu_pin_obj_t pin_PA14 = PIN(0, 14, NO_ADC);
const mcu_pin_obj_t pin_PA15 = PIN(0, 15, NO_ADC);

const mcu_pin_obj_t pin_PB00 = PIN(1, 0, ADC_INPUT(ADC_1, 8));
const mcu_pin_obj_t pin_PB01 = PIN(1, 1, ADC_INPUT(ADC_1, 9));
const mcu_pin_obj_t pin_PB02 = PIN(1, 2, NO_ADC);
const mcu_pin_obj_t pin_PB03 = PIN(1, 3, NO_ADC);
const mcu_pin_obj_t pin_PB04 = PIN(1, 4, NO_ADC);
const mcu_pin_obj_t pin_PB05 = PIN(1, 5, NO_ADC);
const mcu_pin_obj_t pin_PB06 = PIN(1, 6, NO_ADC);
const mcu_pin_obj_t pin_PB07 = PIN(1, 7, NO_ADC);
const mcu_pin_obj_t pin_PB08 = PIN(1, 8, NO_ADC);
const mcu_pin_obj_t pin_PB09 = PIN(1, 9, NO_ADC);
const mcu_pin_obj_t pin_PB10 = PIN(1, 10, NO_ADC);
const mcu_pin_obj_t pin_PB12 = PIN(1, 12, NO_ADC);
const mcu_pin_obj_t pin_PB13 = PIN(1, 13, NO_ADC);
const mcu_pin_obj_t pin_PB14 = PIN(1, 14, NO_ADC);
const mcu_pin_obj_t pin_PB15 = PIN(1, 15, NO_ADC);

const mcu_pin_obj_t pin_PC13 = PIN(2, 13, NO_ADC); // anti-tamp
const mcu_pin_obj_t pin_PC14 = PIN(2, 14, NO_ADC); // OSC32_IN
const mcu_pin_obj_t pin_PC15 = PIN(2, 15, NO_ADC); // OSC32_OUT
