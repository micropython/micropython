/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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
#include <string.h>

#include "py/mperrno.h"
#include "py/runtime.h"

#include "shared-bindings/analogio/AnalogOut.h"

#include "asf/sam0/drivers/dac/dac.h"
#include "samd21_pins.h"

void common_hal_analogio_analogout_construct(analogio_analogout_obj_t* self,
        const mcu_pin_obj_t *pin) {
    if (pin->pin != PIN_PA02) {
        mp_raise_ValueError("AnalogOut not supported on given pin");
        return;
    }
    struct dac_config config_dac;
    dac_get_config_defaults(&config_dac);
    config_dac.reference = DAC_REFERENCE_AVCC;
    enum status_code status = dac_init(&self->dac_instance, DAC, &config_dac);
    if (status != STATUS_OK) {
        mp_raise_OSError(MP_EIO);
        return;
    }
    claim_pin(pin);

    struct dac_chan_config config_analogout_chan;
    dac_chan_get_config_defaults(&config_analogout_chan);
    dac_chan_set_config(&self->dac_instance, DAC_CHANNEL_0, &config_analogout_chan);
    dac_chan_enable(&self->dac_instance, DAC_CHANNEL_0);

    dac_enable(&self->dac_instance);
}

bool common_hal_analogio_analogout_deinited(analogio_analogout_obj_t *self) {
    return self->deinited;
}

void common_hal_analogio_analogout_deinit(analogio_analogout_obj_t *self) {
    if (common_hal_analogio_analogout_deinited(self)) {
        return;
    }
    dac_disable(&self->dac_instance);
    dac_chan_disable(&self->dac_instance, DAC_CHANNEL_0);
    reset_pin(PIN_PA02);
    self->deinited = true;
}

void common_hal_analogio_analogout_set_value(analogio_analogout_obj_t *self,
        uint16_t value) {
    // Input is 16 bit but we only support 10 bit so we shift the input.
    dac_chan_write(&self->dac_instance, DAC_CHANNEL_0, value >> 6);
}
