/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Ibrahim Abdelkader <iabdalkader@openmv.io>
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

#include <stdio.h>
#include <string.h>

#include "py/runtime.h"
#include "py/mphal.h"

#if defined(MICROPY_PY_NETWORK_NINAW10) && defined(MICROPY_HW_PIN_EXT_COUNT)

#include "modmachine.h"
#include "machine_pin.h"
#include "nina_wifi_drv.h"

#define NINA_GPIO_INPUT         (0x00)
#define NINA_GPIO_OUTPUT        (0x01)
#define NINA_GPIO_INPUT_PULLUP  (0x02)

#define NINA_GPIO_MODE          (0x50)
#define NINA_GPIO_READ          (0x53)
#define NINA_GPIO_READ_ANALOG   (0x54)
#define NINA_GPIO_WRITE         (0x51)
#define NINA_GPIO_IS_INPUT_ONLY(p)  ((p >= 3 && p <= 6))
#define NINA_GPIO_IS_ADC_CHANNEL(p) ((p >= 3 && p <= 6))

// This maps logical pin ID (0..MICROPY_HW_PIN_EXT_COUNT) to
// physical pins on the Nina module.
static uint8_t pin_map[MICROPY_HW_PIN_EXT_COUNT] = {
    27, // LEDR
    25, // LEDG
    26, // LEDB
    34, // A4
    39, // A5
    36, // A6
    35, // A7
};

// This maps logical pin ID (0..MICROPY_HW_PIN_EXT_COUNT) to
// ADC channel numbers on the Nina module.
static uint8_t adc_map[MICROPY_HW_PIN_EXT_COUNT] = {
    -1, // LEDR
    -1, // LEDG
    -1, // LEDB
    6, // A4
    3, // A5
    0, // A6
    7, // A7
};

void machine_pin_ext_init(void) {
    nina_init();
}

bool machine_pin_ext_is_adc_channel(const machine_pin_obj_t *self) {
    return NINA_GPIO_IS_ADC_CHANNEL(self->id);
}

uint32_t machine_pin_ext_to_adc_channel(const machine_pin_obj_t *self) {
    return adc_map[self->id];
}

void machine_pin_ext_set(machine_pin_obj_t *self, bool value) {
    if (self->id >= 0 && self->id < MICROPY_HW_PIN_EXT_COUNT) {
        uint8_t buf[] = {pin_map[self->id], value};
        nina_ioctl(NINA_GPIO_WRITE, sizeof(buf), buf, 0);
    }
}

bool machine_pin_ext_get(machine_pin_obj_t *self) {
    bool value = false;
    if (self->id >= 0 && self->id < MICROPY_HW_PIN_EXT_COUNT) {
        uint8_t buf[] = {pin_map[self->id]};
        nina_ioctl(NINA_GPIO_READ, sizeof(buf), buf, 0);
        value = buf[0];
    }
    return value;
}

uint16_t machine_pin_ext_read_u16(uint32_t channel) {
    uint16_t buf = channel;
    nina_ioctl(NINA_GPIO_READ_ANALOG, sizeof(buf), (uint8_t *)&buf, 0);
    return buf;
}

void machine_pin_ext_config(machine_pin_obj_t *self, int mode, int value) {
    if (mode == MACHINE_PIN_MODE_IN || mode == MACHINE_PIN_MODE_ANALOG) {
        mode = NINA_GPIO_INPUT;
        self->is_output = false;
    } else if (mode == MACHINE_PIN_MODE_OUT) {
        mode = NINA_GPIO_OUTPUT;
        self->is_output = true;
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("only Pin.OUT and Pin.IN are supported for this pin"));
    }
    if (self->id >= 0 && self->id < MICROPY_HW_PIN_EXT_COUNT) {
        uint8_t buf[] = {pin_map[self->id], mode};
        if (mode == NINA_GPIO_OUTPUT) {
            if (NINA_GPIO_IS_INPUT_ONLY(self->id)) {
                mp_raise_ValueError(MP_ERROR_TEXT("only Pin.IN is supported for this pin"));
            }
            machine_pin_ext_set(self, value);
        }
        nina_ioctl(NINA_GPIO_MODE, sizeof(buf), buf, 0);
    }
}

#endif // defined(MICROPY_PY_NETWORK_NINAW10) && defined(MICROPY_HW_PIN_EXT_COUNT)
