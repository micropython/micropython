// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "pins.h"

#include "shared-bindings/microcontroller/Pin.h"

#if CIRCUITPY_CYW43
#include "bindings/cyw43/__init__.h"

#define CYW_PIN(p_number) \
    const mcu_pin_obj_t pin_CYW##p_number = { \
        { &cyw43_pin_type }, \
        .number = p_number \
    }
#endif

// This macro is used to simplify pin definition in boards/<board>/pins.c
#define PIN(p_number) \
    const mcu_pin_obj_t pin_GPIO##p_number = { \
        { &mcu_pin_type }, \
        .number = p_number \
    }

PIN(0);
PIN(1);
PIN(2);
PIN(3);
PIN(4);
PIN(5);
PIN(6);
PIN(7);
PIN(8);
PIN(9);
PIN(10);
PIN(11);
PIN(12);
PIN(13);
PIN(14);
PIN(15);
PIN(16);
PIN(17);
PIN(18);
PIN(19);
PIN(20);
PIN(21);
PIN(22);
#if !defined(IGNORE_GPIO23)
PIN(23);
#endif
#if !defined(IGNORE_GPIO24)
PIN(24);
#endif
#if !defined(IGNORE_GPIO25)
PIN(25);
#endif
PIN(26);
PIN(27);
PIN(28);
PIN(29);
#if NUM_BANK0_GPIOS == 48
PIN(30);
PIN(31);
PIN(32);
PIN(33);
PIN(34);
PIN(35);
PIN(36);
PIN(37);
PIN(38);
PIN(39);
PIN(40);
PIN(41);
PIN(42);
PIN(43);
PIN(44);
PIN(45);
PIN(46);
PIN(47);
#endif

#if CIRCUITPY_CYW43
CYW_PIN(0);
CYW_PIN(1);
CYW_PIN(2);
#endif
