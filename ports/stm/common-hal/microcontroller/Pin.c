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
#include "pins.h"

#ifdef MICROPY_HW_NEOPIXEL
bool neopixel_in_use;
#endif
#if defined(MICROPY_HW_APA102_MOSI) && defined(MICROPY_HW_APA102_SCK)
bool apa102_sck_in_use;
bool apa102_mosi_in_use;
#endif

#if defined(TFBGA216)
GPIO_TypeDef *ports[] = {GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG, GPIOH, GPIOI, GPIOJ, GPIOK};
#elif defined(LQFP144)
GPIO_TypeDef *ports[] = {GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG};
#elif defined(LQFP100_f4) || (LQFP100_x7)
GPIO_TypeDef *ports[] = {GPIOA, GPIOB, GPIOC, GPIOD, GPIOE};
#elif defined(LQFP64)
GPIO_TypeDef *ports[] = {GPIOA, GPIOB, GPIOC, GPIOD};
#elif defined(UFQFPN48)
GPIO_TypeDef *ports[] = {GPIOA, GPIOB, GPIOC};
#endif


#define GPIO_PORT_COUNT (MP_ARRAY_SIZE(ports))

STATIC uint16_t claimed_pins[GPIO_PORT_COUNT];
STATIC uint16_t __ALIGNED(4) never_reset_pins[GPIO_PORT_COUNT];

void reset_all_pins(void) {
    // Reset claimed pins
    for (uint8_t i = 0; i < GPIO_PORT_COUNT; i++) {
        claimed_pins[i] = never_reset_pins[i];
    }
    for (uint8_t i = 0; i < GPIO_PORT_COUNT; i++) {
        HAL_GPIO_DeInit(ports[i], ~never_reset_pins[i]);
    }

    #ifdef MICROPY_HW_NEOPIXEL
    neopixel_in_use = false;
    #endif
    #if defined(MICROPY_HW_APA102_MOSI) && defined(MICROPY_HW_APA102_SCK)
    apa102_sck_in_use = false;
    apa102_mosi_in_use = false;
    #endif
}

// Mark pin as free and return it to a quiescent state.
void reset_pin_number(uint8_t pin_port, uint8_t pin_number) {
    if (pin_number == NO_PIN) {
        return;
    }

    if (pin_port == 0x0F) {
        return;
    }
    // Clear claimed bit & reset
    claimed_pins[pin_port] &= ~(1 << pin_number);
    never_reset_pins[pin_port] &= ~(1 << pin_number);
    HAL_GPIO_DeInit(ports[pin_port], 1 << pin_number);

    #ifdef MICROPY_HW_NEOPIXEL
    if (pin_port == MICROPY_HW_NEOPIXEL->port && pin_number == MICROPY_HW_NEOPIXEL->number) {
        neopixel_in_use = false;
        rgb_led_status_init();
        return;
    }
    #endif
    #if defined(MICROPY_HW_APA102_MOSI) && defined(MICROPY_HW_APA102_SCK)
    if (
        (pin_port == MICROPY_HW_APA102_MOSI->port && pin_number == MICROPY_HW_APA102_MOSI->number)
        || (pin_port == MICROPY_HW_APA102_SCK->port && pin_number == MICROPY_HW_APA102_MOSI->number)
        ) {
        apa102_mosi_in_use = false;
        apa102_sck_in_use = false;
        rgb_led_status_init();
        return;
    }
    #endif
}

void never_reset_pin_number(uint8_t pin_port, uint8_t pin_number) {
    if (pin_number == NO_PIN) {
        return;
    }
    never_reset_pins[pin_port] |= 1 << pin_number;
    // Make sure never reset pins are also always claimed
    claimed_pins[pin_port] |= 1 << pin_number;
}

void common_hal_never_reset_pin(const mcu_pin_obj_t *pin) {
    never_reset_pin_number(pin->port, pin->number);
}

void common_hal_reset_pin(const mcu_pin_obj_t *pin) {
    if (pin == NULL) {
        return;
    }
    reset_pin_number(pin->port, pin->number);
}

void claim_pin(uint8_t pin_port, uint8_t pin_number) {
    // Set bit in claimed_pins bitmask.
    claimed_pins[pin_port] |= 1 << pin_number;
}

bool pin_number_is_free(uint8_t pin_port, uint8_t pin_number) {
    return !(claimed_pins[pin_port] & 1 << pin_number);
}

bool common_hal_mcu_pin_is_free(const mcu_pin_obj_t *pin) {
    #ifdef MICROPY_HW_NEOPIXEL
    if (pin == MICROPY_HW_NEOPIXEL) {
        return !neopixel_in_use;
    }
    #endif
    #if defined(MICROPY_HW_APA102_MOSI) && defined(MICROPY_HW_APA102_SCK)
    if (pin == MICROPY_HW_APA102_MOSI) {
        return !apa102_mosi_in_use;
    }
    if (pin == MICROPY_HW_APA102_SCK) {
        return !apa102_sck_in_use;
    }
    #endif

    return pin_number_is_free(pin->port, pin->number);
}

GPIO_TypeDef *pin_port(uint8_t pin_port) {
    return ports[pin_port];
}

uint16_t pin_mask(uint8_t pin_number) {
    return 1 << pin_number;
}

uint8_t common_hal_mcu_pin_number(const mcu_pin_obj_t *pin) {
    return pin->port * 16 + pin->number;
}

void common_hal_mcu_pin_claim(const mcu_pin_obj_t *pin) {
    claim_pin(pin->port, pin->number);
    #ifdef MICROPY_HW_NEOPIXEL
    if (pin == MICROPY_HW_NEOPIXEL) {
        neopixel_in_use = true;
    }
    #endif
    #if defined(MICROPY_HW_APA102_MOSI) && defined(MICROPY_HW_APA102_SCK)
    if (pin == MICROPY_HW_APA102_MOSI) {
        apa102_mosi_in_use = true;
    }
    if (pin == MICROPY_HW_APA102_SCK) {
        apa102_sck_in_use = true;
    }
    #endif
}

void common_hal_mcu_pin_reset_number(uint8_t pin_no) {
    reset_pin_number(pin_no / 16, pin_no % 16);
}
