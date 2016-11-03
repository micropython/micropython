/*
 * This file is part of the Micro Python project, http://micropython.org/
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

#include <string.h>

#include "py/nlr.h"
#include "py/runtime.h"
#include "py/binary.h"
#include "py/mphal.h"
#include "shared-bindings/nativeio/AnalogIn.h"

#include "asf/sam0/drivers/adc/adc.h"

void common_hal_nativeio_analogin_construct(nativeio_analogin_obj_t* self,
        const mcu_pin_obj_t *pin) {
    if (!pin->has_adc) {
        // No ADC function on that pin
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "pin %q does not have ADC capabilities", pin->name));
    }

    self->pin = pin;

    struct adc_config config_adc;
    adc_get_config_defaults(&config_adc);

    config_adc.positive_input = self->pin->adc_input;
    config_adc.resolution = ADC_RESOLUTION_CUSTOM;
    config_adc.accumulate_samples = ADC_ACCUMULATE_SAMPLES_16;
    config_adc.divide_result = ADC_DIVIDE_RESULT_16;
    config_adc.clock_prescaler = ADC_CLOCK_PRESCALER_DIV128;

    adc_init(&self->adc_instance, ADC, &config_adc);
}

// TODO(tannewt): Don't turn it all on just for one read. This simplifies
// handling of reading multiple inputs and surviving sleep though so for now its
// ok.
uint16_t common_hal_nativeio_analogin_get_value(nativeio_analogin_obj_t *self) {
    adc_enable(&self->adc_instance);
    adc_start_conversion(&self->adc_instance);

    uint16_t data;
    enum status_code status = adc_read(&self->adc_instance, &data);
    while (status == STATUS_BUSY) {
      status = adc_read(&self->adc_instance, &data);
    }
    if (status == STATUS_ERR_OVERFLOW) {
      // TODO(tannewt): Throw an error.
    }

    adc_disable(&self->adc_instance);
    return data;
}
