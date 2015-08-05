/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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

#include <stdio.h>
#include STM32_HAL_H

#include "py/nlr.h"
#include "py/runtime.h"
#include "timer.h"
#include "led.h"
#include "pin.h"
#include "genhdr/pins.h"
#include "mphal.h"

#if defined(MICROPY_HW_LED1)

// default is to not PWM LED4
#if !defined(MICROPY_HW_LED4_PWM)
#define MICROPY_HW_LED4_PWM (0)
#endif

/// \moduleref pyb
/// \class LED - LED object
///
/// The LED object controls an individual LED (Light Emitting Diode).

typedef struct _pyb_led_obj_t {
    mp_obj_base_t base;
    mp_uint_t led_id;
    const pin_obj_t *led_pin;
} pyb_led_obj_t;

STATIC const pyb_led_obj_t pyb_led_obj[] = {
    {{&pyb_led_type}, 1, &MICROPY_HW_LED1},
#if defined(MICROPY_HW_LED2)
    {{&pyb_led_type}, 2, &MICROPY_HW_LED2},
#if defined(MICROPY_HW_LED3)
    {{&pyb_led_type}, 3, &MICROPY_HW_LED3},
#if defined(MICROPY_HW_LED4)
    {{&pyb_led_type}, 4, &MICROPY_HW_LED4},
#endif
#endif
#endif
};
#define NUM_LEDS MP_ARRAY_SIZE(pyb_led_obj)

void led_init(void) {
    /* GPIO structure */
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Configure I/O speed, mode, output type and pull */
    GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
    GPIO_InitStructure.Mode = MICROPY_HW_LED_OTYPE;
    GPIO_InitStructure.Pull = GPIO_NOPULL;

    /* Turn off LEDs and initialize */
    for (int led = 0; led < NUM_LEDS; led++) {
        const pin_obj_t *led_pin = pyb_led_obj[led].led_pin;
        mp_hal_gpio_clock_enable(led_pin->gpio);
        MICROPY_HW_LED_OFF(led_pin);
        GPIO_InitStructure.Pin = led_pin->pin_mask;
        HAL_GPIO_Init(led_pin->gpio, &GPIO_InitStructure);
    }

    #if MICROPY_HW_LED4_PWM
    // LED4 (blue) is on PB4 which is TIM3_CH1
    // we use PWM on this channel to fade the LED

    // LED3 (yellow) is on PA15 which has TIM2_CH1, so we could PWM that as well

    // GPIO configuration
    GPIO_InitStructure.Pin = MICROPY_HW_LED4.pin_mask;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init(MICROPY_HW_LED4.gpio, &GPIO_InitStructure);

    // PWM mode configuration
    TIM_OC_InitTypeDef oc_init;
    oc_init.OCMode = TIM_OCMODE_PWM1;
    oc_init.Pulse = 0; // off
    oc_init.OCPolarity = TIM_OCPOLARITY_HIGH;
    oc_init.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&TIM3_Handle, &oc_init, TIM_CHANNEL_1);

    // start PWM
    TIM_CCxChannelCmd(TIM3, TIM_CHANNEL_1, TIM_CCx_ENABLE);
    #endif
}

void led_state(pyb_led_t led, int state) {
    if (led < 1 || led > NUM_LEDS) {
        return;
    }
    if (MICROPY_HW_LED4_PWM && led == 4) {
        if (state) {
            TIM3->CCR1 = 0xffff;
        } else {
            TIM3->CCR1 = 0;
        }
        return;
    }
    const pin_obj_t *led_pin = pyb_led_obj[led - 1].led_pin;
    //printf("led_state(%d,%d)\n", led, state);
    if (state == 0) {
        // turn LED off
        MICROPY_HW_LED_OFF(led_pin);
    } else {
        // turn LED on
        MICROPY_HW_LED_ON(led_pin);
    }
}

void led_toggle(pyb_led_t led) {
    if (led < 1 || led > NUM_LEDS) {
        return;
    }

    if (MICROPY_HW_LED4_PWM && led == 4) {
        if (TIM3->CCR1 == 0) {
            TIM3->CCR1 = 0xffff;
        } else {
            TIM3->CCR1 = 0;
        }
        return;
    }

    // toggle the output data register to toggle the LED state
    const pin_obj_t *led_pin = pyb_led_obj[led - 1].led_pin;
    led_pin->gpio->ODR ^= led_pin->pin_mask;
}

int led_get_intensity(pyb_led_t led) {
    if (led < 1 || led > NUM_LEDS) {
        return 0;
    }

    if (MICROPY_HW_LED4_PWM && led == 4) {
        mp_uint_t i = (TIM3->CCR1 * 255 + (USBD_CDC_POLLING_INTERVAL*1000) - 2) / ((USBD_CDC_POLLING_INTERVAL*1000) - 1);
        if (i > 255) {
            i = 255;
        }
        return i;
    }

    const pin_obj_t *led_pin = pyb_led_obj[led - 1].led_pin;
    GPIO_TypeDef *gpio = led_pin->gpio;

    // TODO convert high/low to on/off depending on board
    if (gpio->ODR & led_pin->pin_mask) {
        // pin is high
        return 255;
    } else {
        // pin is low
        return 0;
    }
}

void led_set_intensity(pyb_led_t led, mp_int_t intensity) {
    if (MICROPY_HW_LED4_PWM && led == 4) {
        // set intensity using PWM pulse width
        if (intensity < 0) {
            intensity = 0;
        } else if (intensity >= 255) {
            intensity = 0xffff;
        } else {
            intensity = intensity * ((USBD_CDC_POLLING_INTERVAL*1000) - 1) / 255;
        }
        TIM3->CCR1 = intensity;
        return;
    }

    // intensity not supported for this LED; just turn it on/off
    led_state(led, intensity > 0);
}

void led_debug(int n, int delay) {
    led_state(1, n & 1);
    led_state(2, n & 2);
    led_state(3, n & 4);
    led_state(4, n & 8);
    HAL_Delay(delay);
}

/******************************************************************************/
/* Micro Python bindings                                                      */

void led_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_led_obj_t *self = self_in;
    mp_printf(print, "LED(%lu)", self->led_id);
}

/// \classmethod \constructor(id)
/// Create an LED object associated with the given LED:
///
///   - `id` is the LED number, 1-4.
STATIC mp_obj_t led_obj_make_new(mp_obj_t type_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    // get led number
    mp_int_t led_id = mp_obj_get_int(args[0]);

    // check led number
    if (!(1 <= led_id && led_id <= NUM_LEDS)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "LED(%d) does not exist", led_id));
    }

    // return static led object
    return (mp_obj_t)&pyb_led_obj[led_id - 1];
}

/// \method on()
/// Turn the LED on.
mp_obj_t led_obj_on(mp_obj_t self_in) {
    pyb_led_obj_t *self = self_in;
    led_state(self->led_id, 1);
    return mp_const_none;
}

/// \method off()
/// Turn the LED off.
mp_obj_t led_obj_off(mp_obj_t self_in) {
    pyb_led_obj_t *self = self_in;
    led_state(self->led_id, 0);
    return mp_const_none;
}

/// \method toggle()
/// Toggle the LED between on and off.
mp_obj_t led_obj_toggle(mp_obj_t self_in) {
    pyb_led_obj_t *self = self_in;
    led_toggle(self->led_id);
    return mp_const_none;
}

/// \method intensity([value])
/// Get or set the LED intensity.  Intensity ranges between 0 (off) and 255 (full on).
/// If no argument is given, return the LED intensity.
/// If an argument is given, set the LED intensity and return `None`.
mp_obj_t led_obj_intensity(mp_uint_t n_args, const mp_obj_t *args) {
    pyb_led_obj_t *self = args[0];
    if (n_args == 1) {
        return mp_obj_new_int(led_get_intensity(self->led_id));
    } else {
        led_set_intensity(self->led_id, mp_obj_get_int(args[1]));
        return mp_const_none;
    }
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(led_obj_on_obj, led_obj_on);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(led_obj_off_obj, led_obj_off);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(led_obj_toggle_obj, led_obj_toggle);
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(led_obj_intensity_obj, 1, 2, led_obj_intensity);

STATIC const mp_map_elem_t led_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_on), (mp_obj_t)&led_obj_on_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_off), (mp_obj_t)&led_obj_off_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_toggle), (mp_obj_t)&led_obj_toggle_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_intensity), (mp_obj_t)&led_obj_intensity_obj },
};

STATIC MP_DEFINE_CONST_DICT(led_locals_dict, led_locals_dict_table);

const mp_obj_type_t pyb_led_type = {
    { &mp_type_type },
    .name = MP_QSTR_LED,
    .print = led_obj_print,
    .make_new = led_obj_make_new,
    .locals_dict = (mp_obj_t)&led_locals_dict,
};

#else
// For boards with no LEDs, we leave an empty function here so that we don't
// have to put conditionals everywhere.
void led_init(void) {
}
void led_state(pyb_led_t led, int state) {
}
void led_toggle(pyb_led_t led) {
}
#endif  // defined(MICROPY_HW_LED1)
