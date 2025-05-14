/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Philipp Ebensberger
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

#include "fsl_gpio.h"
#include "fsl_iomuxc.h"

#include "py/runtime.h"
#include "py/mphal.h"
#include "led.h"

#if defined(MICROPY_HW_LED1_PIN)

const machine_led_obj_t machine_led_obj[] = {
    { .base = {&machine_led_type}, .led_id = 1U, .led_pin = MICROPY_HW_LED1_PIN, },
    #if defined(MICROPY_HW_LED2_PIN)
    { .base = {&machine_led_type}, .led_id = 2U, .led_pin = MICROPY_HW_LED2_PIN, },
    #if defined(MICROPY_HW_LED3_PIN)
    { .base = {&machine_led_type}, .led_id = 3U, .led_pin = MICROPY_HW_LED3_PIN, },
    #if defined(MICROPY_HW_LED4_PIN)
    { .base = {&machine_led_type}, .led_id = 4U, .led_pin = MICROPY_HW_LED4_PIN, },
    #endif
    #endif
    #endif
};

#define NUM_LEDS MP_ARRAY_SIZE(machine_led_obj)

void led_init(void) {
    // Turn off LEDs and initialize
    for (mp_int_t led = 0; led < NUM_LEDS; led++) {
        const machine_pin_obj_t *led_pin = machine_led_obj[led].led_pin;
        mp_hal_pin_output(led_pin);
        MICROPY_HW_LED_OFF(led_pin);
    }
}

void led_state(machine_led_t led, int state) {
    if (led < 1 || led > NUM_LEDS) {
        return;
    }

    const machine_pin_obj_t *led_pin = machine_led_obj[led - 1].led_pin;

    if (state == 0) {
        // turn LED off
        MICROPY_HW_LED_OFF(led_pin);
    } else {
        // turn LED on
        MICROPY_HW_LED_ON(led_pin);
    }
}

void led_toggle(machine_led_t led) {
    if (led < 1 || led > NUM_LEDS) {
        return;
    }

    const machine_pin_obj_t *led_pin = machine_led_obj[led - 1].led_pin;
    mp_hal_pin_toggle(led_pin);
}

void led_debug(int value, int delay) {
    for (mp_int_t i = 0; i < NUM_LEDS; i++) {
        led_state(i + 1, (value & (1 << i)));
    }
    mp_hal_delay_ms(delay);
}

#endif
