/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2016 Damien P. George
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

#include "py/runtime.h"
#include "py/mphal.h"
#include "timer.h"
#include "led.h"
#include "pin.h"

#if defined(MICROPY_HW_LED1)

/// \moduleref pyb
/// \class LED - LED object
///
/// The LED object controls an individual LED (Light Emitting Diode).

// the default is that LEDs are not inverted, and pin driven high turns them on
#ifndef MICROPY_HW_LED_INVERTED
#define MICROPY_HW_LED_INVERTED (0)
#endif

typedef struct _pyb_led_obj_t {
    mp_obj_base_t base;
    mp_uint_t led_id;
    const pin_obj_t *led_pin;
} pyb_led_obj_t;

STATIC const pyb_led_obj_t pyb_led_obj[] = {
    {{&pyb_led_type}, 1, MICROPY_HW_LED1},
#if defined(MICROPY_HW_LED2)
    {{&pyb_led_type}, 2, MICROPY_HW_LED2},
#if defined(MICROPY_HW_LED3)
    {{&pyb_led_type}, 3, MICROPY_HW_LED3},
#if defined(MICROPY_HW_LED4)
    {{&pyb_led_type}, 4, MICROPY_HW_LED4},
#endif
#endif
#endif
};
#define NUM_LEDS MP_ARRAY_SIZE(pyb_led_obj)

void led_init(void) {
    /* Turn off LEDs and initialize */
    for (int led = 0; led < NUM_LEDS; led++) {
        const pin_obj_t *led_pin = pyb_led_obj[led].led_pin;
        mp_hal_gpio_clock_enable(led_pin->gpio);
        MICROPY_HW_LED_OFF(led_pin);
        mp_hal_pin_output(led_pin);
    }
}

#if defined(MICROPY_HW_LED1_PWM) \
    || defined(MICROPY_HW_LED2_PWM) \
    || defined(MICROPY_HW_LED3_PWM) \
    || defined(MICROPY_HW_LED4_PWM)

// The following is semi-generic code to control LEDs using PWM.
// It currently supports TIM1, TIM2 and TIM3, channels 1-4.
// Configure by defining the relevant MICROPY_HW_LEDx_PWM macros in mpconfigboard.h.
// If they are not defined then PWM will not be available for that LED.

#define LED_PWM_ENABLED (1)

#ifndef MICROPY_HW_LED1_PWM
#define MICROPY_HW_LED1_PWM { NULL, 0, 0, 0 }
#endif
#ifndef MICROPY_HW_LED2_PWM
#define MICROPY_HW_LED2_PWM { NULL, 0, 0, 0 }
#endif
#ifndef MICROPY_HW_LED3_PWM
#define MICROPY_HW_LED3_PWM { NULL, 0, 0, 0 }
#endif
#ifndef MICROPY_HW_LED4_PWM
#define MICROPY_HW_LED4_PWM { NULL, 0, 0, 0 }
#endif

#define LED_PWM_TIM_PERIOD (10000) // TIM runs at 1MHz and fires every 10ms

// this gives the address of the CCR register for channels 1-4
#define LED_PWM_CCR(pwm_cfg) ((volatile uint32_t*)&(pwm_cfg)->tim->CCR1 + ((pwm_cfg)->tim_channel >> 2))

typedef struct _led_pwm_config_t {
    TIM_TypeDef *tim;
    uint8_t tim_id;
    uint8_t tim_channel;
    uint8_t alt_func;
} led_pwm_config_t;

STATIC const led_pwm_config_t led_pwm_config[] = {
    MICROPY_HW_LED1_PWM,
    MICROPY_HW_LED2_PWM,
    MICROPY_HW_LED3_PWM,
    MICROPY_HW_LED4_PWM,
};

STATIC uint8_t led_pwm_state = 0;

static inline bool led_pwm_is_enabled(int led) {
    return (led_pwm_state & (1 << led)) != 0;
}

// this function has a large stack so it should not be inlined
STATIC void led_pwm_init(int led) __attribute__((noinline));
STATIC void led_pwm_init(int led) {
    const pin_obj_t *led_pin = pyb_led_obj[led - 1].led_pin;
    const led_pwm_config_t *pwm_cfg = &led_pwm_config[led - 1];

    // GPIO configuration
    mp_hal_pin_config(led_pin, MP_HAL_PIN_MODE_ALT, MP_HAL_PIN_PULL_NONE, pwm_cfg->alt_func);

    // TIM configuration
    switch (pwm_cfg->tim_id) {
        case 1: __TIM1_CLK_ENABLE(); break;
        case 2: __TIM2_CLK_ENABLE(); break;
        case 3: __TIM3_CLK_ENABLE(); break;
        default: assert(0);
    }
    TIM_HandleTypeDef tim = {0};
    tim.Instance = pwm_cfg->tim;
    tim.Init.Period = LED_PWM_TIM_PERIOD - 1;
    tim.Init.Prescaler = timer_get_source_freq(pwm_cfg->tim_id) / 1000000 - 1; // TIM runs at 1MHz
    tim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    tim.Init.CounterMode = TIM_COUNTERMODE_UP;
    tim.Init.RepetitionCounter = 0;
    HAL_TIM_PWM_Init(&tim);

    // PWM configuration
    TIM_OC_InitTypeDef oc_init;
    oc_init.OCMode = TIM_OCMODE_PWM1;
    oc_init.Pulse = 0; // off
    oc_init.OCPolarity = MICROPY_HW_LED_INVERTED ? TIM_OCPOLARITY_LOW : TIM_OCPOLARITY_HIGH;
    oc_init.OCFastMode = TIM_OCFAST_DISABLE;
    oc_init.OCNPolarity = TIM_OCNPOLARITY_HIGH; // needed for TIM1 and TIM8
    oc_init.OCIdleState = TIM_OCIDLESTATE_SET; // needed for TIM1 and TIM8
    oc_init.OCNIdleState = TIM_OCNIDLESTATE_SET; // needed for TIM1 and TIM8
    HAL_TIM_PWM_ConfigChannel(&tim, &oc_init, pwm_cfg->tim_channel);
    HAL_TIM_PWM_Start(&tim, pwm_cfg->tim_channel);

    // indicate that this LED is using PWM
    led_pwm_state |= 1 << led;
}

STATIC void led_pwm_deinit(int led) {
    // make the LED's pin a standard GPIO output pin
    const pin_obj_t *led_pin = pyb_led_obj[led - 1].led_pin;
    GPIO_TypeDef *g = led_pin->gpio;
    uint32_t pin = led_pin->pin;
    static const int mode = 1; // output
    static const int alt = 0; // no alt func
    g->MODER = (g->MODER & ~(3 << (2 * pin))) | (mode << (2 * pin));
    g->AFR[pin >> 3] = (g->AFR[pin >> 3] & ~(15 << (4 * (pin & 7)))) | (alt << (4 * (pin & 7)));
    led_pwm_state &= ~(1 << led);
}

#else
#define LED_PWM_ENABLED (0)
#endif

void led_state(pyb_led_t led, int state) {
    if (led < 1 || led > NUM_LEDS) {
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

    #if LED_PWM_ENABLED
    if (led_pwm_is_enabled(led)) {
        led_pwm_deinit(led);
    }
    #endif
}

void led_toggle(pyb_led_t led) {
    if (led < 1 || led > NUM_LEDS) {
        return;
    }

    #if LED_PWM_ENABLED
    if (led_pwm_is_enabled(led)) {
        // if PWM is enabled then LED has non-zero intensity, so turn it off
        led_state(led, 0);
        return;
    }
    #endif

    // toggle the output data register to toggle the LED state
    const pin_obj_t *led_pin = pyb_led_obj[led - 1].led_pin;
    led_pin->gpio->ODR ^= led_pin->pin_mask;
}

int led_get_intensity(pyb_led_t led) {
    if (led < 1 || led > NUM_LEDS) {
        return 0;
    }

    #if LED_PWM_ENABLED
    if (led_pwm_is_enabled(led)) {
        const led_pwm_config_t *pwm_cfg = &led_pwm_config[led - 1];
        mp_uint_t i = (*LED_PWM_CCR(pwm_cfg) * 255 + LED_PWM_TIM_PERIOD - 2) / (LED_PWM_TIM_PERIOD - 1);
        if (i > 255) {
            i = 255;
        }
        return i;
    }
    #endif

    const pin_obj_t *led_pin = pyb_led_obj[led - 1].led_pin;
    GPIO_TypeDef *gpio = led_pin->gpio;

    if (gpio->ODR & led_pin->pin_mask) {
        // pin is high
        return MICROPY_HW_LED_INVERTED ? 0 : 255;
    } else {
        // pin is low
        return MICROPY_HW_LED_INVERTED ? 255 : 0;
    }
}

void led_set_intensity(pyb_led_t led, mp_int_t intensity) {
    #if LED_PWM_ENABLED
    if (intensity > 0 && intensity < 255) {
        const led_pwm_config_t *pwm_cfg = &led_pwm_config[led - 1];
        if (pwm_cfg->tim != NULL) {
            // set intensity using PWM pulse width
            if (!led_pwm_is_enabled(led)) {
                led_pwm_init(led);
            }
            *LED_PWM_CCR(pwm_cfg) = intensity * (LED_PWM_TIM_PERIOD - 1) / 255;
            return;
        }
    }
    #endif

    // intensity not supported for this LED; just turn it on/off
    led_state(led, intensity > 0);
}

void led_debug(int n, int delay) {
    led_state(1, n & 1);
    led_state(2, n & 2);
    led_state(3, n & 4);
    led_state(4, n & 8);
    mp_hal_delay_ms(delay);
}

/******************************************************************************/
/* MicroPython bindings                                                       */

void led_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_led_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "LED(%u)", self->led_id);
}

/// \classmethod \constructor(id)
/// Create an LED object associated with the given LED:
///
///   - `id` is the LED number, 1-4.
STATIC mp_obj_t led_obj_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    // get led number
    mp_int_t led_id = mp_obj_get_int(args[0]);

    // check led number
    if (!(1 <= led_id && led_id <= NUM_LEDS)) {
        mp_raise_msg_varg(&mp_type_ValueError, "LED(%d) doesn't exist", led_id);
    }

    // return static led object
    return MP_OBJ_FROM_PTR(&pyb_led_obj[led_id - 1]);
}

/// \method on()
/// Turn the LED on.
mp_obj_t led_obj_on(mp_obj_t self_in) {
    pyb_led_obj_t *self = MP_OBJ_TO_PTR(self_in);
    led_state(self->led_id, 1);
    return mp_const_none;
}

/// \method off()
/// Turn the LED off.
mp_obj_t led_obj_off(mp_obj_t self_in) {
    pyb_led_obj_t *self = MP_OBJ_TO_PTR(self_in);
    led_state(self->led_id, 0);
    return mp_const_none;
}

/// \method toggle()
/// Toggle the LED between on and off.
mp_obj_t led_obj_toggle(mp_obj_t self_in) {
    pyb_led_obj_t *self = MP_OBJ_TO_PTR(self_in);
    led_toggle(self->led_id);
    return mp_const_none;
}

/// \method intensity([value])
/// Get or set the LED intensity.  Intensity ranges between 0 (off) and 255 (full on).
/// If no argument is given, return the LED intensity.
/// If an argument is given, set the LED intensity and return `None`.
mp_obj_t led_obj_intensity(size_t n_args, const mp_obj_t *args) {
    pyb_led_obj_t *self = MP_OBJ_TO_PTR(args[0]);
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

STATIC const mp_rom_map_elem_t led_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_on), MP_ROM_PTR(&led_obj_on_obj) },
    { MP_ROM_QSTR(MP_QSTR_off), MP_ROM_PTR(&led_obj_off_obj) },
    { MP_ROM_QSTR(MP_QSTR_toggle), MP_ROM_PTR(&led_obj_toggle_obj) },
    { MP_ROM_QSTR(MP_QSTR_intensity), MP_ROM_PTR(&led_obj_intensity_obj) },
};

STATIC MP_DEFINE_CONST_DICT(led_locals_dict, led_locals_dict_table);

const mp_obj_type_t pyb_led_type = {
    { &mp_type_type },
    .name = MP_QSTR_LED,
    .print = led_obj_print,
    .make_new = led_obj_make_new,
    .locals_dict = (mp_obj_dict_t*)&led_locals_dict,
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
