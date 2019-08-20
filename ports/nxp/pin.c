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
#include <stdint.h>
#include <string.h>

#include "py/runtime.h"
#include "py/mphal.h"
#include "extmod/virtpin.h"
#include "pin.h"

#include "fsl_gpio.h" 

void pin_init0(void)
{
    MP_STATE_PORT(pin_class_mapper) = mp_const_none;
    MP_STATE_PORT(pin_class_map_dict) = mp_const_none;
}


uint32_t pin_get_direction(const pin_obj_t *pin)
{
    GPIO_Type *gpio = pin->gpio;
    
    return ((gpio->GDIR) & (1<<(pin->pin)));
}

/* convert a user-supplied pin name into an ordinal pin number. */
const pin_obj_t *pin_find(mp_obj_t user_obj)
{
    const pin_obj_t *pin_obj;

    /* If a pin was provided, then use it */
    if(mp_obj_is_type(user_obj, &pin_type))
    {
        pin_obj = MP_OBJ_TO_PTR(user_obj);
        return pin_obj;
    }
    nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "Pin(%s) doesn't exist", mp_obj_str_get_str(user_obj)));
}

/*
 * special method __str__() 
 * Return a string describing the pin object
 */
STATIC void pin_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    pin_obj_t *self = MP_OBJ_TO_PTR(self_in);

    mp_printf(print, "Pin(%q, mode=Pin).",self->name);
}

STATIC mp_obj_t pin_obj_init_helper(const pin_obj_t *pin, size_t n_args, const mp_obj_t *args, mp_map_t *kw_args);

/* 
 * special method __init__()   args[0] is pin name ,args[1] is direction, args[2] is outputlogic
 * args[3] is interrupt mode
 */
mp_obj_t  pin_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    /* Run an argument through the mapper and return the result. */
    const pin_obj_t *pin = pin_find(args[0]);

    if(n_args > 1 || n_kw >= 0)
    {
        /* pin mode given, so configure this gpio */
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args+n_args);
        pin_obj_init_helper(pin, n_args-1, args+1, &kw_args);
    }
    return MP_OBJ_FROM_PTR(pin);
}

/* 
 * sepcial method __call__()
 */
STATIC mp_obj_t pin_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    mp_arg_check_num(n_args, n_kw, 0, 1, false);
    pin_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if(n_args == 0)
    {
        return MP_OBJ_NEW_SMALL_INT(mp_hal_pin_read(self));
    }
    else
    {
        mp_hal_pin_write(self, mp_obj_is_true(args[0]));
        return mp_const_none;
    }
}

/* 
 * classmethod mapper([fun])
 * Get or set the pin mapper function.
 */
STATIC mp_obj_t pin_mapper(size_t n_args, const mp_obj_t *args)
{
    if(n_args>1)
    {
        MP_STATE_PORT(pin_class_mapper) = args[1];
        return mp_const_none;
    }
    return MP_STATE_PORT(pin_class_mapper);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pin_mapper_fun_obj, 1, 2, pin_mapper);
STATIC MP_DEFINE_CONST_CLASSMETHOD_OBJ(pin_mapper_obj, MP_ROM_PTR(&pin_mapper_fun_obj));

/* 
 * classmethod dict([dict])
 * Get or set the pin mapper dictionary.
 */
STATIC mp_obj_t pin_map_dict(size_t n_args, const mp_obj_t *args)
{
    if(n_args > 1)
    {
        MP_STATE_PORT(pin_class_map_dict) = args[1];
        return mp_const_none;
    }
    return MP_STATE_PORT(pin_class_mapper);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pin_map_dict_fun_obj, 1, 2, pin_map_dict);
STATIC MP_DEFINE_CONST_CLASSMETHOD_OBJ(pin_map_dict_obj, MP_ROM_PTR(&pin_map_dict_fun_obj));

/* 
 * pin class init helper C API 
 * init(direction, value=0, interruptMode=None)
 */
STATIC mp_obj_t pin_obj_init_helper(const pin_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_direction, MP_ARG_INT},
        { MP_QSTR_value, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_interrupt_mode, MP_ARG_REQUIRED | MP_ARG_INT },

    };

    /* parse args */
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    
    /* get pin direction */
    gpio_pin_direction_t direction = (gpio_pin_direction_t)args[0].u_int;
    if (!IS_GPIO_DIRECTION(direction))
    {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "invalid pin direction: %d", direction));
    } 
    uint8_t value = (uint8_t)args[1].u_int;
    
    gpio_interrupt_mode_t interrupt_mode = kGPIO_NoIntmode;

    gpio_pin_config_t config;
    /* sepcifies the pin direction */
    config.direction = direction;
    /* Set a default output logic */
    config.outputLogic = value;
    /* specifies the pin interrupt mode */
    config.interruptMode = interrupt_mode;
    GPIO_PinInit(self->gpio, self->pin, &config);

    return mp_const_none;
}

STATIC mp_obj_t pin_obj_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    return pin_obj_init_helper(MP_OBJ_TO_PTR(args[0]), n_args - 1, args + 1, kw_args);
}
MP_DEFINE_CONST_FUN_OBJ_KW(pin_init_obj, 1, pin_obj_init);

/* method pin()
 * Get the pin number of the instance.
 */
STATIC mp_obj_t pin_pin(mp_obj_t self_in)
{
    pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(self->pin);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pin_pin_obj, pin_pin);

/* method gpio()
 * Returns the base address of the GPIO associated with this pin instance 
 */ 
STATIC mp_obj_t pin_gpio(mp_obj_t self_in)
{
    pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT((intptr_t)self->gpio);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pin_gpio_obj, pin_gpio);

/* 
 * method value()
 * Get or set the digital logic level of the pin
 */
STATIC mp_obj_t pin_value(size_t n_args, const mp_obj_t *args)
{
    return pin_call(args[0], n_args-1, 0, args+1);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pin_value_obj, 1, 2, pin_value);

/* 
 * method interruptMode()
 * Return the special pin's interrupt mode 
 */
STATIC mp_obj_t pin_interruptMode(mp_obj_t self_in)
{
    pin_obj_t *pin = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(GPIO_GetPinsInterruptFlags(pin->gpio));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pin_interruptMode_obj, pin_interruptMode);

/* 
 * method direction()
 * Return special pin's direction
 */
STATIC mp_obj_t pin_direction(mp_obj_t self_in)
{
    return MP_OBJ_NEW_SMALL_INT(pin_get_direction(MP_OBJ_TO_PTR(self_in)));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pin_direction_obj, pin_direction);

/* pin class local dict table */
STATIC const mp_rom_map_elem_t pin_locals_dict_table[] = {
    
    /* instance method */
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&pin_init_obj) },

    { MP_ROM_QSTR(MP_QSTR_value), MP_ROM_PTR(&pin_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_interrupt_mode), MP_ROM_PTR(&pin_interruptMode_obj) },
    { MP_ROM_QSTR(MP_QSTR_direction), MP_ROM_PTR(&pin_direction_obj) },

    /* Legacy names */
    { MP_ROM_QSTR(MP_QSTR_pin), MP_ROM_PTR(&pin_pin_obj) },
    { MP_ROM_QSTR(MP_QSTR_gpio), MP_ROM_PTR(&pin_gpio_obj) },

    { MP_ROM_QSTR(MP_QSTR_mapper), MP_ROM_PTR(&pin_mapper_obj) },
    {MP_ROM_QSTR(MP_QSTR_dict), MP_ROM_PTR(&pin_map_dict_obj)},

};

STATIC MP_DEFINE_CONST_DICT(pin_locals_dict, pin_locals_dict_table);

STATIC mp_uint_t pin_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode)
{
    (void)errcode;
    pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    
    switch (request)
    {
        case MP_PIN_READ:{
            return mp_hal_pin_read(self);
        }
        case MP_PIN_WRITE:{
            mp_hal_pin_write(self, arg);
            return 0;
        }
    }
    return -1;
}

/* Pin protocol */
STATIC const mp_pin_p_t pin_pin_p = {
    .ioctl = pin_ioctl,
};


/* define class pin */
const mp_obj_type_t pin_type = {
    {&mp_type_type},
    .name = MP_QSTR_Pin,
    .print = pin_print,
    .make_new = pin_make_new,
    .call = pin_call,
    .protocol = &pin_pin_p,
    .locals_dict = (mp_obj_dict_t*)&pin_locals_dict,
};


