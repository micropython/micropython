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

// This file is never compiled standalone, it's included directly from
// extmod/machine_adc.c via MICROPY_PY_MACHINE_ADC_INCLUDEFILE.

#include "py/mphal.h"

#if defined(MIMXRT117x_SERIES)
#include "fsl_lpadc.h"
#else
#include "fsl_adc.h"
#endif

#include "fsl_gpio.h"
#include "fsl_iomuxc.h"

// The ADC class doesn't have any constants for this port.
#define MICROPY_PY_MACHINE_ADC_CLASS_CONSTANTS

#if defined(MIMXRT117x_SERIES)
#define ADC_UV_FULL_RANGE (3840000)
#else
#define ADC_UV_FULL_RANGE (3300000)
#endif

typedef struct _machine_adc_obj_t {
    mp_obj_base_t base;
    ADC_Type *adc;
    uint8_t channel;
    uint8_t channel_group;
    uint16_t resolution;
} machine_adc_obj_t;

static ADC_Type *const adc_bases[] = ADC_BASE_PTRS;

static void mp_machine_adc_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    machine_adc_obj_t *self = MP_OBJ_TO_PTR(self_in);

    // Get ADC adc id
    for (int i = 1; i < sizeof(adc_bases) / sizeof(ADC_Type *); ++i) {
        if (adc_bases[i] == self->adc) {
            mp_printf(print, "ADC(%u, channel=%u)", i, self->channel);
            break;
        }
    }
}

static mp_obj_t mp_machine_adc_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    // Unpack and check parameter
    const machine_pin_obj_t *pin = pin_find(all_args[0]);

    if (pin->adc_list_len == 0) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Pin(%q) does not have ADC capabilities"), pin->name);
    }

    // Extract arguments
    ADC_Type *adc_instance = pin->adc_list[0].instance;  // NOTE: we only use the first ADC assignment - multiple assignments are not supported for now
    uint8_t channel = pin->adc_list[0].channel;

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
static mp_int_t mp_machine_adc_read_u16(machine_adc_obj_t *self) {
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

    return result_struct.convValue * 2;
}

void machine_adc_init(void) {
    lpadc_config_t adc_config;        // Set ADC configuration
    LPADC_GetDefaultConfig(&adc_config);
    adc_config.enableAnalogPreliminary = true;
    adc_config.referenceVoltageSource = kLPADC_ReferenceVoltageAlt1;
    LPADC_Init(LPADC1, &adc_config);
}

#else

static mp_int_t mp_machine_adc_read_u16(machine_adc_obj_t *self) {
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
    return value * 65535 / self->resolution;
}

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

static mp_int_t mp_machine_adc_read_uv(machine_adc_obj_t *self) {
    return (uint64_t)ADC_UV_FULL_RANGE * mp_machine_adc_read_u16(self) / 65536;
}
