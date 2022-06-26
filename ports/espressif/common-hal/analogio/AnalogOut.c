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
#include "supervisor/shared/translate/translate.h"

#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2)
#include "components/driver/include/driver/dac_common.h"
#define HAS_DAC 1
#if defined(CONFIG_IDF_TARGET_ESP32)
#define pin_CHANNEL_1 pin_GPIO25
#define pin_CHANNEL_2 pin_GPIO26
#elif defined(CONFIG_IDF_TARGET_ESP32S2)
#define pin_CHANNEL_1 pin_GPIO17
#define pin_CHANNEL_2 pin_GPIO18
#endif
#else
#define HAS_DAC 0
#endif

void common_hal_analogio_analogout_construct(analogio_analogout_obj_t *self,
    const mcu_pin_obj_t *pin) {
    #if HAS_DAC
    if (pin == &pin_CHANNEL_1) {
        self->channel = DAC_CHANNEL_1;
    } else if (pin == &pin_CHANNEL_2) {
        self->channel = DAC_CHANNEL_2;
    }
    else {
        raise_ValueError_invalid_pin();
    }
    dac_output_enable(self->channel);
    #else
    mp_raise_NotImplementedError(NULL);
    #endif
}

bool common_hal_analogio_analogout_deinited(analogio_analogout_obj_t *self) {
    #if HAS_DAC
    return self->channel == DAC_CHANNEL_MAX;
    #else
    return false;
    #endif
}

void common_hal_analogio_analogout_deinit(analogio_analogout_obj_t *self) {
    #if HAS_DAC
    dac_output_disable(self->channel);
    self->channel = DAC_CHANNEL_MAX;
    #endif
}

void common_hal_analogio_analogout_set_value(analogio_analogout_obj_t *self,
    uint16_t value) {
    #if HAS_DAC
    uint8_t dac_value = (value * 255) / 65535;
    dac_output_enable(self->channel);
    dac_output_voltage(self->channel, dac_value);
    #endif
}

void analogout_reset(void) {
    #if HAS_DAC
    dac_output_disable(DAC_CHANNEL_1);
    dac_output_disable(DAC_CHANNEL_2);
    #endif
}
