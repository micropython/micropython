// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// DO NOT include this file directly. Use shared-bindings/microcontroller/Pin.h instead to ensure
// that all necessary includes are already included.

#pragma once

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    uint8_t number;
} mcu_pin_obj_t;

extern const mcu_pin_obj_t pin_GPIO0;
extern const mcu_pin_obj_t pin_GPIO1;
extern const mcu_pin_obj_t pin_GPIO2;
extern const mcu_pin_obj_t pin_GPIO3;
extern const mcu_pin_obj_t pin_GPIO4;
extern const mcu_pin_obj_t pin_GPIO5;
extern const mcu_pin_obj_t pin_GPIO6;
extern const mcu_pin_obj_t pin_GPIO7;
extern const mcu_pin_obj_t pin_GPIO8;
extern const mcu_pin_obj_t pin_GPIO9;
extern const mcu_pin_obj_t pin_GPIO10;
extern const mcu_pin_obj_t pin_GPIO11;
extern const mcu_pin_obj_t pin_GPIO12;
extern const mcu_pin_obj_t pin_GPIO13;
extern const mcu_pin_obj_t pin_GPIO14;
extern const mcu_pin_obj_t pin_GPIO15;
extern const mcu_pin_obj_t pin_GPIO16;
extern const mcu_pin_obj_t pin_GPIO17;
extern const mcu_pin_obj_t pin_GPIO18;
extern const mcu_pin_obj_t pin_GPIO19;
extern const mcu_pin_obj_t pin_GPIO20;
extern const mcu_pin_obj_t pin_GPIO21;
extern const mcu_pin_obj_t pin_GPIO22;
extern const mcu_pin_obj_t pin_GPIO23;
extern const mcu_pin_obj_t pin_GPIO24;
#if !defined(IGNORE_GPIO25)
extern const mcu_pin_obj_t pin_GPIO25;
#endif
extern const mcu_pin_obj_t pin_GPIO26;
extern const mcu_pin_obj_t pin_GPIO27;
extern const mcu_pin_obj_t pin_GPIO28;
extern const mcu_pin_obj_t pin_GPIO29;
#if NUM_BANK0_GPIOS == 48
extern const mcu_pin_obj_t pin_GPIO30;
extern const mcu_pin_obj_t pin_GPIO31;
extern const mcu_pin_obj_t pin_GPIO32;
extern const mcu_pin_obj_t pin_GPIO33;
extern const mcu_pin_obj_t pin_GPIO34;
extern const mcu_pin_obj_t pin_GPIO35;
extern const mcu_pin_obj_t pin_GPIO36;
extern const mcu_pin_obj_t pin_GPIO37;
extern const mcu_pin_obj_t pin_GPIO38;
extern const mcu_pin_obj_t pin_GPIO39;
extern const mcu_pin_obj_t pin_GPIO40;
extern const mcu_pin_obj_t pin_GPIO41;
extern const mcu_pin_obj_t pin_GPIO42;
extern const mcu_pin_obj_t pin_GPIO43;
extern const mcu_pin_obj_t pin_GPIO44;
extern const mcu_pin_obj_t pin_GPIO45;
extern const mcu_pin_obj_t pin_GPIO46;
extern const mcu_pin_obj_t pin_GPIO47;
#endif

#if CIRCUITPY_CYW43
extern const mcu_pin_obj_t pin_CYW0;
extern const mcu_pin_obj_t pin_CYW1;
extern const mcu_pin_obj_t pin_CYW2;
#endif
