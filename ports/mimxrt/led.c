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
#include "pin.h"

/// \moduleref machine
/// \class LED - LED object
///
/// The LED object controls an individual LED (Light Emitting Diode).

typedef struct _machine_led_obj_t
{
    mp_obj_base_t base;
    mp_uint_t led_id;
    const pin_obj_t *led_pin;
} machine_led_obj_t;

STATIC const machine_led_obj_t machine_led_obj[] = {
    {
        .base = {&machine_led_type},
        .led_id = 1U,
        .led_pin = &MICROPY_HW_LED_PIN,
    }
};

#define NUM_LEDS MP_ARRAY_SIZE(machine_led_obj)


void led_init(void) {
    // Turn off LEDs and initialize
    for (mp_int_t led = 0; led < NUM_LEDS; led++) {
        const pin_obj_t *led_pin = machine_led_obj[led].led_pin;

        gpio_pin_config_t pin_config = {
            .outputLogic = 1U,
            .direction = kGPIO_DigitalOutput,
            .interruptMode = kGPIO_NoIntmode,
        };

        GPIO_PinInit(led_pin->gpio, led_pin->pin, &pin_config);

        // ALT mode for GPIO is always 5
        IOMUXC_SetPinMux(led_pin->muxRegister, 5U, 0, 0, led_pin->configRegister,
            1U);                 // Software Input On Field: Input Path is determined by functionality
        IOMUXC_SetPinConfig(led_pin->muxRegister, 5U, 0, 0, led_pin->configRegister, 0x10B0U);
        MICROPY_HW_LED_OFF(led_pin);
    }
}

void led_state(machine_led_t led, int state) {
    if (led < 1 || led > NUM_LEDS) {
        return;
    }

    const pin_obj_t *led_pin = machine_led_obj[led - 1].led_pin;

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

    const pin_obj_t *led_pin = machine_led_obj[led - 1].led_pin;
    mp_hal_pin_toggle(led_pin);
}

void led_debug(int value, int delay) {
    for (mp_int_t i = 0; i < NUM_LEDS; i++) {
        led_state(i + 1, (value & (1 << i)));
    }
    mp_hal_delay_ms(delay);
}


/******************************************************************************/
/* MicroPython bindings                                                       */

STATIC void led_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    machine_led_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "LED(%u)", self->led_id);
}

/// \classmethod \constructor(id)
/// Create an LED object associated with the given LED:
///
///   - `id` is the LED number, 1.
STATIC mp_obj_t led_obj_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    // Extract arguments
    mp_int_t led_id = mp_obj_get_int(args[0]);

    // Check led id is in range
    if (!(1 <= led_id && led_id <= NUM_LEDS)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "LED(%d) doesn't exist", led_id));
    }

    // Return reference to static object
    return MP_OBJ_FROM_PTR(&machine_led_obj[led_id - 1]);
}

/// \method on()
/// Turn the LED on.
STATIC mp_obj_t led_obj_on(mp_obj_t self_in) {
    machine_led_obj_t *self = MP_OBJ_TO_PTR(self_in);
    MICROPY_HW_LED_ON(self->led_pin);
    return mp_const_none;
}

/// \method off()
/// Turn the LED off.
STATIC mp_obj_t led_obj_off(mp_obj_t self_in) {
    machine_led_obj_t *self = MP_OBJ_TO_PTR(self_in);
    MICROPY_HW_LED_OFF(self->led_pin);
    return mp_const_none;
}

/// \method toggle()
/// Toggle the LED between on and off.
STATIC mp_obj_t led_obj_toggle(mp_obj_t self_in) {
    machine_led_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_hal_pin_toggle(self->led_pin);
    return mp_const_none;
}

// TODO: Implement intensity method
/// \method intensity([value])
/// Get or set the LED intensity.  Intensity ranges between 0 (off) and 255 (full on).
/// If no argument is given, return the LED intensity.
/// If an argument is given, set the LED intensity and return `None`.
mp_obj_t led_obj_intensity(size_t n_args, const mp_obj_t *args) {
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(led_obj_on_obj, led_obj_on);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(led_obj_off_obj, led_obj_off);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(led_obj_toggle_obj, led_obj_toggle);
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(led_obj_intensity_obj, 1, 2, led_obj_intensity);

STATIC const mp_rom_map_elem_t led_locals_dict_table[] = {
    {MP_ROM_QSTR(MP_QSTR_on), MP_ROM_PTR(&led_obj_on_obj)},
    {MP_ROM_QSTR(MP_QSTR_off), MP_ROM_PTR(&led_obj_off_obj)},
    {MP_ROM_QSTR(MP_QSTR_toggle), MP_ROM_PTR(&led_obj_toggle_obj)},
    {MP_ROM_QSTR(MP_QSTR_intensity), MP_ROM_PTR(&led_obj_intensity_obj)},
};

STATIC MP_DEFINE_CONST_DICT(led_locals_dict, led_locals_dict_table);

const mp_obj_type_t machine_led_type = {
    {&mp_type_type},
    .name = MP_QSTR_LED,
    .print = led_obj_print,
    .make_new = led_obj_make_new,
    .locals_dict = (mp_obj_dict_t *)&led_locals_dict,
};
