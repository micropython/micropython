#include <stdio.h>
#include <stm32f4xx_hal.h>
#include "usbd_cdc_msc.h"
#include "usbd_cdc_interface.h"

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "map.h"
#include "runtime.h"
#include "led.h"
#include "pin.h"
#include "build/pins.h"

static const pin_obj_t *gLed[] = {
    &PYB_LED1,
#if defined(PYB_LED2)
    &PYB_LED2,
#if defined(PYB_LED3)
    &PYB_LED3,
#if defined(PYB_LED4)
    &PYB_LED4,
#endif
#endif
#endif
};
#define NUM_LEDS (sizeof(gLed) / sizeof(gLed[0]))

void led_init(void) {
    /* GPIO structure */
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Configure I/O speed, mode, output type and pull */
    GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
    GPIO_InitStructure.Mode = PYB_OTYPE;
    GPIO_InitStructure.Pull = GPIO_NOPULL;

    /* Turn off LEDs and initialize */
    for (int led = 0; led < NUM_LEDS; led++) {
        PYB_LED_OFF(gLed[led]);
        GPIO_InitStructure.Pin = gLed[led]->pin_mask;
        HAL_GPIO_Init(gLed[led]->gpio, &GPIO_InitStructure);
    }

#if defined(PYBOARD4) || defined(PYBv10)
    // LED4 (blue) is on PB4 which is TIM3_CH1
    // we use PWM on this channel to fade the LED

    // GPIO configuration
    GPIO_InitStructure.Pin = PYB_LED4.pin_mask;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init(PYB_LED4.gpio, &GPIO_InitStructure);

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
#if defined(PYBOARD4) || defined(PYBv10)
    if (led == 4) {
        if (state) {
            TIM3->CCR1 = 0xffff;
        } else {
            TIM3->CCR1 = 0;
        }
        return;
    }
#endif
    const pin_obj_t *led_pin = gLed[led - 1];
    //printf("led_state(%d,%d)\n", led, state);
    if (state == 0) {
        // turn LED off
        PYB_LED_OFF(led_pin);
    } else {
        // turn LED on
        PYB_LED_ON(led_pin);
    }
}

void led_toggle(pyb_led_t led) {
    if (led < 1 || led > NUM_LEDS) {
        return;
    }

#if defined(PYBOARD4) || defined(PYBv10)
    if (led == 4) {
        if (TIM3->CCR1 == 0) {
            TIM3->CCR1 = 0xffff;
        } else {
            TIM3->CCR1 = 0;
        }
        return;
    }
#endif

    const pin_obj_t *led_pin = gLed[led - 1];
    GPIO_TypeDef *gpio = led_pin->gpio;

    // We don't know if we're turning the LED on or off, but we don't really
    // care. Just invert the state.
    if (gpio->ODR & led_pin->pin_mask) {
        // pin is high, make it low
        gpio->BSRRH = led_pin->pin_mask;
    } else {
        // pin is low, make it high
        gpio->BSRRL = led_pin->pin_mask;
    }
}

int led_get_intensity(pyb_led_t led) {
    if (led < 1 || led > NUM_LEDS) {
        return 0;
    }

#if defined(PYBOARD4) || defined(PYBv10)
    if (led == 4) {
        machine_uint_t i = TIM3->CCR1 * 255 / ((USBD_CDC_POLLING_INTERVAL*1000) - 1);
        if (i > 255) {
            i = 255;
        }
        return i;
    }
#endif

    const pin_obj_t *led_pin = gLed[led - 1];
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

void led_set_intensity(pyb_led_t led, machine_int_t intensity) {
#if defined(PYBOARD4) || defined(PYBv10)
    if (led == 4) {
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
#endif

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

typedef struct _pyb_led_obj_t {
    mp_obj_base_t base;
    machine_uint_t led_id;
} pyb_led_obj_t;

STATIC const pyb_led_obj_t pyb_led_obj[NUM_LEDS] = {
    {{&pyb_led_type}, 1},
#if defined(PYB_LED2)
    {{&pyb_led_type}, 2},
#if defined(PYB_LED3)
    {{&pyb_led_type}, 3},
#if defined(PYB_LED4)
    {{&pyb_led_type}, 4},
#endif
#endif
#endif
};

void led_obj_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_led_obj_t *self = self_in;
    print(env, "<LED %lu>", self->led_id);
}

STATIC mp_obj_t led_obj_make_new(mp_obj_t type_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    // check arguments
    rt_check_nargs(n_args, 1, 1, n_kw, false);

    // get led number
    machine_int_t led_id = mp_obj_get_int(args[0]) - 1;

    // check led number
    if (!(0 <= led_id && led_id < NUM_LEDS)) {
        nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "Led %d does not exist", led_id));
    }

    // return static led object
    return (mp_obj_t)&pyb_led_obj[led_id];
}

mp_obj_t led_obj_on(mp_obj_t self_in) {
    pyb_led_obj_t *self = self_in;
    led_state(self->led_id, 1);
    return mp_const_none;
}

mp_obj_t led_obj_off(mp_obj_t self_in) {
    pyb_led_obj_t *self = self_in;
    led_state(self->led_id, 0);
    return mp_const_none;
}

mp_obj_t led_obj_toggle(mp_obj_t self_in) {
    pyb_led_obj_t *self = self_in;
    led_toggle(self->led_id);
    return mp_const_none;
}

mp_obj_t led_obj_intensity(uint n_args, const mp_obj_t *args) {
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
    .name = MP_QSTR_Led,
    .print = led_obj_print,
    .make_new = led_obj_make_new,
    .locals_dict = (mp_obj_t)&led_locals_dict,
};
