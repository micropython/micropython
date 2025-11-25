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

#include "stdio.h"
#include "stdlib.h"
#include <string.h>
#include "py/obj.h"
#include "py/runtime.h"
#include "mphalport.h"

#if MICROPY_QPY_MISC_ADC

#include "helios_adc.h"

typedef enum
{
	ADC0 = 0,
	ADC1 = 1,
	ADC2 = 2,
	ADC3 = 3,
}ADCn;

typedef struct _misc_adc_obj_t {
    mp_obj_base_t base;
    unsigned int pin;
} misc_adc_obj_t;

const mp_obj_type_t misc_adc_type;

static mp_obj_t Helios_adc_init(mp_obj_t self_in)
{
	int ret = Helios_ADC_Init();
	return mp_obj_new_int(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_1(Helios_adc_init_obj, Helios_adc_init);

static mp_obj_t Helios_adc_read(mp_obj_t self_in, mp_obj_t adc_channel)
{
	int ret = -1;
	int channel = mp_obj_get_int(adc_channel);
	if ((channel == 0) || (channel == 1) || (channel == 2) || (channel == 3))
	{
		unsigned int chl = channel;
		//ret = ql_adc_read(chl, &batvol);
		
		ret = Helios_ADC_Read((Helios_ADCNum) chl);
		return mp_obj_new_int(ret);
	}
	return mp_obj_new_int(-1);
}
static MP_DEFINE_CONST_FUN_OBJ_2(Helios_adc_read_obj, Helios_adc_read);

static mp_obj_t Helios_adc_deinit(mp_obj_t self_in)
{
	int ret = Helios_ADC_Deinit();
	return mp_obj_new_int(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_1(Helios_adc_deinit_obj, Helios_adc_deinit);



static const mp_rom_map_elem_t misc_adc_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&Helios_adc_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&Helios_adc_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&Helios_adc_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_ADC0), MP_ROM_INT(ADC0) },
#if !defined(PLAT_Qualcomm)
    { MP_ROM_QSTR(MP_QSTR_ADC1), MP_ROM_INT(ADC1) },
#if !defined(PLAT_Unisoc_8850) && !defined(BOARD_EC600ECN_LC) && !defined(PLAT_SONY_ALT1350)
    { MP_ROM_QSTR(MP_QSTR_ADC2), MP_ROM_INT(ADC2) },
    { MP_ROM_QSTR(MP_QSTR_ADC3), MP_ROM_INT(ADC3) },
#endif
#endif
};
static MP_DEFINE_CONST_DICT(misc_adc_locals_dict, misc_adc_locals_dict_table);

static mp_obj_t misc_adc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) 
{
    mp_arg_check_num(n_args, n_kw, 0, MP_OBJ_FUN_ARGS_MAX, true);

    // create ADC object
    misc_adc_obj_t *self = m_new_obj(misc_adc_obj_t);
    self->base.type = &misc_adc_type;
    return MP_OBJ_FROM_PTR(self);
}

MP_DEFINE_CONST_OBJ_TYPE(
    misc_adc_type,
    MP_QSTR_ADC,
    MP_TYPE_FLAG_NONE,
    make_new, misc_adc_make_new,
    locals_dict, &misc_adc_locals_dict
    );
#endif /* MICROPY_QPY_MISC_ADC */


