/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
 * Copyright (c) 2019 Artur Pacholec
 * Copyright (c) 2020 Lucian Copeland for Adafruit Industries
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
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"

STATIC bool claimed_pins[IOMUXC_SW_PAD_CTL_PAD_COUNT];
STATIC bool never_reset_pins[IOMUXC_SW_PAD_CTL_PAD_COUNT];

// There are two numbering systems used here:
// IOMUXC index, used for iterating through pins and accessing reset information,
// and GPIO port and number, used to store claimed and reset tagging. The two number
// systems are not related and one cannot determine the other without a pin object
void reset_all_pins(void) {
    for (uint8_t i = 0; i < IOMUXC_SW_PAD_CTL_PAD_COUNT; i++) {
        claimed_pins[i] = never_reset_pins[i];
    }
    for (uint8_t i = 0; i < IOMUXC_SW_PAD_CTL_PAD_COUNT; i++) {
        if (!never_reset_pins[i]) {
            IOMUXC->SW_MUX_CTL_PAD[i] = ((mcu_pin_obj_t *)(mcu_pin_globals.map.table[i].value))->mux_reset;
            IOMUXC->SW_PAD_CTL_PAD[i] = ((mcu_pin_obj_t *)(mcu_pin_globals.map.table[i].value))->pad_reset;
        }
    }
}

// Since i.MX pins need extra register and reset information to reset properly,
// resetting pins by number alone has been removed.
void common_hal_reset_pin(const mcu_pin_obj_t *pin) {
    if (pin == NULL) {
        return;
    }
    never_reset_pins[pin->mux_idx] = false;
    claimed_pins[pin->mux_idx] = false;
    *(uint32_t *)pin->mux_reg = pin->mux_reset;
    *(uint32_t *)pin->cfg_reg = pin->pad_reset;
}

void common_hal_never_reset_pin(const mcu_pin_obj_t *pin) {
    never_reset_pins[pin->mux_idx] = true;
}

bool common_hal_mcu_pin_is_free(const mcu_pin_obj_t *pin) {
    return !claimed_pins[pin->mux_idx];
}

uint8_t common_hal_mcu_pin_number(const mcu_pin_obj_t *pin) {
    return pin->mux_idx; // returns IOMUXC to align with pin table
}

void common_hal_mcu_pin_claim(const mcu_pin_obj_t *pin) {
    claimed_pins[pin->mux_idx] = true;
}

void claim_pin(const mcu_pin_obj_t *pin) {
    common_hal_mcu_pin_claim(pin);
}

void common_hal_mcu_pin_reset_number(uint8_t pin_no) {
    common_hal_reset_pin((mcu_pin_obj_t *)(mcu_pin_globals.map.table[pin_no].value));
}
