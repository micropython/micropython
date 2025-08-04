/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022-2024 Infineon Technologies AG
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

#ifndef MICROPY_INCLUDED_MACHINE_PIN_PHY_H
#define MICROPY_INCLUDED_MACHINE_PIN_PHY_H

#include <stdint.h>
#include "genhdr/pins.h"

#define PIN_PHY_GPIO_IN_USE_GENERAL_ERROR 0x4020d01
#define PIN_PHY_I2C_IN_USE_ERROR 0x4020900
#define PIN_PHY_SPI_IN_USE_ERROR 0x4021907
#define PIN_PHY_UART_IN_USE_ERROR 0x4021f00

#define assert_pin_phy_used(ret)  { \
        switch (ret) { \
            case PIN_PHY_GPIO_IN_USE_GENERAL_ERROR: \
            case PIN_PHY_I2C_IN_USE_ERROR: \
            case PIN_PHY_SPI_IN_USE_ERROR: \
            case PIN_PHY_UART_IN_USE_ERROR: \
                mp_raise_msg(&mp_type_Exception, MP_ERROR_TEXT("The chosen pin(s) might be already in use by another machine instance.")); \
                break; \
        } \
}

typedef struct _machine_pin_phy_obj_t {
    uint32_t addr;
    char *name;
} machine_pin_phy_obj_t;

extern machine_pin_phy_obj_t machine_pin_phy_obj[];

extern const uint8_t machine_pin_num_of_cpu_pins;

int pin_fetch_address(mp_obj_t pin); // function to support the bitstream class (obj to pin address)

// Function Prototypes to support interaction between c<->py
int pin_find(mp_obj_t obj);
mp_obj_t pin_name_by_addr(mp_obj_t pin);
int pin_addr_by_name(mp_obj_t obj);

#endif // MICROPY_INCLUDED_MACHINE_PIN_PHY_H
