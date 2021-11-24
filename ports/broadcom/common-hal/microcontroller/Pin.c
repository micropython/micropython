/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
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

#include "shared-bindings/microcontroller/Pin.h"

#include "peripherals/broadcom/gpio.h"

void reset_all_pins(void) {
}

void never_reset_pin_number(uint8_t pin_number) {
}

void reset_pin_number(uint8_t pin_number) {
    gpio_set_function(pin_number, 0);
}

void common_hal_never_reset_pin(const mcu_pin_obj_t *pin) {
    never_reset_pin_number(pin->number);
}

void common_hal_reset_pin(const mcu_pin_obj_t *pin) {
    reset_pin_number(pin->number);
}

void claim_pin(const mcu_pin_obj_t *pin) {
    // Nothing to do because all changes will set the GPIO settings.
}

bool pin_number_is_free(uint8_t pin_number) {
    return true;
}

bool common_hal_mcu_pin_is_free(const mcu_pin_obj_t *pin) {
    return pin_number_is_free(pin->number);
}

uint8_t common_hal_mcu_pin_number(const mcu_pin_obj_t *pin) {
    return pin->number;
}

void common_hal_mcu_pin_claim(const mcu_pin_obj_t *pin) {
    return claim_pin(pin);
}

void common_hal_mcu_pin_reset_number(uint8_t pin_no) {
    reset_pin_number(pin_no);
}

#define PIN(num) \
    const mcu_pin_obj_t pin_GPIO##num = { \
        { &mcu_pin_type }, \
        .number = num \
    };

PIN(0)
PIN(1)
PIN(2)
PIN(3)
PIN(4)
PIN(5)
PIN(6)
PIN(7)
PIN(8)
PIN(9)
PIN(10)
PIN(11)
PIN(12)
PIN(13)
PIN(14)
PIN(15)
PIN(16)
PIN(17)
PIN(18)
PIN(19)
PIN(20)
PIN(21)
PIN(22)
PIN(23)
PIN(24)
PIN(25)
PIN(26)
PIN(27)
PIN(28)
PIN(29)

PIN(30)
PIN(31)
PIN(32)
PIN(33)
PIN(34)
PIN(35)
PIN(36)
PIN(37)
PIN(38)
PIN(39)

PIN(40)
PIN(41)
PIN(42)
PIN(43)
PIN(44)
PIN(45)
PIN(46)
PIN(47)
PIN(48)
PIN(49)

PIN(50)
PIN(51)
PIN(52)
PIN(53)
PIN(54)
PIN(55)
PIN(56)
PIN(57)
