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
#include "stdlib.h"
#include "mpconfigport.h"
#include "py/obj.h"
#include "py/compile.h"
#include "py/runtime.h"
#include "py/repl.h"
#include "py/mperrno.h"

#if MICROPY_QPY_MISC_POWER

#include "modmisc.h"
#include "helios_power.h"

const mp_obj_type_t misc_power_type;

static mp_obj_t misc_power_reset()
{
	int ret = 1;
	Helios_Power_Reset(ret);
	return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_0(misc_power_reset_obj, misc_power_reset);


static mp_obj_t misc_power_down()
{
	int ret = 1;
	Helios_Power_Down(ret);
	return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_0(misc_power_down_obj, misc_power_down);

static mp_obj_t misc_power_get_down_reason()
{
	int ret;
	ret = Helios_Power_GetDownReason();
	return mp_obj_new_int(ret);
}

#if !defined(PLAT_RDA)
static MP_DEFINE_CONST_FUN_OBJ_0(misc_power_get_down_reason_obj, misc_power_get_down_reason);
#endif
static mp_obj_t misc_power_get_up_reason()
{
	int ret = 0;
	ret = Helios_Power_GetUpReason();
	return mp_obj_new_int(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_0(misc_power_get_up_reason_obj, misc_power_get_up_reason);


static mp_obj_t misc_power_get_batt()
{
	unsigned int ret;
	ret = Helios_Power_GetBatteryVol();
	return mp_obj_new_int(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_0(misc_power_get_batt_obj, misc_power_get_batt);

static const mp_rom_map_elem_t misc_power_locals_dict_table[] = {
	{ MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_misc) },
	{ MP_ROM_QSTR(MP_QSTR_powerRestart), MP_ROM_PTR(&misc_power_reset_obj) },
	{ MP_ROM_QSTR(MP_QSTR_powerDown), MP_ROM_PTR(&misc_power_down_obj) },
	{ MP_ROM_QSTR(MP_QSTR_powerOnReason), MP_ROM_PTR(&misc_power_get_up_reason_obj) },
#if !defined(PLAT_RDA)
	{ MP_ROM_QSTR(MP_QSTR_powerDownReason), MP_ROM_PTR(&misc_power_get_down_reason_obj) },
#endif
	{ MP_ROM_QSTR(MP_QSTR_getVbatt), MP_ROM_PTR(&misc_power_get_batt_obj) },
};

static MP_DEFINE_CONST_DICT(misc_power_locals_dict, misc_power_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    misc_power_type,
    MP_QSTR_Power,
    MP_TYPE_FLAG_NONE,
    locals_dict, &misc_power_locals_dict
);
#endif /* MICROPY_QPY_MISC_POWER */
