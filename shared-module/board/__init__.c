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

#include "shared-bindings/busio/I2C.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/busio/UART.h"

#include "shared-bindings/microcontroller/Pin.h"
#include "supervisor/shared/translate.h"
#include "mpconfigboard.h"
#include "py/runtime.h"

#ifdef CIRCUITPY_DISPLAYIO
#include "shared-module/displayio/__init__.h"
#endif

#if BOARD_I2C
mp_obj_t common_hal_board_get_i2c(void) {
    return MP_STATE_VM(shared_i2c_bus);
}

mp_obj_t common_hal_board_create_i2c(void) {
    busio_i2c_obj_t *self = m_new_ll_obj(busio_i2c_obj_t);
    self->base.type = &busio_i2c_type;

    common_hal_busio_i2c_construct(self, DEFAULT_I2C_BUS_SCL, DEFAULT_I2C_BUS_SDA, 400000, 0);
    MP_STATE_VM(shared_i2c_bus) = MP_OBJ_FROM_PTR(self);
    return MP_STATE_VM(shared_i2c_bus);
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
    if (spi_singleton != NULL) {
        return spi_singleton;
    }
    busio_spi_obj_t *self = &spi_obj;
    self->base.type = &busio_spi_type;

    const mcu_pin_obj_t* clock = MP_OBJ_TO_PTR(DEFAULT_SPI_BUS_SCK);
    const mcu_pin_obj_t* mosi = MP_OBJ_TO_PTR(DEFAULT_SPI_BUS_MOSI);
    const mcu_pin_obj_t* miso = MP_OBJ_TO_PTR(DEFAULT_SPI_BUS_MISO);
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

    const mcu_pin_obj_t* rx = MP_OBJ_TO_PTR(DEFAULT_UART_BUS_RX);
    const mcu_pin_obj_t* tx = MP_OBJ_TO_PTR(DEFAULT_UART_BUS_TX);

    common_hal_busio_uart_construct(self, tx, rx, 9600, 8, PARITY_NONE, 1, 1000, 64);
    MP_STATE_VM(shared_uart_bus) = MP_OBJ_FROM_PTR(self);
    return MP_STATE_VM(shared_uart_bus);
}
#endif

void reset_board_busses(void) {
#if BOARD_I2C
    MP_STATE_VM(shared_i2c_bus) = NULL;
#endif
#if BOARD_SPI
    bool display_using_spi = false;
    #ifdef CIRCUITPY_DISPLAYIO
    for (uint8_t i = 0; i < CIRCUITPY_DISPLAY_LIMIT; i++) {
        if (displays[i].fourwire_bus.bus == spi_singleton) {
            display_using_spi = true;
            break;
        }
    }
    #endif
    if (!display_using_spi) {
        spi_singleton = NULL;
    }
#endif
#if BOARD_UART
    MP_STATE_VM(shared_uart_bus) = NULL;
#endif
}
