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

#include "shared-bindings/microcontroller/Pin.h"

#include "atmel_start_pins.h"
#include "hal/include/hal_gpio.h"

#include "supervisor/shared/rgb_led_status.h"
#ifdef SAMD21
#include "samd21_pins.h"
#endif
#ifdef SAMD51
#include "samd51_pins.h"
#endif

#ifdef MICROPY_HW_NEOPIXEL
bool neopixel_in_use;
#endif
#ifdef MICROPY_HW_APA102_MOSI
bool apa102_sck_in_use;
bool apa102_mosi_in_use;
#endif
#ifdef SPEAKER_ENABLE_PIN
bool speaker_enable_in_use;
#endif

void reset_all_pins(void) {
    uint32_t pin_mask[PORT_BITS / 32 + 1] = PORT_OUT_IMPLEMENTED;

    // Do not full reset USB or SWD lines.
    pin_mask[0] &= ~(PORT_PA24 | PORT_PA25 | PORT_PA30 | PORT_PA31);

    #ifdef SAMD21
    pin_mask[0] &= ~(PORT_PA31);
    #endif

    gpio_set_port_direction(GPIO_PORTA, pin_mask[0] & ~MICROPY_PORT_A, GPIO_DIRECTION_OFF);
    gpio_set_port_direction(GPIO_PORTB, pin_mask[1] & ~MICROPY_PORT_B, GPIO_DIRECTION_OFF);
    #if PORT_BITS > 64
    gpio_set_port_direction(GPIO_PORTC, pin_mask[2] & ~MICROPY_PORT_C, GPIO_DIRECTION_OFF);
    #endif
    #if PORT_BITS > 96
    gpio_set_port_direction(GPIO_PORTD, pin_mask[3] & ~MICROPY_PORT_D, GPIO_DIRECTION_OFF);
    #endif

    // Configure SWD
    #ifdef SAMD51
    gpio_set_pin_function(PIN_PA30, MUX_PA30H_CM4_SWCLK);
    // SWDIO will be automatically switched on PA31 when a signal is input on
    // SWCLK.
    #endif
    #ifdef SAMD21
    //gpio_set_pin_function(PIN_PA30, GPIO_PIN_FUNCTION_G);
    //gpio_set_pin_direction(PIN_PA31, GPIO_DIRECTION_OUT);
    //gpio_set_pin_function(PIN_PA31, GPIO_PIN_FUNCTION_G);
    #endif

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
    gpio_set_pin_function(SPEAKER_ENABLE_PIN->pin, GPIO_PIN_FUNCTION_OFF);
    gpio_set_pin_direction(SPEAKER_ENABLE_PIN->pin, GPIO_DIRECTION_OUT);
    gpio_set_pin_level(SPEAKER_ENABLE_PIN->pin, false);
    #endif
}

void reset_pin(uint8_t pin) {
    if (pin >= PORT_BITS) {
        return;
    }

    #ifdef MICROPY_HW_NEOPIXEL
    if (pin == MICROPY_HW_NEOPIXEL->pin) {
        neopixel_in_use = false;
        rgb_led_status_init();
        return;
    }
    #endif
    #ifdef MICROPY_HW_APA102_MOSI
    if (pin == MICROPY_HW_APA102_MOSI->pin ||
        pin == MICROPY_HW_APA102_SCK->pin) {
        apa102_mosi_in_use = apa102_mosi_in_use && pin != MICROPY_HW_APA102_MOSI->pin;
        apa102_sck_in_use = apa102_sck_in_use && pin != MICROPY_HW_APA102_SCK->pin;
        if (!apa102_sck_in_use && !apa102_mosi_in_use) {
            rgb_led_status_init();
        }
        return;
    }
    #endif

    if (pin == PIN_PA30
        #ifdef SAMD51
        ) {
        gpio_set_pin_function(pin, GPIO_PIN_FUNCTION_H);
        #endif
        #ifdef SAMD21
        || pin == PIN_PA31) {
        gpio_set_pin_function(pin, GPIO_PIN_FUNCTION_G);
        #endif
    } else {
        gpio_set_pin_direction(pin, GPIO_DIRECTION_OFF);
        gpio_set_pin_function(pin, GPIO_PIN_FUNCTION_OFF);
    }

    #ifdef SPEAKER_ENABLE_PIN
    if (pin == SPEAKER_ENABLE_PIN->pin) {
        speaker_enable_in_use = false;
        gpio_set_pin_function(pin, GPIO_PIN_FUNCTION_OFF);
        gpio_set_pin_direction(SPEAKER_ENABLE_PIN->pin, GPIO_DIRECTION_OUT);
        gpio_set_pin_level(SPEAKER_ENABLE_PIN->pin, false);
    }
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

bool common_hal_mcu_pin_is_free(const mcu_pin_obj_t* pin) {
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

    PortGroup *const port = &PORT->Group[(enum gpio_port)GPIO_PORT(pin->pin)];
    uint8_t pin_index = GPIO_PIN(pin->pin);
    volatile PORT_PINCFG_Type *state = &port->PINCFG[pin_index];
    volatile PORT_PMUX_Type *pmux = &port->PMUX[pin_index / 2];

    if (pin->pin == PIN_PA30 || pin->pin == PIN_PA31) {
        return state->bit.PMUXEN == 1 && ((pmux->reg >> (4 * pin_index % 2)) & 0xf) == 0x6;
    }

    return state->bit.PMUXEN == 0 && state->bit.INEN == 0 &&
           state->bit.PULLEN == 0 && (port->DIR.reg & (1 << pin_index)) == 0;
}
