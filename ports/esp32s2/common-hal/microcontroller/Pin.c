/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
 * Copyright (c) 2019 Lucian Copeland for Adafruit Industries
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
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "supervisor/shared/rgb_led_status.h"

#include "py/mphal.h"

#include "esp-idf/components/driver/include/driver/gpio.h"
#include "esp-idf/components/soc/include/hal/gpio_hal.h"

#ifdef MICROPY_HW_NEOPIXEL
bool neopixel_in_use;
#endif

STATIC uint32_t never_reset_pins[2];
STATIC uint32_t in_use[2];

bool apa102_mosi_in_use;
bool apa102_sck_in_use;

void never_reset_pin_number(gpio_num_t pin_number) {
    never_reset_pins[pin_number / 32] |= 1 << pin_number % 32;
}

void common_hal_never_reset_pin(const mcu_pin_obj_t* pin) {
    never_reset_pin_number(pin->number);
}

// Mark pin as free and return it to a quiescent state.
void reset_pin_number(gpio_num_t pin_number) {
    never_reset_pins[pin_number / 32] &= ~(1 << pin_number % 32);
    in_use[pin_number / 32] &= ~(1 << pin_number % 32);

    #ifdef MICROPY_HW_NEOPIXEL
    if (pin_number == MICROPY_HW_NEOPIXEL->number) {
        neopixel_in_use = false;
        rgb_led_status_init();
        return;
    }
    #endif
}

void common_hal_reset_pin(const mcu_pin_obj_t* pin) {
    reset_pin_number(pin->number);
}

void reset_all_pins(void) {
    for (uint8_t i = 0; i < GPIO_PIN_COUNT; i++) {
        uint32_t iomux_address = GPIO_PIN_MUX_REG[i];
        if (iomux_address == 0 ||
            (never_reset_pins[i / 32] & (1 << i % 32)) != 0) {
            continue;
        }
        gpio_set_direction(i, GPIO_MODE_DEF_INPUT);
        gpio_pullup_dis(i);
        gpio_pulldown_dis(i);
    }
    in_use[0] = 0;
    in_use[1] = 0;

    #ifdef MICROPY_HW_NEOPIXEL
    neopixel_in_use = false;
    #endif
}

void claim_pin(const mcu_pin_obj_t* pin) {
    in_use[pin->number / 32] |= (1 << pin->number % 32);
    #ifdef MICROPY_HW_NEOPIXEL
    if (pin == MICROPY_HW_NEOPIXEL) {
        neopixel_in_use = true;
    }
    #endif
}

void common_hal_mcu_pin_claim(const mcu_pin_obj_t* pin) {
    claim_pin(pin);
}

bool pin_number_is_free(gpio_num_t pin_number) {
    #ifdef MICROPY_HW_NEOPIXEL
    if (pin_number == MICROPY_HW_NEOPIXEL->number) {
        return !neopixel_in_use;
    }
    #endif

    uint8_t offset = pin_number / 32;
    uint8_t mask = 1 << pin_number % 32;
    return (never_reset_pins[offset] & mask) == 0 && (in_use[offset] & mask) == 0;
}

bool common_hal_mcu_pin_is_free(const mcu_pin_obj_t *pin) {
    return pin_number_is_free(pin->number);
}
