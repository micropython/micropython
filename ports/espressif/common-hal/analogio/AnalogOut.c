/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2019, Lucian Copeland for Adafruit Industries
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
#include "shared-bindings/microcontroller/Pin.h"

#ifdef SOC_DAC_SUPPORTED
#include "driver/dac_oneshot.h"

#if defined(CONFIG_IDF_TARGET_ESP32)
#define pin_CHANNEL_0 pin_GPIO25
#define pin_CHANNEL_1 pin_GPIO26
#elif defined(CONFIG_IDF_TARGET_ESP32S2)
#define pin_CHANNEL_0 pin_GPIO17
#define pin_CHANNEL_1 pin_GPIO18
#endif
static dac_oneshot_handle_t _active_handles[SOC_DAC_CHAN_NUM];
#endif


void common_hal_analogio_analogout_construct(analogio_analogout_obj_t *self,
    const mcu_pin_obj_t *pin) {
    #ifdef SOC_DAC_SUPPORTED
    if (pin == &pin_CHANNEL_0) {
        self->channel = DAC_CHAN_0;
    } else if (pin == &pin_CHANNEL_1) {
        self->channel = DAC_CHAN_1;
    } else {
        raise_ValueError_invalid_pin();
    }
    dac_oneshot_config_t config = {
        .chan_id = self->channel
    };
    dac_oneshot_new_channel(&config, &self->handle);
    _active_handles[self->channel] = self->handle;
    #else
    mp_raise_NotImplementedError(NULL);
    #endif
}

bool common_hal_analogio_analogout_deinited(analogio_analogout_obj_t *self) {
    #ifdef SOC_DAC_SUPPORTED
    return self->handle == NULL;
    #else
    return false;
    #endif
}

void common_hal_analogio_analogout_deinit(analogio_analogout_obj_t *self) {
    #ifdef SOC_DAC_SUPPORTED
    dac_oneshot_del_channel(self->handle);
    self->handle = NULL;
    _active_handles[self->channel] = NULL;
    #endif
}

void common_hal_analogio_analogout_set_value(analogio_analogout_obj_t *self,
    uint16_t value) {
    #ifdef SOC_DAC_SUPPORTED
    uint8_t dac_value = (value * 255) / 65535;
    dac_oneshot_output_voltage(self->handle, dac_value);
    #endif
}

void analogout_reset(void) {
    #ifdef SOC_DAC_SUPPORTED
    for (uint8_t c = 0; c < SOC_DAC_CHAN_NUM; c++) {
        if (_active_handles[c] != NULL) {
            dac_oneshot_del_channel(_active_handles[c]);
        }
        _active_handles[c] = NULL;
    }
    #endif
}
