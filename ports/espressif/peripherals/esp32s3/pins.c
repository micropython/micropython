// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 microDev
//
// SPDX-License-Identifier: MIT

#include "peripherals/pins.h"

// NOTE: These numbers do NOT always match the package and module pin number.
// These are by solely by GPIO numbers.
const mcu_pin_obj_t pin_GPIO0 = PIN(0, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO1 = PIN(1, ADC_UNIT_1, ADC_CHANNEL_0, TOUCH_PAD_NUM1);
const mcu_pin_obj_t pin_GPIO2 = PIN(2, ADC_UNIT_1, ADC_CHANNEL_1, TOUCH_PAD_NUM2);
const mcu_pin_obj_t pin_GPIO3 = PIN(3, ADC_UNIT_1, ADC_CHANNEL_2, TOUCH_PAD_NUM3);
const mcu_pin_obj_t pin_GPIO4 = PIN(4, ADC_UNIT_1, ADC_CHANNEL_3, TOUCH_PAD_NUM4);
const mcu_pin_obj_t pin_GPIO5 = PIN(5, ADC_UNIT_1, ADC_CHANNEL_4, TOUCH_PAD_NUM5);
const mcu_pin_obj_t pin_GPIO6 = PIN(6, ADC_UNIT_1, ADC_CHANNEL_5, TOUCH_PAD_NUM6);
const mcu_pin_obj_t pin_GPIO7 = PIN(7, ADC_UNIT_1, ADC_CHANNEL_6, TOUCH_PAD_NUM7);
const mcu_pin_obj_t pin_GPIO8 = PIN(8, ADC_UNIT_1, ADC_CHANNEL_7, TOUCH_PAD_NUM8);
const mcu_pin_obj_t pin_GPIO9 = PIN(9, ADC_UNIT_1, ADC_CHANNEL_8, TOUCH_PAD_NUM9);
const mcu_pin_obj_t pin_GPIO10 = PIN(10, ADC_UNIT_1, ADC_CHANNEL_9, TOUCH_PAD_NUM10);
const mcu_pin_obj_t pin_GPIO11 = PIN(11, ADC_UNIT_2, ADC_CHANNEL_0, TOUCH_PAD_NUM11);
const mcu_pin_obj_t pin_GPIO12 = PIN(12, ADC_UNIT_2, ADC_CHANNEL_1, TOUCH_PAD_NUM12);
const mcu_pin_obj_t pin_GPIO13 = PIN(13, ADC_UNIT_2, ADC_CHANNEL_2, TOUCH_PAD_NUM13);
const mcu_pin_obj_t pin_GPIO14 = PIN(14, ADC_UNIT_2, ADC_CHANNEL_3, TOUCH_PAD_NUM14);
const mcu_pin_obj_t pin_GPIO15 = PIN(15, ADC_UNIT_2, ADC_CHANNEL_4, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO16 = PIN(16, ADC_UNIT_2, ADC_CHANNEL_5, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO17 = PIN(17, ADC_UNIT_2, ADC_CHANNEL_6, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO18 = PIN(18, ADC_UNIT_2, ADC_CHANNEL_7, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO19 = PIN(19, ADC_UNIT_2, ADC_CHANNEL_8, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO20 = PIN(20, ADC_UNIT_2, ADC_CHANNEL_9, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO21 = PIN(21, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO26 = PIN(26, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO27 = PIN(27, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO28 = PIN(28, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO29 = PIN(29, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO30 = PIN(30, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO31 = PIN(31, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO32 = PIN(32, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO33 = PIN(33, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO34 = PIN(34, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO35 = PIN(35, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO36 = PIN(36, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO37 = PIN(37, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO38 = PIN(38, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO39 = PIN(39, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO40 = PIN(40, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO41 = PIN(41, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO42 = PIN(42, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO43 = PIN(43, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO44 = PIN(44, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO45 = PIN(45, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO46 = PIN(46, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO47 = PIN(47, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH_CHANNEL);
const mcu_pin_obj_t pin_GPIO48 = PIN(48, NO_ADC, NO_ADC_CHANNEL, NO_TOUCH_CHANNEL);
