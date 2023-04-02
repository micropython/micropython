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

#include <stdint.h>
#include "py/obj.h"
#include "py/runtime.h"
#include "py/mphal.h"

#if defined(MIMXRT117x_SERIES)
#include "fsl_lpadc.h"
#else
#include "fsl_adc.h"
#endif

#include "fsl_gpio.h"
#include "fsl_iomuxc.h"

#include "modmachine.h"

typedef struct _machine_adc_obj_t {
    mp_obj_base_t base;
    ADC_Type *adc;
    uint8_t channel;
    uint8_t channel_group;
    uint16_t resolution;
} machine_adc_obj_t;

STATIC ADC_Type *const adc_bases[] = ADC_BASE_PTRS;

STATIC void adc_obj_print(const mp_print_t *print, mp_obj_t o, mp_print_kind_t kind) {
    (void)kind;
    machine_adc_obj_t *self = MP_OBJ_TO_PTR(o);

    // Get ADC adc id
    for (int i = 1; i < sizeof(adc_bases) / sizeof(ADC_Type *); ++i) {
        if (adc_bases[i] == self->adc) {
            mp_printf(print, "ADC(%u, channel=%u)", i, self->channel);
            break;
        }
    }
}

STATIC mp_obj_t adc_obj_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    // Unpack and check parameter
    const machine_pin_obj_t *pin = pin_find(args[0]);

    if (pin->adc_list_len == 0) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Pin(%q) does not have ADC capabilities"), pin->name);
    }

    // Extract arguments
    ADC_Type *adc_instance = pin->adc_list[0].instance;  // NOTE: we only use the first ADC assignment - multiple assignments are not supported for now
    uint8_t channel = pin->adc_list[0].channel;

    #if 0 // done in adc_read_u16
    // Configure ADC peripheral channel
    adc_channel_config_t channel_config = {
        .channelNumber = (uint32_t)channel,
        .enableInterruptOnConversionCompleted = false,
    };
    ADC_SetChannelConfig(adc_instance, 0UL, &channel_config);  // NOTE: we always choose channel group '0' since we only perform software triggered conversion
    #endif

    // Create ADC Instance
    machine_adc_obj_t *o = mp_obj_malloc(machine_adc_obj_t, &machine_adc_type);
    o->adc = adc_instance;
    o->channel = (uint8_t)channel;
    o->channel_group = 0;
    o->resolution = 4096;  // NOTE: currently only 12bit resolution supported

    return MP_OBJ_FROM_PTR(o);
}

// read_u16()
#if defined(MIMXRT117x_SERIES)
STATIC mp_obj_t machine_adc_read_u16(mp_obj_t self_in) {
    machine_adc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    lpadc_conv_command_config_t adc_config;
    lpadc_conv_trigger_config_t trigger_config;

    // Set ADC configuration
    LPADC_GetDefaultConvCommandConfig(&adc_config);
    adc_config.channelNumber = self->channel;
    adc_config.sampleScaleMode = kLPADC_SamplePartScale;
    LPADC_SetConvCommandConfig(self->adc, 1, &adc_config);

    // Set Trigger mode
    LPADC_GetDefaultConvTriggerConfig(&trigger_config);
    trigger_config.targetCommandId = 1;
    LPADC_SetConvTriggerConfig(self->adc, 0U, &trigger_config);

    // Measure input voltage
    LPADC_DoSoftwareTrigger(self->adc, 1U);
    lpadc_conv_result_t result_struct;
    while (!LPADC_GetConvResult(self->adc, &result_struct)) {
    }

    return MP_OBJ_NEW_SMALL_INT(result_struct.convValue * 2);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_adc_read_u16_obj, machine_adc_read_u16);

void machine_adc_init(void) {
    lpadc_config_t adc_config;        // Set ADC configuration
    LPADC_GetDefaultConfig(&adc_config);
    adc_config.enableAnalogPreliminary = true;
    adc_config.referenceVoltageSource = kLPADC_ReferenceVoltageAlt1;
    LPADC_Init(LPADC1, &adc_config);
}

#else

STATIC mp_obj_t machine_adc_read_u16(mp_obj_t self_in) {
    machine_adc_obj_t *self = MP_OBJ_TO_PTR(self_in);

    // Initiate conversion
    adc_channel_config_t channel_config = {
        .channelNumber = self->channel,
        .enableInterruptOnConversionCompleted = false,
    };
    ADC_SetChannelConfig(self->adc, (uint32_t)self->channel_group, &channel_config);

    // Wait for conversion to finish
    while (!ADC_GetChannelStatusFlags(self->adc, (uint32_t)self->channel_group)) {
        // do nothing
    }

    // Measure input voltage
    uint32_t value = ADC_GetChannelConversionValue(self->adc, (uint32_t)self->channel_group);
    return MP_OBJ_NEW_SMALL_INT(value * 65535 / self->resolution);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_adc_read_u16_obj, machine_adc_read_u16);

void machine_adc_init(void) {
    for (int i = 1; i < sizeof(adc_bases) / sizeof(ADC_Type *); ++i) {
        ADC_Type *adc_instance = adc_bases[i];

        // Configure ADC perpheral
        adc_config_t config;
        ADC_GetDefaultConfig(&config);
        ADC_Init(adc_instance, &config);

        // Perform calibration
        status_t calib_state = ADC_DoAutoCalibration(adc_instance);

        if (calib_state == kStatus_Fail) {
            mp_printf(&mp_plat_print, "Calibration for ADC Instance %d failed", i);
        }
    }
}
#endif

STATIC const mp_rom_map_elem_t adc_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_read_u16), MP_ROM_PTR(&machine_adc_read_u16_obj) },
};

STATIC MP_DEFINE_CONST_DICT(adc_locals_dict, adc_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_adc_type,
    MP_QSTR_ADC,
    MP_TYPE_FLAG_NONE,
    make_new, adc_obj_make_new,
    print, adc_obj_print,
    locals_dict, &adc_locals_dict
    );
