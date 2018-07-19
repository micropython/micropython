/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Glenn Ruben Bakke
 * Copyright (c) 2018 Artur Pacholec
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

#include "ble_drv.h"
#include "nrfx.h"
#include "nrf_error.h"
#include "nrf_sdm.h"
#include "py/nlr.h"
#include "shared-module/bleio/Address.h"

void common_hal_bleio_adapter_set_enabled(bool enabled) {
    uint32_t err_code;

    if (enabled) {
        err_code = ble_drv_stack_enable();
    } else {
        err_code = sd_softdevice_disable();
    }

    if (err_code != NRF_SUCCESS) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError,
                  "Failed to change softdevice status, error: 0x%08lX", err_code));
    }
}

bool common_hal_bleio_adapter_get_enabled(void) {
    return ble_drv_stack_enabled();
}

void common_hal_bleio_adapter_get_address(bleio_address_obj_t *address) {
    ble_drv_address_get(address);
}
