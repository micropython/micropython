/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Texas Instruments, Inc
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

#include <sensor.h>

#include "py/mpconfig.h"
#if MICROPY_PY_TMP006

#include <zephyr.h>
#include <misc/stack.h>

#include "py/runtime.h"

static struct device *tmp006_dev = NULL;

static double get_temp(struct device *temp_dev)
{
	int r;
	struct sensor_value temp_value;
	double temp = 0.0;

	r = sensor_sample_fetch(temp_dev);
	if (r) {
		printf("sensor_sample_fetch failed return: %d\n", r);
	}

	r = sensor_channel_get(temp_dev, SENSOR_CHAN_TEMP,
			       &temp_value);
	if (r) {
		printf("sensor_channel_get failed return: %d\n", r);
	}

	temp = sensor_value_to_double(&temp_value);
	temp = (temp * 9.0 / 5.0) + 32.0;

	return(temp);
}

STATIC mp_obj_t tmp006_read_temp(void) {
	float temp;

	if (!tmp006_dev) {
		/* Initialize driver. */
		tmp006_dev = device_get_binding("TEMP_0");
		if (!tmp006_dev) {
			return -1;
		}
	}
	
	/* Get temp in degrees F */
	temp = get_temp(tmp006_dev);
 
#if MICROPY_PY_BUILTINS_FLOAT
	return mp_obj_new_float(temp);
#else
	return mp_obj_new_int((int)temp);
#endif
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(tmp006_read_temp_obj, tmp006_read_temp);

STATIC const mp_rom_map_elem_t mp_module_time_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_tmp006) },
    { MP_ROM_QSTR(MP_QSTR_read_temp), MP_ROM_PTR(&tmp006_read_temp_obj) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_time_globals, mp_module_time_globals_table);

const mp_obj_module_t mp_module_tmp006 = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_time_globals,
};

#endif // MICROPY_PY_TMP006
