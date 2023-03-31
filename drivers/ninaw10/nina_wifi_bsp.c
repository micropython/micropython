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
#include "extmod/machine_spi.h"
#include "mpconfigboard.h"

#include "nina_bsp.h"
#include "nina_wifi_drv.h"

#if NINA_DEBUG
#define debug_printf(...) mp_printf(&mp_plat_print, __VA_ARGS__)
#else
#define debug_printf(...)
#endif

#define PIN_NOT_SET     (0)

nina_wiring_t nina_wiring = {
    NULL,
    PIN_NOT_SET,
    PIN_NOT_SET,
    PIN_NOT_SET,
    PIN_NOT_SET,
};

void nina_bsp_wiring(mp_obj_type_t *spi, mp_obj_type_t *gpio1, mp_obj_type_t *ack, mp_obj_type_t *reset, mp_obj_type_t *gpio0) {
    nina_wiring.spi = spi;
    nina_wiring.gpio1 = mp_hal_get_pin_obj(gpio1);
    nina_wiring.ack = mp_hal_get_pin_obj(ack);
    nina_wiring.reset = mp_hal_get_pin_obj(reset);
    nina_wiring.gpio0 = mp_hal_get_pin_obj(gpio0);
}

int nina_bsp_init(void) {
    #ifdef MICROPY_HW_NINA_GPIO1
    if (nina_wiring.gpio1 == PIN_NOT_SET) {
        nina_wiring.gpio1 = MICROPY_HW_NINA_GPIO1;
    }
    #endif
    #ifdef MICROPY_HW_NINA_ACK
    if (nina_wiring.ack == PIN_NOT_SET) {
        nina_wiring.ack = MICROPY_HW_NINA_ACK;
    }
    #endif
    #ifdef MICROPY_HW_NINA_GPIO0
    if (nina_wiring.gpio0 == PIN_NOT_SET) {
        nina_wiring.gpio0 = MICROPY_HW_NINA_GPIO0;
    }
    #endif
    #ifdef MICROPY_HW_NINA_RESET
    if (nina_wiring.reset == PIN_NOT_SET) {
        nina_wiring.reset = MICROPY_HW_NINA_RESET;
    }
    #endif
    mp_hal_pin_output(nina_wiring.gpio1);
    mp_hal_pin_input(nina_wiring.ack);
    mp_hal_pin_output(nina_wiring.reset);
    mp_hal_pin_output(nina_wiring.gpio0);

    // Reset module in WiFi mode
    mp_hal_pin_write(nina_wiring.gpio1, 1);
    mp_hal_pin_write(nina_wiring.gpio0, 1);

    mp_hal_pin_write(nina_wiring.reset, 0);
    mp_hal_delay_ms(100);

    mp_hal_pin_write(nina_wiring.reset, 1);
    mp_hal_delay_ms(750);

    // Initialize SPI.
    if (nina_wiring.spi == NULL) {
        // use the hard-coded setting
        #ifdef MICROPY_HW_WIFI_SPI_ID
        #ifdef MICROPY_HW_WIFI_SPI_SCK
        mp_obj_t spi_obj = MP_OBJ_NEW_SMALL_INT(MICROPY_HW_WIFI_SPI_SCK);
        mp_obj_t miso_obj = MP_OBJ_NEW_SMALL_INT(MICROPY_HW_WIFI_SPI_MISO);
        mp_obj_t mosi_obj = MP_OBJ_NEW_SMALL_INT(MICROPY_HW_WIFI_SPI_MOSI);
        mp_obj_t args[] = {
            MP_OBJ_NEW_SMALL_INT(MICROPY_HW_WIFI_SPI_ID),
            MP_OBJ_NEW_SMALL_INT(MICROPY_HW_WIFI_SPI_BAUDRATE),
            MP_ROM_QSTR(MP_QSTR_sck), mp_pin_make_new(NULL, 1, 0, &spi_obj),
            MP_ROM_QSTR(MP_QSTR_miso), mp_pin_make_new(NULL, 1, 0, &miso_obj),
            MP_ROM_QSTR(MP_QSTR_mosi), mp_pin_make_new(NULL, 1, 0, &mosi_obj),
        };
        MP_STATE_PORT(mp_wifi_spi) = MP_OBJ_TYPE_GET_SLOT(&machine_spi_type, make_new)(
            (mp_obj_t)&machine_spi_type, 2, 3, args);
        #else  // MICROPY_HW_WIFI_SPI_SCK
        mp_obj_t args[] = {
            MP_OBJ_NEW_SMALL_INT(MICROPY_HW_WIFI_SPI_ID),
            MP_OBJ_NEW_SMALL_INT(MICROPY_HW_WIFI_SPI_BAUDRATE),
        };
        MP_STATE_PORT(mp_wifi_spi) = MP_OBJ_TYPE_GET_SLOT(&machine_spi_type, make_new)(
            (mp_obj_t)&machine_spi_type, 2, 0, args);
        #endif  // MICROPY_HW_WIFI_SPI_SCK
        #else  //  MICROPY_HW_WIFI_SPI_ID
        mp_raise_ValueError(MP_ERROR_TEXT("missing NINA wiring definition"));
        #endif  //  MICROPY_HW_WIFI_SPI_ID
    } else {
        MP_STATE_PORT(mp_wifi_spi) = (struct _machine_spi_obj_t *)nina_wiring.spi;
    }
    return 0;
}

int nina_bsp_deinit(void) {
    mp_hal_pin_output(nina_wiring.gpio1);
    mp_hal_pin_write(nina_wiring.gpio1, 1);

    mp_hal_pin_output(nina_wiring.reset);
    mp_hal_pin_write(nina_wiring.reset, 0);
    mp_hal_delay_ms(100);

    mp_hal_pin_output(nina_wiring.gpio0);
    mp_hal_pin_write(nina_wiring.gpio0, 1);
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
    return mp_hal_pin_read(nina_wiring.gpio0);
}

int nina_bsp_spi_slave_select(uint32_t timeout) {
    // Wait for ACK to go low.
    for (mp_uint_t start = mp_hal_ticks_ms(); mp_hal_pin_read(nina_wiring.ack) == 1; mp_hal_delay_ms(1)) {
        if (timeout && ((mp_hal_ticks_ms() - start) >= timeout)) {
            return -1;
        }
    }

    // Chip select.
    mp_hal_pin_write(nina_wiring.gpio1, 0);

    // Wait for ACK to go high.
    for (mp_uint_t start = mp_hal_ticks_ms(); mp_hal_pin_read(nina_wiring.ack) == 0; mp_hal_delay_ms(1)) {
        if ((mp_hal_ticks_ms() - start) >= 100) {
            mp_hal_pin_write(nina_wiring.gpio1, 1);
            return -1;
        }
    }

    return 0;
}

int nina_bsp_spi_slave_deselect(void) {
    mp_hal_pin_write(nina_wiring.gpio1, 1);
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
