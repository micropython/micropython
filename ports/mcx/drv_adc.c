/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 NXP
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

#include "board.h"

#include "fsl_lpadc.h"
#include "fsl_spc.h"
#include "fsl_vref.h"

#include "drv_adc.h"

int drv_adc_init(drv_adc_t *adc, uint8_t id) {
    adc->instance = MCX_BoardGetADCInstance(id);
    if (adc->instance == NULL) {
        return -EINVAL;
    }

    adc->id = id;

    int adc_inputfreq = MCX_BoardConfigureADCClock(id);
    if (adc_inputfreq <= 0) {
        return -EINVAL;
    }

    ADC_Type *lpadc = adc->instance;

    SPC_EnableActiveModeAnalogModules(SPC0, kSPC_controlVref);

    vref_config_t vref_cfg;
    VREF_GetDefaultConfig(&vref_cfg);

    vref_cfg.bufferMode = kVREF_ModeBandgapOnly;

    VREF_Init(VREF0, &vref_cfg);

    lpadc_config_t adc_cfg;
    LPADC_GetDefaultConfig(&adc_cfg);

    adc_cfg.enableAnalogPreliminary = true;
    adc_cfg.referenceVoltageSource = kLPADC_ReferenceVoltageAlt3;

    LPADC_Init(lpadc, &adc_cfg);
    LPADC_DoOffsetCalibration(lpadc);
    LPADC_DoAutoCalibration(lpadc);

    return 0;
}

int drv_adc_measure(drv_adc_t *adc, drv_adc_side_t side, uint8_t ch, uint16_t *result) {
    ADC_Type *lpadc = adc->instance;

    lpadc_conv_command_config_t cmd_cfg;
    LPADC_GetDefaultConvCommandConfig(&cmd_cfg);

    cmd_cfg.channelNumber = ch;
    cmd_cfg.conversionResolutionMode = kLPADC_ConversionResolutionHigh;

    if (side == DRV_ADCSideA) {
        cmd_cfg.sampleChannelMode = kLPADC_SampleChannelSingleEndSideA;
    } else {
        cmd_cfg.sampleChannelMode = kLPADC_SampleChannelSingleEndSideB;
    }

    LPADC_SetConvCommandConfig(lpadc, 15, &cmd_cfg);

    lpadc_conv_trigger_config_t trig_cfg;
    LPADC_GetDefaultConvTriggerConfig(&trig_cfg);

    trig_cfg.targetCommandId = 15;
    trig_cfg.enableHardwareTrigger = false;

    LPADC_SetConvTriggerConfig(lpadc, 0U, &trig_cfg);

    LPADC_DoSoftwareTrigger(lpadc, 1U << 0U);

    lpadc_conv_result_t rs;

    while (!LPADC_GetConvResult(lpadc, &rs, 0U)) {
        ;
    }

    *result = rs.convValue;

    return 0;
}
