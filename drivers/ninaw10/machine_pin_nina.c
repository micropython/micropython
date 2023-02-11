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
#define NINA_GPIO_WRITE         (0x51)

static uint8_t pin_map[MICROPY_HW_PIN_EXT_COUNT] = {
    27, // LEDR
    25, // LEDG
    26, // LEDB
    36, // A6
    35, // A7
};

void machine_pin_ext_init(void) {
    nina_init();
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
    }
    return value;
}

void machine_pin_ext_config(machine_pin_obj_t *self, int mode, int value) {
    if (mode == MACHINE_PIN_MODE_IN) {
        mode = NINA_GPIO_INPUT;
        self->is_output = false;
    } else if (mode == MACHINE_PIN_MODE_OUT) {
        mode = NINA_GPIO_OUTPUT;
        self->is_output = true;
        machine_pin_ext_set(self, value);
    } else {
        mp_raise_ValueError("only Pin.OUT and Pin.IN are supported for this pin");
    }
    if (self->id >= 0 && self->id < MICROPY_HW_PIN_EXT_COUNT) {
        uint8_t buf[] = {pin_map[self->id], mode};
        nina_ioctl(NINA_GPIO_MODE, sizeof(buf), buf, 0);
    }
}

#endif // defined(MICROPY_PY_NETWORK_NINAW10) && defined(MICROPY_HW_PIN_EXT_COUNT)
