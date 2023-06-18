/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2022 Damien P. George
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

#if defined(MICROPY_PY_NETWORK_CYW43) && defined(MICROPY_HW_PIN_EXT_COUNT)

#include "modmachine.h"
#include "machine_pin.h"
#include "lib/cyw43-driver/src/cyw43.h"

void machine_pin_ext_init(void) {
}

void machine_pin_ext_set(machine_pin_obj_t *self, bool value) {
    if (value != self->last_output_value || !self->is_output) {
        cyw43_gpio_set(&cyw43_state, self->id, value);
    }
    self->last_output_value = value;
}

bool machine_pin_ext_get(machine_pin_obj_t *self) {
    bool value = false;
    cyw43_gpio_get(&cyw43_state, self->id, &value);
    return value;
}

void machine_pin_ext_config(machine_pin_obj_t *self, int mode, int value) {
    if (mode == MACHINE_PIN_MODE_IN) {
        if (self->is_output) {
            // TODO need to disable output
        }
        self->is_output = false;
    } else if (mode == MACHINE_PIN_MODE_OUT) {
        if (!self->is_output) {
            // TODO need to enable output
            // for now we just set the value
            if (value == -1) {
                value = self->last_output_value;
            }
            self->last_output_value = !self->last_output_value; // defeat shortcircuit
            machine_pin_ext_set(self, value);
            self->is_output = true;
        }
    } else {
        mp_raise_ValueError("only Pin.OUT and Pin.IN are supported for this pin");
    }

    if (value != -1) {
        if (self->is_output) {
            machine_pin_ext_set(self, value);
        } else {
            // figure if you pass a value to IN it should still remember it (this is what regular GPIO does)
            self->last_output_value = value;
        }
    }
}

#endif // defined(MICROPY_PY_NETWORK_CYW43) && defined(MICROPY_HW_PIN_EXT_COUNT)
