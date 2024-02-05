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
#include "extmod/modmachine.h"

#include "nina_bsp.h"
#include "nina_wifi_drv.h"

#if NINA_DEBUG
#define debug_printf(...) mp_printf(&mp_plat_print, __VA_ARGS__)
#else
#define debug_printf(...)
#endif

int nina_bsp_init(void) {
    mp_hal_pin_output(MICROPY_HW_NINA_GPIO1);
    mp_hal_pin_input(MICROPY_HW_NINA_ACK);
    mp_hal_pin_output(MICROPY_HW_NINA_RESET);
    mp_hal_pin_output(MICROPY_HW_NINA_GPIO0);

    // Reset module in WiFi mode
    mp_hal_pin_write(MICROPY_HW_NINA_GPIO1, 1);
    mp_hal_pin_write(MICROPY_HW_NINA_GPIO0, 1);

    mp_hal_pin_write(MICROPY_HW_NINA_RESET, 0);
    mp_hal_delay_ms(100);

    mp_hal_pin_write(MICROPY_HW_NINA_RESET, 1);
    mp_hal_delay_ms(750);

    mp_hal_pin_write(MICROPY_HW_NINA_GPIO0, 0);
    mp_hal_pin_input(MICROPY_HW_NINA_GPIO0);

    // Initialize SPI.
    mp_obj_t args[] = {
        MP_OBJ_NEW_SMALL_INT(MICROPY_HW_WIFI_SPI_ID),
        MP_OBJ_NEW_SMALL_INT(MICROPY_HW_WIFI_SPI_BAUDRATE),
    };

    MP_STATE_PORT(mp_wifi_spi) = MP_OBJ_TYPE_GET_SLOT(&machine_spi_type, make_new)((mp_obj_t)&machine_spi_type, 2, 0, args);
    return 0;
}

int nina_bsp_deinit(void) {
    mp_hal_pin_output(MICROPY_HW_NINA_GPIO1);
    mp_hal_pin_write(MICROPY_HW_NINA_GPIO1, 1);

    mp_hal_pin_output(MICROPY_HW_NINA_RESET);
    mp_hal_pin_write(MICROPY_HW_NINA_RESET, 0);
    mp_hal_delay_ms(100);

    mp_hal_pin_output(MICROPY_HW_NINA_GPIO0);
    mp_hal_pin_write(MICROPY_HW_NINA_GPIO0, 1);
    return 0;
}

int nina_bsp_atomic_enter(void) {
    #if MICROPY_ENABLE_SCHEDULER
    mp_sched_lock();
    #endif
    return 0;
}

int nina_bsp_atomic_exit(void) {
    #if MICROPY_ENABLE_SCHEDULER
    mp_sched_unlock();
    #endif
    return 0;
}

int nina_bsp_read_irq(void) {
    return mp_hal_pin_read(MICROPY_HW_NINA_GPIO0);
}

int nina_bsp_spi_slave_select(uint32_t timeout) {
    // Wait for ACK to go low.
    for (mp_uint_t start = mp_hal_ticks_ms(); mp_hal_pin_read(MICROPY_HW_NINA_ACK) == 1; mp_hal_delay_ms(1)) {
        if (timeout && ((mp_hal_ticks_ms() - start) >= timeout)) {
            return -1;
        }
    }

    // Chip select.
    mp_hal_pin_write(MICROPY_HW_NINA_GPIO1, 0);

    // Wait for ACK to go high.
    for (mp_uint_t start = mp_hal_ticks_ms(); mp_hal_pin_read(MICROPY_HW_NINA_ACK) == 0; mp_hal_delay_ms(1)) {
        if ((mp_hal_ticks_ms() - start) >= 100) {
            mp_hal_pin_write(MICROPY_HW_NINA_GPIO1, 1);
            return -1;
        }
    }

    return 0;
}

int nina_bsp_spi_slave_deselect(void) {
    mp_hal_pin_write(MICROPY_HW_NINA_GPIO1, 1);
    return 0;
}

int nina_bsp_spi_transfer(const uint8_t *tx_buf, uint8_t *rx_buf, uint32_t size) {
    mp_obj_t mp_wifi_spi = MP_STATE_PORT(mp_wifi_spi);
    ((mp_machine_spi_p_t *)MP_OBJ_TYPE_GET_SLOT(&machine_spi_type, protocol))->transfer(mp_wifi_spi, size, tx_buf, rx_buf);
    #if NINA_DEBUG
    for (int i = 0; i < size; i++) {
        if (tx_buf) {
            debug_printf("0x%x ", tx_buf[i]);
        } else {
            debug_printf("0x%x ", rx_buf[i]);
        }
    }
    #endif
    return 0;
}

#endif // MICROPY_PY_NETWORK_NINAW10
