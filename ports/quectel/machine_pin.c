/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
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
#include "py/gc.h"
#include "py/mphal.h"

#if MICROPY_QPY_MACHINE_PIN

#include "helios_gpio.h"
#include "helios_debug.h"


#define HELIOS_PIN_LOG(msg, ...)      custom_log("machine_pin", msg, ##__VA_ARGS__)

#define GPIO_OBJ_DEF(n) {{&machine_pin_type}, HELIOS_GPIO##n, 0, 0, 0}
#define PLAT_GPIO_OBJ_DEF(n) BOOST_PP_REPEAT_1(n,GPIO_OBJ_DEF)


typedef struct _machine_pin_obj_t {
    mp_obj_base_t base;
    uint  pin;
	uint  dir;
	uint  pull;
	uint  value;
} machine_pin_obj_t;

const mp_obj_type_t machine_pin_type;


static machine_pin_obj_t *machine_pin_obj[PLAT_GPIO_NUM] = {NULL};


static void machine_pin_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_pin_obj_t *self = self_in;
    mp_printf(print, "<Pin(%u)>", self->pin);
}

// pin.init(mode, pull=None, *, value)
static mp_obj_t machine_pin_obj_init_helper(machine_pin_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_dir, ARG_pull, ARG_value };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_dir, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_pull, MP_ARG_INT, {.u_int = -1}},
        { MP_QSTR_value, MP_ARG_INT, {.u_int = -1}},
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

	// get dir mode
	switch (args[ARG_dir].u_int) {
		case -1:
			break;
		case HELIOS_GPIO_INPUT:
		case HELIOS_GPIO_OUTPUT:
			self->dir = args[ARG_dir].u_int;
			break;
		default:
			mp_raise_ValueError(MP_ERROR_TEXT("invalid pin dir, range{0:dir in, 1:dir out}"));
			break;
	}
    // get pull mode
	switch (args[ARG_pull].u_int) {
		case -1:
			break;
		case HELIOS_PULL_NONE:
		case HELIOS_PULL_UP:
		case HELIOS_PULL_DOWN:
			self->pull = args[ARG_pull].u_int;
			break;
		default:
			mp_raise_ValueError(MP_ERROR_TEXT("invalid pin pull, range{0:PIN_PULL_DISABLE, 1:PIN_PULL_PU, 2:PIN_PULL_PD}"));
			break;
	}
	// get initial value
	switch (args[ARG_value].u_int) {
		case -1:
			break;
		case HELIOS_LVL_LOW:
		case HELIOS_LVL_HIGH:
			self->value = args[ARG_value].u_int;
			break;
		default:
			mp_raise_ValueError(MP_ERROR_TEXT("invalid pin value, range{0:PIN_LEVEL_LOW, 1:PIN_LEVEL_HIGH}"));
			break;
	}
	Helios_GPIOInitStruct gpio_struct = {0};
	gpio_struct.dir = self->dir;
	gpio_struct.pull = self->pull;
	gpio_struct.value = self->value;
	HELIOS_PIN_LOG("pin = %d dir = %d, pull = %d value=%d\n",self->pin, self->dir, self->pull, self->value);
	int ret = Helios_GPIO_Init((Helios_GPIONum) self->pin, &gpio_struct);
	HELIOS_PIN_LOG("gpio init ret = %d\n",ret);

	if (ret != 0){
		mp_raise_ValueError(MP_ERROR_TEXT("GPIO initialization error"));
	}


	return mp_const_none;
}

// constructor(drv_name, pin, ...)
mp_obj_t mp_pin_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);
	
	// get the wanted pin object
	//unsigned int i = 0;
	int wanted_pin = 0;
	wanted_pin = mp_obj_get_int(args[0]);

	if(wanted_pin == 0 || wanted_pin > PLAT_GPIO_NUM) {
		mp_raise_ValueError(MP_ERROR_TEXT("invalid pin"));
	}

	HELIOS_PIN_LOG("wanted_pin = %d\n",wanted_pin);
	machine_pin_obj_t *self = NULL;

	if (machine_pin_obj[wanted_pin-1] == NULL)
	{
		machine_pin_obj[wanted_pin-1] = mp_obj_malloc_with_finaliser(machine_pin_obj_t, &machine_pin_type);
	}
	self = machine_pin_obj[wanted_pin-1];
	self->base.type = &machine_pin_type;
	self->pin = wanted_pin;
	
	// default settings
	self->dir = HELIOS_GPIO_OUTPUT;
	self->pull = HELIOS_PULL_NONE;
	self->value = HELIOS_LVL_LOW;
	HELIOS_PIN_LOG("dir = %d, pull = %d value=%d\n",self->dir, self->pull, self->value);

    if (n_args > 1 || n_kw > 0) {
        // pin mode given, so configure this GPIO
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
		HELIOS_PIN_LOG("n_args = %d\n",n_args);
        machine_pin_obj_init_helper(self, n_args - 1, args + 1, &kw_args);
		HELIOS_PIN_LOG("n_kw = %d\n",n_kw);
    } else {
		
		Helios_GPIOInitStruct gpio_struct = {0};
		gpio_struct.dir = self->dir;
		gpio_struct.pull = self->pull;
		gpio_struct.value = self->value;
		Helios_GPIO_Init((Helios_GPIONum) self->pin, &gpio_struct);
	}
	
	HELIOS_PIN_LOG("pin init end\n");
    return MP_OBJ_FROM_PTR(self);
}

// fast method for getting/setting pin value
static mp_obj_t machine_pin_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);
    machine_pin_obj_t *self = self_in;
    if (n_args == 0) {
		int pin_val = Helios_GPIO_GetLevel((Helios_GPIONum) self->pin);
        return MP_OBJ_NEW_SMALL_INT(pin_val);
    } else {
		Helios_GPIO_SetLevel((Helios_GPIONum) self->pin, (Helios_LvlMode) mp_obj_is_true(args[0]));
        return mp_const_none;
    }
}

// pin.init(mode, pull)
static mp_obj_t machine_pin_obj_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    return machine_pin_obj_init_helper(args[0], n_args - 1, args + 1, kw_args);
}
MP_DEFINE_CONST_FUN_OBJ_KW(machine_pin_init_obj, 1, machine_pin_obj_init);

// pin.value([value])
static mp_obj_t machine_pin_value(size_t n_args, const mp_obj_t *args) {
    return machine_pin_call(args[0], n_args - 1, 0, args + 1);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pin_value_obj, 1, 2, machine_pin_value);

static mp_obj_t machine_pin_off(mp_obj_t self_in) {
    machine_pin_obj_t *self = self_in;
	Helios_GPIO_SetLevel((Helios_GPIONum) self->pin, (Helios_LvlMode) HELIOS_LVL_LOW);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_off_obj, machine_pin_off);

static mp_obj_t machine_pin_on(mp_obj_t self_in) {
    machine_pin_obj_t *self = self_in;
	Helios_GPIO_SetLevel((Helios_GPIONum) self->pin, (Helios_LvlMode) HELIOS_LVL_HIGH);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_on_obj, machine_pin_on);

static mp_obj_t machine_pin_write(mp_obj_t self_in, mp_obj_t value) 
{
    machine_pin_obj_t *self = self_in;
    int pin_value = mp_obj_get_int(value);
	int ret = Helios_GPIO_SetLevel((Helios_GPIONum) self->pin, (Helios_LvlMode) pin_value);
    return mp_obj_new_int(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_2(machine_pin_write_obj, machine_pin_write);

static mp_obj_t machine_pin_read(size_t n_args, const mp_obj_t *args) 
{
    return machine_pin_call(args[0], n_args - 1, 0, args + 1);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pin_read_obj, 1, 2, machine_pin_read);


static mp_uint_t machine_pin_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    (void)errcode;
    machine_pin_obj_t *self = self_in;

    switch (request) {
        case MP_PIN_READ: {
			int pin_val = Helios_GPIO_GetLevel((Helios_GPIONum) self->pin);
            return pin_val;
        }
        case MP_PIN_WRITE: {
			Helios_GPIO_SetLevel((Helios_GPIONum) self->pin, (Helios_LvlMode) arg);
            return 0;
        }
    }
    return -1;
}

static mp_obj_t machine_pin_set_dir(mp_obj_t self_in, mp_obj_t dir) 
{
    machine_pin_obj_t *self = self_in;
	int ret = -1;
	
    int pin_value = mp_obj_get_int(dir);
	if((pin_value != HELIOS_GPIO_INPUT) && (pin_value != HELIOS_GPIO_OUTPUT)){
		mp_raise_ValueError(MP_ERROR_TEXT("Invalid direction parameter"));
	}
	ret = Helios_GPIO_SetDirection((Helios_GPIONum) self->pin, (Helios_GPIODir) pin_value);
    return mp_obj_new_int(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_2(machine_pin_set_dir_obj, machine_pin_set_dir);

static mp_obj_t machine_pin_get_dir(mp_obj_t self_in) 
{
    machine_pin_obj_t *self = self_in;
	int ret = Helios_GPIO_GetDirection((Helios_GPIONum) self->pin);
    return mp_obj_new_int(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_get_dir_obj, machine_pin_get_dir);


static mp_obj_t machine_pin_deinit(mp_obj_t self_in) {
    machine_pin_obj_t *self = MP_OBJ_TO_PTR(self_in);
	//HELIOS_PIN_LOG("machine pin deinit.pin=%d.\r\n", self->pin);
	
	machine_pin_obj[self->pin - 1] = NULL;
	
	int ret = Helios_GPIO_Deinit((Helios_GPIONum)self->pin);
	if (ret != 0)
	{
		HELIOS_PIN_LOG("GPIO%d deinit failed.\r\n", self->pin);
	}
	return mp_obj_new_int(ret);
	
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_deinit_obj, machine_pin_deinit);


static const mp_rom_map_elem_t machine_pin_locals_dict_table[] = {
    // instance methods
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&machine_pin_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_init),    MP_ROM_PTR(&machine_pin_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_value),   MP_ROM_PTR(&machine_pin_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_off),     MP_ROM_PTR(&machine_pin_off_obj) },
    { MP_ROM_QSTR(MP_QSTR_on),      MP_ROM_PTR(&machine_pin_on_obj) },
    { MP_ROM_QSTR(MP_QSTR_write),   MP_ROM_PTR(&machine_pin_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_read),    MP_ROM_PTR(&machine_pin_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_dir),    MP_ROM_PTR(&machine_pin_set_dir_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_dir),    MP_ROM_PTR(&machine_pin_get_dir_obj) },
    // class constants 
    //GPIO DEFINE
    { MP_ROM_QSTR(MP_QSTR_IN),        MP_ROM_INT(HELIOS_GPIO_INPUT) },
    { MP_ROM_QSTR(MP_QSTR_OUT),       MP_ROM_INT(HELIOS_GPIO_OUTPUT) },
    { MP_ROM_QSTR(MP_QSTR_PULL_PU),   MP_ROM_INT(HELIOS_PULL_UP) },
    { MP_ROM_QSTR(MP_QSTR_PULL_PD),   MP_ROM_INT(HELIOS_PULL_DOWN) },
	{ MP_ROM_QSTR(MP_QSTR_PULL_DISABLE), MP_ROM_INT(HELIOS_PULL_NONE) },
	PLAT_GPIO_DEF(PLAT_GPIO_NUM),
};

static MP_DEFINE_CONST_DICT(machine_pin_locals_dict, machine_pin_locals_dict_table);

static const mp_pin_p_t machine_pin_pin_p = {
    .ioctl = machine_pin_ioctl,
};

MP_DEFINE_CONST_OBJ_TYPE(
	machine_pin_type,
	MP_QSTR_Pin,
	MP_TYPE_FLAG_NONE,
    make_new, mp_pin_make_new,
	print, machine_pin_print,
    call, machine_pin_call,
    protocol, &machine_pin_pin_p,
    locals_dict, &machine_pin_locals_dict
	);

#endif

