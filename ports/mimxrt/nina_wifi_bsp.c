/*
 * This file is part of the OpenMV project, https://openmv.io.
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
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
 *
 * NINA-W10 driver BSP implementation.
 */

#include "py/mphal.h"

#if MICROPY_PY_NETWORK_NINAW10

#include <stdint.h>
#include <string.h>

#include "py/runtime.h"
#include "modmachine.h"
#include "mpconfigboard.h"

#include "nina_bsp.h"
#include "nina_wifi_drv.h"

#if NINA_DEBUG
#define debug_printf(...) mp_printf(&mp_plat_print, __VA_ARGS__)
#else
#define debug_printf(...)
#endif

int nina_bsp_spi_slave_select(uint32_t timeout) {
    // Wait for ACK to go low.
    for (mp_uint_t start = mp_hal_ticks_ms(); mp_hal_pin_read(MICROPY_HW_NINA_ACK) == 1; mp_hal_delay_us(100)) {
        if (timeout && ((mp_hal_ticks_ms() - start) >= timeout)) {
            return -1;
        }
    }

    // Chip select.
    mp_hal_pin_write(MICROPY_HW_NINA_CS, 0);

    // Wait for ACK to go high.
    for (mp_uint_t start = mp_hal_ticks_ms(); mp_hal_pin_read(MICROPY_HW_NINA_ACK) == 0; mp_hal_delay_us(100)) {
        if ((mp_hal_ticks_ms() - start) >= 100) {
            mp_hal_pin_write(MICROPY_HW_NINA_CS, 1);
            return -1;
        }
    }

    return 0;
}

#endif // MICROPY_PY_NETWORK_NINAW10
