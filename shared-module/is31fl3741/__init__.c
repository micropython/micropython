/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Mark Komus
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


#include "shared-bindings/is31fl3741/__init__.h"
#include "shared-bindings/busio/I2C.h"
#include "shared-bindings/is31fl3741/IS31FL3741.h"

void is31fl3741_begin_transaction(busio_i2c_obj_t *i2c) {
    while (!common_hal_busio_i2c_try_lock(i2c)) {
        RUN_BACKGROUND_TASKS;
        if (mp_hal_is_interrupted()) {
            break;
        }
    }
}

void is31fl3741_end_transaction(busio_i2c_obj_t *i2c) {
    common_hal_busio_i2c_unlock(i2c);
}

void common_hal_is31fl3741_init(busio_i2c_obj_t *i2c, uint8_t addr) {
    is31fl3741_begin_transaction(i2c);

    uint8_t command = 0xFC; // device ID
    common_hal_busio_i2c_write(i2c, addr, &command, 1, false);
    uint8_t data = 0;
    common_hal_busio_i2c_read(i2c, addr, &data, 1);

    is31fl3741_send_reset(i2c, addr);
    is31fl3741_send_enable(i2c, addr);
    is31fl3741_set_current(i2c, addr, 0xFF);

    // set scale (brightness) to max for all LEDs
    for (int i = 0; i < 351; i++) {
        is31fl3741_set_led(i2c, addr, i, 0xFF, 2);
    }

    is31fl3741_end_transaction(i2c);
}

void common_hal_is31fl3741_write(busio_i2c_obj_t *i2c, uint8_t addr, const mp_obj_t *mapping, const uint8_t *pixels, size_t numBytes) {
    is31fl3741_begin_transaction(i2c);

    for (size_t i = 0; i < numBytes; i += 3) {
        uint16_t ridx = mp_obj_get_int(mapping[i]);
        if (ridx != 65535) {
            is31fl3741_set_led(i2c, addr, ridx, IS31GammaTable[pixels[i]], 0); // red
            is31fl3741_set_led(i2c, addr, mp_obj_get_int(mapping[i + 1]), IS31GammaTable[pixels[i + 1]], 0); // green
            is31fl3741_set_led(i2c, addr, mp_obj_get_int(mapping[i + 2]), IS31GammaTable[pixels[i + 2]], 0); // blue
        }
    }

    is31fl3741_end_transaction(i2c);
}
