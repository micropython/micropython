/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
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

#include "shared-bindings/microcontroller/Pin.h"
#include "supervisor/shared/translate.h"
#include "mpconfigboard.h"
#include "py/runtime.h"

#if CIRCUITPY_BUSIO
#include "shared-bindings/busio/I2C.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/busio/UART.h"
#endif

#if CIRCUITPY_DISPLAYIO
#include "shared-module/displayio/__init__.h"
#endif

#if CIRCUITPY_SHARPDISPLAY
#include "shared-bindings/sharpdisplay/SharpMemoryFramebuffer.h"
#include "shared-module/sharpdisplay/SharpMemoryFramebuffer.h"
#endif

#if BOARD_I2C
// Statically allocate the I2C object so it can live past the end of the heap and into the next VM.
// That way it can be used by built-in I2CDisplay displays and be accessible through board.I2C().
STATIC busio_i2c_obj_t i2c_obj;
STATIC mp_obj_t i2c_singleton = NULL;

mp_obj_t common_hal_board_get_i2c(void) {
    return i2c_singleton;
}

mp_obj_t common_hal_board_create_i2c(void) {
    // All callers have either already verified this or come so early that it can't be otherwise.
    assert(i2c_singleton == NULL || common_hal_busio_i2c_deinited(i2c_singleton));
    busio_i2c_obj_t *self = &i2c_obj;
    self->base.type = &busio_i2c_type;

    common_hal_busio_i2c_construct(self, DEFAULT_I2C_BUS_SCL, DEFAULT_I2C_BUS_SDA, 100000, 255);
    i2c_singleton = (mp_obj_t)self;
    return i2c_singleton;
}
#endif


#if BOARD_SPI
// Statically allocate the SPI object so it can live past the end of the heap and into the next VM.
// That way it can be used by built-in FourWire displays and be accessible through board.SPI().
STATIC busio_spi_obj_t spi_obj;
STATIC mp_obj_t spi_singleton = NULL;

mp_obj_t common_hal_board_get_spi(void) {
    return spi_singleton;
}

mp_obj_t common_hal_board_create_spi(void) {
    // All callers have either already verified this or come so early that it can't be otherwise.
    assert(spi_singleton == NULL || common_hal_busio_spi_deinited(spi_singleton));
    busio_spi_obj_t *self = &spi_obj;
    self->base.type = &busio_spi_type;

    const mcu_pin_obj_t *clock = MP_OBJ_TO_PTR(DEFAULT_SPI_BUS_SCK);
    const mcu_pin_obj_t *mosi = MP_OBJ_TO_PTR(DEFAULT_SPI_BUS_MOSI);
    const mcu_pin_obj_t *miso = MP_OBJ_TO_PTR(DEFAULT_SPI_BUS_MISO);
    common_hal_busio_spi_construct(self, clock, mosi, miso);
    spi_singleton = (mp_obj_t)self;
    return spi_singleton;
}
#endif

#if BOARD_UART
mp_obj_t common_hal_board_get_uart(void) {
    return MP_STATE_VM(shared_uart_bus);
}

mp_obj_t common_hal_board_create_uart(void) {
    busio_uart_obj_t *self = m_new_ll_obj(busio_uart_obj_t);
    self->base.type = &busio_uart_type;

    const mcu_pin_obj_t *rx = MP_OBJ_TO_PTR(DEFAULT_UART_BUS_RX);
    const mcu_pin_obj_t *tx = MP_OBJ_TO_PTR(DEFAULT_UART_BUS_TX);
    #ifdef DEFAULT_UART_BUS_RTS
    const mcu_pin_obj_t *rts = MP_OBJ_TO_PTR(DEFAULT_UART_BUS_RTS);
    #else
    const mcu_pin_obj_t *rts = NULL;
    #endif
    #ifdef DEFAULT_UART_BUS_CTS
    const mcu_pin_obj_t *cts = MP_OBJ_TO_PTR(DEFAULT_UART_BUS_CTS);
    #else
    const mcu_pin_obj_t *cts = NULL;
    #endif
    #ifdef DEFAULT_UART_IS_RS485
    const mcu_pin_obj_t *rs485_dir = MP_OBJ_TO_PTR(DEFAULT_UART_BUS_RS485DIR);
    #ifdef DEFAULT_UART_RS485_INVERT
    const bool rs485_invert = true;
    #else
    const bool rs485_invert = false;
    #endif
    #else
    const mcu_pin_obj_t *rs485_dir = NULL;
    const bool rs485_invert = false;
    #endif

    common_hal_busio_uart_construct(self, tx, rx, rts, cts, rs485_dir, rs485_invert,
        9600, 8, BUSIO_UART_PARITY_NONE, 1, 1.0f, 64, NULL, false);
    MP_STATE_VM(shared_uart_bus) = MP_OBJ_FROM_PTR(self);
    return MP_STATE_VM(shared_uart_bus);
}
#endif

void reset_board_busses(void) {
    #if BOARD_I2C
    bool display_using_i2c = false;
    #if CIRCUITPY_DISPLAYIO
    for (uint8_t i = 0; i < CIRCUITPY_DISPLAY_LIMIT; i++) {
        if (displays[i].bus_base.type == &displayio_i2cdisplay_type && displays[i].i2cdisplay_bus.bus == i2c_singleton) {
            display_using_i2c = true;
            break;
        }
    }
    #endif
    // make sure I2C lock is not held over a soft reset
    if (i2c_singleton != NULL) {
        common_hal_busio_i2c_unlock(i2c_singleton);
        if (!display_using_i2c) {
            common_hal_busio_i2c_deinit(i2c_singleton);
            i2c_singleton = NULL;
        }
    }
    #endif
    #if BOARD_SPI
    bool display_using_spi = false;
    #if CIRCUITPY_DISPLAYIO
    for (uint8_t i = 0; i < CIRCUITPY_DISPLAY_LIMIT; i++) {
        mp_const_obj_t bus_type = displays[i].bus_base.type;
        if (bus_type == &displayio_fourwire_type && displays[i].fourwire_bus.bus == spi_singleton) {
            display_using_spi = true;
            break;
        }
        #if CIRCUITPY_SHARPDISPLAY
        if (displays[i].bus_base.type == &sharpdisplay_framebuffer_type && displays[i].sharpdisplay.bus == spi_singleton) {
            display_using_spi = true;
            break;
        }
        #endif
    }
    #endif
    // make sure SPI lock is not held over a soft reset
    if (spi_singleton != NULL) {
        common_hal_busio_spi_unlock(spi_singleton);
        if (!display_using_spi) {
            common_hal_busio_spi_deinit(spi_singleton);
            spi_singleton = NULL;
        }
    }
    #endif
    #if BOARD_UART
    MP_STATE_VM(shared_uart_bus) = NULL;
    #endif
}
