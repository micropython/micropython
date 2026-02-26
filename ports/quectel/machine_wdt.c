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
#include "py/runtime.h"
#include "modmachine.h"
#include "mphalport.h"
#include "py/obj.h"
#include "py/mphal.h"
#include "py/smallint.h"
#include "utime_mphal.h"

#include "helios_wdt.h"
typedef struct _machine_wdt_init_t
{
	mp_obj_base_t base;
}machine_wdt_init_t;
const mp_obj_type_t machine_wdt_type;

typedef struct _machine_wdt_obj_t
{
	int status;  // Y/N 
	int period;
}machine_wdt_obj_t;

static machine_wdt_init_t machine_wdt_obj = {0};

static mp_obj_t machine_wdt_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) 
{
	mp_arg_check_num(n_args, n_kw, 0, 1, true);

	machine_wdt_init_t *self = &machine_wdt_obj;
	machine_wdt_obj_t info = {0};
	self->base.type = &machine_wdt_type;
	if (n_args > 0)
	{
		info.period = mp_obj_get_int(args[0]);
		--n_args;
		++args;
	}
	Helios_WDT_Deinit();
	if(0 != Helios_WDT_Init((uint64_t) info.period)) {
		return mp_const_false;
	}
	return MP_OBJ_FROM_PTR(self);
}


static mp_obj_t machine_wdt_feed(const mp_obj_t arg0)
{
	Helios_WDT_Feed();
	return mp_obj_new_int(0);
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_wdt_feed_obj, machine_wdt_feed);


static mp_obj_t machine_wdt_stop(const mp_obj_t arg0)
{
	Helios_WDT_Deinit();
	return mp_obj_new_int(0);
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_wdt_stop_obj, machine_wdt_stop);


static const mp_rom_map_elem_t machine_wdt_locals_dict_table[] = {
	{ MP_ROM_QSTR(MP_QSTR_feed), MP_ROM_PTR(&machine_wdt_feed_obj) },
	{ MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&machine_wdt_stop_obj) },
};
static MP_DEFINE_CONST_DICT(machine_wdt_locals_dict, machine_wdt_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_wdt_type,
    MP_QSTR_WDT,
    MP_TYPE_FLAG_NONE,
    make_new, machine_wdt_make_new,
    locals_dict, &machine_wdt_locals_dict
    );
