/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
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

#include "common-hal/analogio/AnalogIn.h"

#include <string.h>

#include "py/gc.h"
#include "py/nlr.h"
#include "py/runtime.h"
#include "py/binary.h"
#include "py/mphal.h"

#include "shared-bindings/analogio/AnalogIn.h"

#include "atmel_start_pins.h"
#include "hal/include/hal_adc_sync.h"
#include "hpl/gclk/hpl_gclk_base.h"

#ifdef SAMD21
#include "hpl/pm/hpl_pm_base.h"
#endif

void common_hal_analogio_analogin_construct(analogio_analogin_obj_t* self,
        const mcu_pin_obj_t *pin) {
    uint8_t adc_index;
    uint8_t adc_channel = 0xff;
    for (adc_index = 0; adc_index < NUM_ADC_PER_PIN; adc_index++) {
        // TODO(tannewt): Only use ADC0 on the SAMD51 when touch isn't being
        // used.
        if (pin->adc_input[adc_index] != 0xff) {
            adc_channel = pin->adc_input[adc_index];
            break;
        }
    }
    if (adc_channel == 0xff) {
        // No ADC function on that pin
        mp_raise_ValueError("Pin does not have ADC capabilities");
    }
    claim_pin(pin);

    gpio_set_pin_function(pin->pin, GPIO_PIN_FUNCTION_B);

    static Adc* adc_insts[] = ADC_INSTS;
    self->instance = adc_insts[adc_index];
    self->channel = adc_channel;
    self->pin = pin;
}

bool common_hal_analogio_analogin_deinited(analogio_analogin_obj_t *self) {
    return self->pin == mp_const_none;
}

void common_hal_analogio_analogin_deinit(analogio_analogin_obj_t *self) {
    if (common_hal_analogio_analogin_deinited(self)) {
        return;
    }
    reset_pin(self->pin->pin);
    self->pin = mp_const_none;
}

void analogin_reset() {
}

uint16_t common_hal_analogio_analogin_get_value(analogio_analogin_obj_t *self) {
    // Something else might have used the ADC in a different way,
    // so we completely re-initialize it.

    // Turn the clocks on.
    #ifdef SAMD51
    if (self->instance == ADC0) {
        hri_mclk_set_APBDMASK_ADC0_bit(MCLK);
        hri_gclk_write_PCHCTRL_reg(GCLK, ADC0_GCLK_ID, GCLK_PCHCTRL_GEN_GCLK1_Val | (1 << GCLK_PCHCTRL_CHEN_Pos));
    } else if (self->instance == ADC1) {
        hri_mclk_set_APBDMASK_ADC1_bit(MCLK);
        hri_gclk_write_PCHCTRL_reg(GCLK, ADC1_GCLK_ID, GCLK_PCHCTRL_GEN_GCLK1_Val | (1 << GCLK_PCHCTRL_CHEN_Pos));
    }
    #endif

    #ifdef SAMD21
    _pm_enable_bus_clock(PM_BUS_APBC, ADC);
    _gclk_enable_channel(ADC_GCLK_ID, GCLK_CLKCTRL_GEN_GCLK0_Val);
    #endif

    struct adc_sync_descriptor adc;
    adc_sync_init(&adc, self->instance, (void *)NULL);
    adc_sync_set_reference(&adc, ADC_REFCTRL_REFSEL_INTVCC1_Val);
    adc_sync_set_resolution(&adc, ADC_CTRLB_RESSEL_12BIT_Val);

    #ifdef SAMD21
    adc_sync_set_channel_gain(&adc, self->channel, ADC_INPUTCTRL_GAIN_DIV2_Val);

    // Load the factory calibration
    hri_adc_write_CALIB_BIAS_CAL_bf(ADC, (*((uint32_t*) ADC_FUSES_BIASCAL_ADDR) & ADC_FUSES_BIASCAL_Msk) >> ADC_FUSES_BIASCAL_Pos);
    // Bits 7:5
    uint16_t linearity = ((*((uint32_t*) ADC_FUSES_LINEARITY_1_ADDR) & ADC_FUSES_LINEARITY_1_Msk) >> ADC_FUSES_LINEARITY_1_Pos) << 5;
    // Bits 4:0
    linearity |= (*((uint32_t*) ADC_FUSES_LINEARITY_0_ADDR) & ADC_FUSES_LINEARITY_0_Msk) >> ADC_FUSES_LINEARITY_0_Pos;
    hri_adc_write_CALIB_LINEARITY_CAL_bf(ADC, linearity);
    #endif

    // SAMD51 has a CALIB register but doesn't have documented fuses for them.
    #ifdef SAMD51
    uint8_t biasrefbuf;
    uint8_t biasr2r;
    uint8_t biascomp;
    if (self->instance == ADC0) {
        biasrefbuf = ((*(uint32_t*) ADC0_FUSES_BIASREFBUF_ADDR) & ADC0_FUSES_BIASREFBUF_Msk) >> ADC0_FUSES_BIASREFBUF_Pos;
        biasr2r = ((*(uint32_t*) ADC0_FUSES_BIASR2R_ADDR) & ADC0_FUSES_BIASR2R_Msk) >> ADC0_FUSES_BIASR2R_Pos;
        biascomp = ((*(uint32_t*) ADC0_FUSES_BIASCOMP_ADDR) & ADC0_FUSES_BIASCOMP_Msk) >> ADC0_FUSES_BIASCOMP_Pos;
    } else {
        biasrefbuf = ((*(uint32_t*) ADC1_FUSES_BIASREFBUF_ADDR) & ADC1_FUSES_BIASREFBUF_Msk) >> ADC1_FUSES_BIASREFBUF_Pos;
        biasr2r = ((*(uint32_t*) ADC1_FUSES_BIASR2R_ADDR) & ADC1_FUSES_BIASR2R_Msk) >> ADC1_FUSES_BIASR2R_Pos;
        biascomp = ((*(uint32_t*) ADC1_FUSES_BIASCOMP_ADDR) & ADC1_FUSES_BIASCOMP_Msk) >> ADC1_FUSES_BIASCOMP_Pos;
    }
    hri_adc_write_CALIB_BIASREFBUF_bf(self->instance, biasrefbuf);
    hri_adc_write_CALIB_BIASR2R_bf(self->instance, biasr2r);
    hri_adc_write_CALIB_BIASCOMP_bf(self->instance, biascomp);
    #endif

    adc_sync_enable_channel(&adc, self->channel);

    // We need to set the inputs because the above channel enable only enables the ADC.
    adc_sync_set_inputs(&adc, self->channel, ADC_INPUTCTRL_MUXNEG_GND_Val, self->channel);

    // Read twice and discard first result, as recommended in section 14 of
    // http://www.atmel.com/images/Atmel-42645-ADC-Configurations-with-Examples_ApplicationNote_AT11481.pdf
    // "Discard the first conversion result whenever there is a change in ADC configuration
    // like voltage reference / ADC channel change"
    // Empirical observation shows the first reading is quite different than subsequent ones.

    uint16_t value;
    adc_sync_read_channel(&adc, self->channel, ((uint8_t*) &value), 2);
    adc_sync_read_channel(&adc, self->channel, ((uint8_t*) &value), 2);

    adc_sync_deinit(&adc);
    // Shift the value to be 16 bit.
    return value << 4;
}

float common_hal_analogio_analogin_get_reference_voltage(analogio_analogin_obj_t *self) {
    return 3.3f;
}
