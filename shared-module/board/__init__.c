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

#include "shared-bindings/board/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-module/board/__init__.h"
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

#if CIRCUITPY_BOARD_I2C
// Statically allocate the I2C object so it can live past the end of the heap and into the next VM.
// That way it can be used by built-in I2CDisplay displays and be accessible through board.I2C().

typedef struct {
    const mcu_pin_obj_t *scl;
    const mcu_pin_obj_t *sda;
} board_i2c_pin_t;

static const board_i2c_pin_t i2c_pin[CIRCUITPY_BOARD_I2C] = CIRCUITPY_BOARD_I2C_PIN;
static busio_i2c_obj_t i2c_obj[CIRCUITPY_BOARD_I2C];
static bool i2c_obj_created[CIRCUITPY_BOARD_I2C];

bool common_hal_board_is_i2c(mp_obj_t obj) {
    for (uint8_t instance = 0; instance < CIRCUITPY_BOARD_I2C; instance++) {
        if (obj == &i2c_obj[instance]) {
            return true;
        }
    }
    return false;
}

mp_obj_t common_hal_board_get_i2c(const mp_int_t instance) {
    return i2c_obj_created[instance] ? &i2c_obj[instance] : NULL;
}

mp_obj_t common_hal_board_create_i2c(const mp_int_t instance) {
    const mp_obj_t singleton = common_hal_board_get_i2c(instance);
    if (singleton != NULL && !common_hal_busio_i2c_deinited(singleton)) {
        return singleton;
    }

    busio_i2c_obj_t *self = &i2c_obj[instance];
    self->base.type = &busio_i2c_type;

    assert_pin_free(i2c_pin[instance].scl);
    assert_pin_free(i2c_pin[instance].sda);

    common_hal_busio_i2c_construct(self, i2c_pin[instance].scl, i2c_pin[instance].sda, 100000, 255);

    i2c_obj_created[instance] = true;
    return &i2c_obj[instance];
}
#endif

#if CIRCUITPY_BOARD_SPI
// Statically allocate the SPI object so it can live past the end of the heap and into the next VM.
// That way it can be used by built-in FourWire displays and be accessible through board.SPI().

typedef struct {
    const mcu_pin_obj_t *clock;
    const mcu_pin_obj_t *mosi;
    const mcu_pin_obj_t *miso;
} board_spi_pin_t;

static const board_spi_pin_t spi_pin[CIRCUITPY_BOARD_SPI] = CIRCUITPY_BOARD_SPI_PIN;
static busio_spi_obj_t spi_obj[CIRCUITPY_BOARD_SPI];
static bool spi_obj_created[CIRCUITPY_BOARD_SPI];

bool common_hal_board_is_spi(mp_obj_t obj) {
    for (uint8_t instance = 0; instance < CIRCUITPY_BOARD_SPI; instance++) {
        if (obj == &spi_obj[instance]) {
            return true;
        }
    }
    return false;
}

mp_obj_t common_hal_board_get_spi(const mp_int_t instance) {
    return spi_obj_created[instance] ? &spi_obj[instance] : NULL;
}

mp_obj_t common_hal_board_create_spi(const mp_int_t instance) {
    const mp_obj_t singleton = common_hal_board_get_spi(instance);
    if (singleton != NULL && !common_hal_busio_spi_deinited(singleton)) {
        return singleton;
    }

    busio_spi_obj_t *self = &spi_obj[instance];
    self->base.type = &busio_spi_type;

    assert_pin_free(spi_pin[instance].clock);
    assert_pin_free(spi_pin[instance].mosi);
    assert_pin_free(spi_pin[instance].miso);

    common_hal_busio_spi_construct(self, spi_pin[instance].clock, spi_pin[instance].mosi, spi_pin[instance].miso);

    spi_obj_created[instance] = true;
    return &spi_obj[instance];
}
#endif

#if CIRCUITPY_BOARD_UART

typedef struct {
    const mcu_pin_obj_t *tx;
    const mcu_pin_obj_t *rx;
} board_uart_pin_t;

static const board_uart_pin_t uart_pin[CIRCUITPY_BOARD_UART] = CIRCUITPY_BOARD_UART_PIN;
static busio_uart_obj_t uart_obj[CIRCUITPY_BOARD_UART];
static bool uart_obj_created[CIRCUITPY_BOARD_UART];

bool common_hal_board_is_uart(mp_obj_t obj) {
    for (uint8_t instance = 0; instance < CIRCUITPY_BOARD_UART; instance++) {
        if (obj == &uart_obj[instance]) {
            return true;
        }
    }
    return false;
}

mp_obj_t common_hal_board_get_uart(const mp_int_t instance) {
    return uart_obj_created[instance] ? &uart_obj[instance] : NULL;
}

mp_obj_t common_hal_board_create_uart(const mp_int_t instance) {
    const mp_obj_t singleton = common_hal_board_get_uart(instance);
    if (singleton != NULL && !common_hal_busio_uart_deinited(singleton)) {
        return singleton;
    }

    busio_uart_obj_t *self = &uart_obj[instance];
    self->base.type = &busio_uart_type;

    MP_STATE_VM(board_uart_bus) = &uart_obj;

    assert_pin_free(uart_pin[instance].tx);
    assert_pin_free(uart_pin[instance].rx);

    common_hal_busio_uart_construct(self, uart_pin[instance].tx, uart_pin[instance].rx,
        NULL, NULL, NULL, false, 9600, 8, BUSIO_UART_PARITY_NONE, 1, 1.0f, 64, NULL, false);

    uart_obj_created[instance] = true;
    return &uart_obj[instance];
}
#endif

void reset_board_buses(void) {
    #if CIRCUITPY_BOARD_I2C
    for (uint8_t instance = 0; instance < CIRCUITPY_BOARD_I2C; instance++) {
        bool display_using_i2c = false;
        #if CIRCUITPY_DISPLAYIO
        for (uint8_t i = 0; i < CIRCUITPY_DISPLAY_LIMIT; i++) {
            if (displays[i].bus_base.type == &displayio_i2cdisplay_type && displays[i].i2cdisplay_bus.bus == &i2c_obj[instance]) {
                display_using_i2c = true;
                break;
            }
        }
        #endif
        if (i2c_obj_created[instance]) {
            // make sure I2C lock is not held over a soft reset
            common_hal_busio_i2c_unlock(&i2c_obj[instance]);
            if (!display_using_i2c) {
                common_hal_busio_i2c_deinit(&i2c_obj[instance]);
                i2c_obj_created[instance] = false;
            }
        }
    }
    #endif
    #if CIRCUITPY_BOARD_SPI
    for (uint8_t instance = 0; instance < CIRCUITPY_BOARD_SPI; instance++) {
        bool display_using_spi = false;
        #if CIRCUITPY_DISPLAYIO
        for (uint8_t i = 0; i < CIRCUITPY_DISPLAY_LIMIT; i++) {
            mp_const_obj_t bus_type = displays[i].bus_base.type;
            if (bus_type == &displayio_fourwire_type && displays[i].fourwire_bus.bus == &spi_obj[instance]) {
                display_using_spi = true;
                break;
            }
            #if CIRCUITPY_SHARPDISPLAY
            if (displays[i].bus_base.type == &sharpdisplay_framebuffer_type && displays[i].sharpdisplay.bus == &spi_obj[instance]) {
                display_using_spi = true;
                break;
            }
            #endif
        }
        #endif
        if (spi_obj_created[instance]) {
            // make sure SPI lock is not held over a soft reset
            common_hal_busio_spi_unlock(&spi_obj[instance]);
            if (!display_using_spi) {
                common_hal_busio_spi_deinit(&spi_obj[instance]);
                spi_obj_created[instance] = false;
            }
        }
    }
    #endif
    #if CIRCUITPY_BOARD_UART
    for (uint8_t instance = 0; instance < CIRCUITPY_BOARD_UART; instance++) {
        if (uart_obj_created[instance]) {
            common_hal_busio_uart_deinit(&uart_obj[instance]);
            uart_obj_created[instance] = false;
        }
    }
    #endif
}
