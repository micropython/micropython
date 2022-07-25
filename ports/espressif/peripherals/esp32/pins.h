/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
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

// DO NOT include this file directly.
// Use shared-bindings/microcontroller/Pin.h instead.
// This ensures that all necessary includes are already included.

#ifndef MICROPY_INCLUDED_ESPRESSIF_PERIPHERALS_ESP32_PINS_H
#define MICROPY_INCLUDED_ESPRESSIF_PERIPHERALS_ESP32_PINS_H

#define GPIO0_EXISTS 1
extern const mcu_pin_obj_t pin_GPIO0;
#define GPIO1_EXISTS 1
extern const mcu_pin_obj_t pin_GPIO1;
#define GPIO2_EXISTS 1
extern const mcu_pin_obj_t pin_GPIO2;
#define GPIO3_EXISTS 1
extern const mcu_pin_obj_t pin_GPIO3;
#define GPIO4_EXISTS 1
extern const mcu_pin_obj_t pin_GPIO4;
#define GPIO5_EXISTS 1
extern const mcu_pin_obj_t pin_GPIO5;
#define GPIO6_EXISTS 1
extern const mcu_pin_obj_t pin_GPIO6;
#define GPIO7_EXISTS 1
extern const mcu_pin_obj_t pin_GPIO7;
#define GPIO8_EXISTS 1
extern const mcu_pin_obj_t pin_GPIO8;
#define GPIO9_EXISTS 1
extern const mcu_pin_obj_t pin_GPIO9;
#define GPIO10_EXISTS 1
extern const mcu_pin_obj_t pin_GPIO10;
#define GPIO11_EXISTS 1
extern const mcu_pin_obj_t pin_GPIO11;
#define GPIO12_EXISTS 1
extern const mcu_pin_obj_t pin_GPIO12;
#define GPIO13_EXISTS 1
extern const mcu_pin_obj_t pin_GPIO13;
#define GPIO14_EXISTS 1
extern const mcu_pin_obj_t pin_GPIO14;
#define GPIO15_EXISTS 1
extern const mcu_pin_obj_t pin_GPIO15;
#define GPIO16_EXISTS 1
extern const mcu_pin_obj_t pin_GPIO16;
#define GPIO17_EXISTS 1
extern const mcu_pin_obj_t pin_GPIO17;
#define GPIO18_EXISTS 1
extern const mcu_pin_obj_t pin_GPIO18;
#define GPIO19_EXISTS 1
extern const mcu_pin_obj_t pin_GPIO19;
// GPIO20 not exposed on some packages, but available in some modules
#define GPIO20_EXISTS 1
extern const mcu_pin_obj_t pin_GPIO20;
#define GPIO21_EXISTS 1
extern const mcu_pin_obj_t pin_GPIO21;
#define GPIO22_EXISTS 1
extern const mcu_pin_obj_t pin_GPIO22;
#define GPIO23_EXISTS 1
extern const mcu_pin_obj_t pin_GPIO23;
// no GPIO24
#define GPIO25_EXISTS 1
extern const mcu_pin_obj_t pin_GPIO25;
#define GPIO26_EXISTS 1
extern const mcu_pin_obj_t pin_GPIO26;
#define GPIO27_EXISTS 1
extern const mcu_pin_obj_t pin_GPIO27;
// no GPIO28
// no GPIO29
// no GPIO30
// no GPIO31
#define GPIO32_EXISTS 1
extern const mcu_pin_obj_t pin_GPIO32;
#define GPIO33_EXISTS 1
extern const mcu_pin_obj_t pin_GPIO33;
#define GPIO34_EXISTS 1
extern const mcu_pin_obj_t pin_GPIO34;
#define GPIO35_EXISTS 1
extern const mcu_pin_obj_t pin_GPIO35;
#define GPIO36_EXISTS 1
extern const mcu_pin_obj_t pin_GPIO36;
#define GPIO37_EXISTS 1
extern const mcu_pin_obj_t pin_GPIO37;
#define GPIO38_EXISTS 1
extern const mcu_pin_obj_t pin_GPIO38;
#define GPIO39_EXISTS 1
extern const mcu_pin_obj_t pin_GPIO39;

#endif // MICROPY_INCLUDED_ESPRESSIF_PERIPHERALS_ESP32_PINS_H
