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

#include "py/mphal.h"

#include "components/driver/include/driver/gpio.h"
#include "components/hal/include/hal/gpio_hal.h"

STATIC uint32_t never_reset_pins[2];
STATIC uint32_t in_use[2];

void never_reset_pin_number(gpio_num_t pin_number) {
    if (pin_number == NO_PIN) {
        return;
    }
    never_reset_pins[pin_number / 32] |= 1 << pin_number % 32;
}

void common_hal_never_reset_pin(const mcu_pin_obj_t *pin) {
    if (pin == NULL) {
        return;
    }
    never_reset_pin_number(pin->number);
}

STATIC void _reset_pin(gpio_num_t pin_number) {
    #if defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)
    // Never ever reset pins used for flash and RAM.
    if (26 <= pin_number && pin_number <= 32) {
        return;
    }
    #ifdef CONFIG_SPIRAM_MODE_OCT
    // Octal DQ4-DQ7 and DQS/DM
    if (33 <= pin_number && pin_number <= 37) {
        return;
    }
    #endif

    #if CIRCUITPY_USB
    // Never reset USB pins.
    if (pin_number == 19 || pin_number == 20) {
        return;
    }
    #endif
    #elif defined(CONFIG_IDF_TARGET_ESP32C3)
    // Never ever reset pins used for flash and RAM.
    if (11 <= pin_number && pin_number <= 17) {
        return;
    }
    #endif

    gpio_reset_pin(pin_number);

    #ifdef DOUBLE_TAP_PIN
    // Pull the double tap pin down so that resets come back to CircuitPython.
    if (pin_number == DOUBLE_TAP_PIN->number) {
        gpio_pullup_dis(pin_number);
        gpio_pulldown_en(pin_number);
    }
    #endif
}

// Mark pin as free and return it to a quiescent state.
void reset_pin_number(gpio_num_t pin_number) {
    if (pin_number == NO_PIN) {
        return;
    }
    never_reset_pins[pin_number / 32] &= ~(1 << pin_number % 32);
    in_use[pin_number / 32] &= ~(1 << pin_number % 32);

    _reset_pin(pin_number);
}

void common_hal_mcu_pin_reset_number(uint8_t i) {
    reset_pin_number((gpio_num_t)i);
}

void common_hal_reset_pin(const mcu_pin_obj_t *pin) {
    if (pin == NULL) {
        return;
    }
    reset_pin_number(pin->number);
}

void reset_all_pins(void) {
    for (uint8_t i = 0; i < GPIO_PIN_COUNT; i++) {
        uint32_t iomux_address = GPIO_PIN_MUX_REG[i];
        if (iomux_address == 0 ||
            (never_reset_pins[i / 32] & (1 << i % 32)) != 0) {
            continue;
        }
        _reset_pin(i);
    }
    in_use[0] = never_reset_pins[0];
    in_use[1] = never_reset_pins[1];
}

void claim_pin_number(gpio_num_t pin_number) {
    if (pin_number == NO_PIN) {
        return;
    }
    in_use[pin_number / 32] |= (1 << (pin_number % 32));
}

void claim_pin(const mcu_pin_obj_t *pin) {
    in_use[pin->number / 32] |= (1 << (pin->number % 32));
}

void common_hal_mcu_pin_claim(const mcu_pin_obj_t *pin) {
    claim_pin(pin);
}

bool pin_number_is_free(gpio_num_t pin_number) {
    uint8_t offset = pin_number / 32;
    uint32_t mask = 1 << (pin_number % 32);
    return (in_use[offset] & mask) == 0;
}

bool common_hal_mcu_pin_is_free(const mcu_pin_obj_t *pin) {
    return pin_number_is_free(pin->number);
}

uint8_t common_hal_mcu_pin_number(const mcu_pin_obj_t *pin) {
    return pin ? pin->number : NO_PIN;
}
