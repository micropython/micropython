/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2019 Damien P. George
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

#if defined (MICROPY_PY_LED) && MICROPY_PY_LED
#include "led.h"
#include "pin.h"

#include "fsl_gpio.h"

typedef struct _pyb_led_obj_t
{
    mp_obj_base_t base;
    mp_uint_t led_id;
    const pin_obj_t *led_pin;
}pyb_led_obj_t;

const pin_obj_t pin_led_obj = PIN(1, 9, GPIO1);

STATIC const pyb_led_obj_t pyb_led_obj[] = {
    {{&pyb_led_type}, 1, &pin_led_obj},
};

#define NUM_LEDS MP_ARRAY_SIZE(pyb_led_obj)

void led_init(void)
{
    for(int led=0; led<NUM_LEDS; led++)
    {
        const pin_obj_t *led_pin = pyb_led_obj[led].led_pin;
        gpio_pin_config_t led_config = {kGPIO_DigitalOutput, 0, kGPIO_NoIntmode};
        GPIO_PinInit(led_pin->gpio, led_pin->pin, &led_config);
    }
}

void led_state(pyb_led_t led, int state)
{
    if (led < 1 || led >NUM_LEDS )
        return ;
    
    const pin_obj_t* led_pin = pyb_led_obj[led-1].led_pin;

    if (state == 0)
    {
        mp_hal_pin_low(led_pin);
    }
    else
    {
        mp_hal_pin_high(led_pin);   
    }    
}

/*
 * class method print
 */
void led_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_led_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "LED(%u)", self->led_id);
}

/* 
 * class method make new 
 */
STATIC mp_obj_t led_obj_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    mp_arg_check_num(n_args, n_kw, 1, 1, false);
    
    mp_int_t led_id = mp_obj_get_int(args[0]);
    // check led number
    if (!(1 <= led_id && led_id <= NUM_LEDS)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "LED(%d) doesn't exist", led_id));
    }

    // return static led object
    return MP_OBJ_FROM_PTR(&pyb_led_obj[led_id - 1]);
}

/* 
 * instance method on()
 * open related led
 */
mp_obj_t led_obj_on(mp_obj_t self_in)
{
    pyb_led_obj_t *self = MP_OBJ_TO_PTR(self_in);
    led_state(self->led_id,0);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(led_obj_on_obj, led_obj_on);

/* 
 * instance method off()
 * open related led
 */
mp_obj_t led_obj_off(mp_obj_t self_in)
{
    pyb_led_obj_t *self = MP_OBJ_TO_PTR(self_in);
    led_state(self->led_id,1);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(led_obj_off_obj, led_obj_off);

/* 
 * instance method toggle()
 * toggle related led
 */
mp_obj_t led_obj_toggle(mp_obj_t self_in)
{
    pyb_led_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_hal_pin_toggle(self->led_pin);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(led_obj_toggle_obj, led_obj_toggle);


STATIC const mp_rom_map_elem_t led_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_on), MP_ROM_PTR(&led_obj_on_obj) },
    { MP_ROM_QSTR(MP_QSTR_off), MP_ROM_PTR(&led_obj_off_obj) },
    { MP_ROM_QSTR(MP_QSTR_toggle), MP_ROM_PTR(&led_obj_toggle_obj) },

};

STATIC MP_DEFINE_CONST_DICT(led_locals_dict, led_locals_dict_table);


const mp_obj_type_t pyb_led_type = {
    {&mp_type_type},
    .name = MP_QSTR_LED,
    .print = led_obj_print,
    .make_new = led_obj_make_new,
    .locals_dict = (mp_obj_dict_t*)&led_locals_dict,
};
#endif /* MICROPY_PY_LED */