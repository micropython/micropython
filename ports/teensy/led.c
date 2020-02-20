#include <stdio.h>

#include "Arduino.h"

#include "py/runtime.h"
#include "py/mphal.h"
#include "led.h"
#include "pin.h"
#include "genhdr/pins.h"

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
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStructure.Mode = MICROPY_HW_LED_OTYPE;
    GPIO_InitStructure.Pull = GPIO_NOPULL;

    /* Turn off LEDs and initialize */
    for (int led = 0; led < NUM_LEDS; led++) {
        const pin_obj_t *led_pin = pyb_led_obj[led].led_pin;
        MICROPY_HW_LED_OFF(led_pin);
        GPIO_InitStructure.Pin = led_pin->pin_mask;
        HAL_GPIO_Init(led_pin->gpio, &GPIO_InitStructure);
    }
}

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
}

void led_toggle(pyb_led_t led) {
    if (led < 1 || led > NUM_LEDS) {
        return;
    }
    const pin_obj_t *led_pin = pyb_led_obj[led - 1].led_pin;
    GPIO_TypeDef *gpio = led_pin->gpio;

    // We don't know if we're turning the LED on or off, but we don't really
    // care. Just invert the state.
    if (gpio->PDOR & led_pin->pin_mask) {
        // pin is high, make it low
        gpio->PCOR = led_pin->pin_mask;
    } else {
        // pin is low, make it high
        gpio->PSOR = led_pin->pin_mask;
    }
}

/******************************************************************************/
/* MicroPython bindings                                                       */

void led_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_led_obj_t *self = self_in;
    (void)kind;
    mp_printf(print, "<LED %lu>", self->led_id);
}

STATIC mp_obj_t led_obj_make_new(const mp_obj_type_t *type, uint n_args, uint n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    // get led number
    mp_int_t led_id = mp_obj_get_int(args[0]);

    // check led number
    if (!(1 <= led_id && led_id <= NUM_LEDS)) {
        mp_raise_msg_varg(&mp_type_ValueError, "LED %d does not exist", led_id);
    }

    // return static led object
    return (mp_obj_t)&pyb_led_obj[led_id - 1];
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

STATIC MP_DEFINE_CONST_FUN_OBJ_1(led_obj_on_obj, led_obj_on);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(led_obj_off_obj, led_obj_off);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(led_obj_toggle_obj, led_obj_toggle);

STATIC const mp_rom_map_elem_t led_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_on), MP_ROM_PTR(&led_obj_on_obj) },
    { MP_ROM_QSTR(MP_QSTR_off), MP_ROM_PTR(&led_obj_off_obj) },
    { MP_ROM_QSTR(MP_QSTR_toggle), MP_ROM_PTR(&led_obj_toggle_obj) },
};

STATIC MP_DEFINE_CONST_DICT(led_locals_dict, led_locals_dict_table);

const mp_obj_type_t pyb_led_type = {
    { &mp_type_type },
    .name = MP_QSTR_LED,
    .print = led_obj_print,
    .make_new = led_obj_make_new,
    .locals_dict = (mp_obj_t)&led_locals_dict,
};
