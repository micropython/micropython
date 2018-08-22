/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
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

#include "common-hal/microcontroller/Pin.h"
#include "nrf_gpio.h"
#include "py/mphal.h"

void reset_all_pins(void) {
    for (uint32_t pin = 0; pin < NUMBER_OF_PINS; ++pin) {
        nrf_gpio_cfg_default(pin);
    }

    #ifdef MICROPY_HW_NEOPIXEL
    neopixel_in_use = false;
    #endif
    #ifdef MICROPY_HW_APA102_MOSI
    apa102_sck_in_use = false;
    apa102_mosi_in_use = false;
    #endif

    // After configuring SWD because it may be shared.
    #ifdef SPEAKER_ENABLE_PIN
    speaker_enable_in_use = false;
    // TODO set pin to out and turn off.
    #endif
}

void claim_pin(const mcu_pin_obj_t* pin) {
    #ifdef MICROPY_HW_NEOPIXEL
    if (pin == MICROPY_HW_NEOPIXEL) {
        neopixel_in_use = true;
    }
    #endif
    #ifdef MICROPY_HW_APA102_MOSI
    if (pin == MICROPY_HW_APA102_MOSI) {
        apa102_mosi_in_use = true;
    }
    if (pin == MICROPY_HW_APA102_SCK) {
        apa102_sck_in_use = true;
    }
    #endif

    #ifdef SPEAKER_ENABLE_PIN
    if (pin == SPEAKER_ENABLE_PIN) {
        speaker_enable_in_use = true;
    }
    #endif
}

bool common_hal_mcu_pin_is_free(const mcu_pin_obj_t *pin) {
    #ifdef MICROPY_HW_NEOPIXEL
    if (pin == MICROPY_HW_NEOPIXEL) {
        return !neopixel_in_use;
    }
    #endif
    #ifdef MICROPY_HW_APA102_MOSI
    if (pin == MICROPY_HW_APA102_MOSI) {
        return !apa102_mosi_in_use;
    }
    if (pin == MICROPY_HW_APA102_SCK) {
        return !apa102_sck_in_use;
    }
    #endif

    #ifdef SPEAKER_ENABLE_PIN
    if (pin == SPEAKER_ENABLE_PIN) {
        return !speaker_enable_in_use;
    }
    #endif

    // TODO check pin enable.
    return true;
}
