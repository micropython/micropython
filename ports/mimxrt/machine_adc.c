/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Philipp Ebensberger
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

#include "py/runtime.h"
#include "py/mphal.h"
#include "adc.h"


STATIC machine_adc_obj_t test_adc_instance;
static ADC_Type *const adcBases[] = ADC_BASE_PTRS;


STATIC void adc_obj_print(const mp_print_t *print, mp_obj_t o, mp_print_kind_t kind) {
    (void)kind;
    machine_adc_obj_t *self = MP_OBJ_TO_PTR(o);

    // Get ADC instance id
    for (int i = 0; i < sizeof(adcBases)/sizeof(ADC_Type*); ++i)
    {
        if (adcBases[i] == self->adc)
        {
            mp_printf(print, "ADC(%u, channel=%u)", i, self->channel);
        }
    }    
}

STATIC mp_obj_t adc_obj_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    // Extract arguments
    ADC_Type* adc_instance = adcBases[0];  // FIXME: get correct instance from pin
    
    // Configure ADC perpheral
    adc_config_t config;
    ADC_GetDefaultConfig(&config);  // FIXME: generate configuration based on user input
    ADC_Init(adc_instance, &config);

    // Configure ADC peripheral channel
    adc_channel_config_t channel_config = {
        .channelNumber=0,
        .enableInterruptOnConversionCompleted=false,
    };
    ADC_SetChannelConfig(adc_instance, 0UL, &channel_config);

    // Create ADC Instance
    test_adc_instance.adc = adc_instance;
    test_adc_instance.channel = 0;

    return MP_OBJ_FROM_PTR(&test_adc_instance);
}


STATIC const mp_rom_map_elem_t adc_locals_dict_table[] = {
};

STATIC MP_DEFINE_CONST_DICT(adc_locals_dict, adc_locals_dict_table);

const mp_obj_type_t machine_adc_type = {
    {&mp_type_type},
    .name = MP_QSTR_ADC,
    .print = adc_obj_print,
    .make_new = adc_obj_make_new,
    .locals_dict = (mp_obj_dict_t *)&adc_locals_dict,
};