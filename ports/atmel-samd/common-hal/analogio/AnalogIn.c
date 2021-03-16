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

#include "samd/adc.h"
#include "shared-bindings/analogio/AnalogIn.h"
#include "supervisor/shared/translate.h"

#include "atmel_start_pins.h"
#include "hal/include/hal_adc_sync.h"
#include "hpl/gclk/hpl_gclk_base.h"

#ifdef SAMD21
#include "hpl/pm/hpl_pm_base.h"
#endif

void common_hal_analogio_analogin_construct(analogio_analogin_obj_t *self,
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
        mp_raise_ValueError(translate("Pin does not have ADC capabilities"));
    }
    claim_pin(pin);

    gpio_set_pin_function(pin->number, GPIO_PIN_FUNCTION_B);

    static Adc *adc_insts[] = ADC_INSTS;
    self->instance = adc_insts[adc_index];
    self->channel = adc_channel;
    self->pin = pin;
}

bool common_hal_analogio_analogin_deinited(analogio_analogin_obj_t *self) {
    return self->pin == NULL;
}

void common_hal_analogio_analogin_deinit(analogio_analogin_obj_t *self) {
    if (common_hal_analogio_analogin_deinited(self)) {
        return;
    }
    reset_pin_number(self->pin->number);
    self->pin = NULL;
}

void analogin_reset() {
}

uint16_t common_hal_analogio_analogin_get_value(analogio_analogin_obj_t *self) {
    // Something else might have used the ADC in a different way,
    // so we completely re-initialize it.

    struct adc_sync_descriptor adc;

    samd_peripherals_adc_setup(&adc, self->instance);

    // Full scale is 3.3V (VDDANA) = 65535.

    // On SAMD21, INTVCC1 is 0.5*VDDANA. On SAMD51, INTVCC1 is 1*VDDANA.
    // So on SAMD21 only, divide the input by 2, so full scale will match 0.5*VDDANA.
    adc_sync_set_reference(&adc, ADC_REFCTRL_REFSEL_INTVCC1_Val);
    #ifdef SAMD21
    adc_sync_set_channel_gain(&adc, self->channel, ADC_INPUTCTRL_GAIN_DIV2_Val);
    #endif

    adc_sync_set_resolution(&adc, ADC_CTRLB_RESSEL_12BIT_Val);

    adc_sync_enable_channel(&adc, self->channel);

    // We need to set the inputs because the above channel enable only enables the ADC.
    adc_sync_set_inputs(&adc, self->channel, ADC_INPUTCTRL_MUXNEG_GND_Val, self->channel);

    // Read twice and discard first result, as recommended in section 14 of
    // http://www.atmel.com/images/Atmel-42645-ADC-Configurations-with-Examples_ApplicationNote_AT11481.pdf
    // "Discard the first conversion result whenever there is a change in ADC configuration
    // like voltage reference / ADC channel change"
    // Empirical observation shows the first reading is quite different than subsequent ones.

    uint16_t value;
    adc_sync_read_channel(&adc, self->channel, ((uint8_t *)&value), 2);
    adc_sync_read_channel(&adc, self->channel, ((uint8_t *)&value), 2);

    adc_sync_deinit(&adc);
    // Shift the value to be 16 bit.
    return value << 4;
}

float common_hal_analogio_analogin_get_reference_voltage(analogio_analogin_obj_t *self) {
    return 3.3f;
}
