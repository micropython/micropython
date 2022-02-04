/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft
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

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/board/__init__.h"
#if CIRCUITPY_BOARD_I2C
#include "shared-bindings/busio/I2C.h"
#endif
#if CIRCUITPY_BOARD_SPI
#include "shared-bindings/busio/SPI.h"
#endif
#if CIRCUITPY_BOARD_UART
#include "shared-bindings/busio/UART.h"
#endif

//| """Board specific pin names
//|
//| Common container for board base pin names. These will vary from board to
//| board so don't expect portability when using this module.
//|
//| .. warning:: The board module varies by board. The APIs documented here may or may not be
//|              available on a specific board."""

//| board_id: str
//| """Board ID string. The unique identifier for the board model in
//| circuitpython, as well as on circuitpython.org.
//| Example: "hallowing_m0_express"."""

//| def I2C() -> busio.I2C:
//|     """Returns the `busio.I2C` object for the board's designated I2C bus(es).
//|     The object created is a singleton, and uses the default parameter values for `busio.I2C`."""
//|     ...
//|
#if CIRCUITPY_BOARD_I2C
STATIC mp_obj_t board_i2c_0(void) {
    return common_hal_board_create_i2c(0);
}
#else
STATIC mp_obj_t board_i2c_0(void) {
    mp_raise_NotImplementedError_varg(translate("No default %q bus"), MP_QSTR_I2C);
    return MP_ROM_NONE;
}
#endif
MP_DEFINE_CONST_FUN_OBJ_0(board_i2c_obj, board_i2c_0);

//| def SPI() -> busio.SPI:
//|     """Returns the `busio.SPI` object for the board's designated SPI bus(es).
//|     The object created is a singleton, and uses the default parameter values for `busio.SPI`."""
//|     ...
//|
#if CIRCUITPY_BOARD_SPI
STATIC mp_obj_t board_spi_0(void) {
    return common_hal_board_create_spi(0);
}
#else
STATIC mp_obj_t board_spi_0(void) {
    mp_raise_NotImplementedError_varg(translate("No default %q bus"), MP_QSTR_SPI);
    return MP_ROM_NONE;
}
#endif
MP_DEFINE_CONST_FUN_OBJ_0(board_spi_obj, board_spi_0);

//| def UART() -> busio.UART:
//|     """Returns the `busio.UART` object for the board's designated UART bus(es).
//|     The object created is a singleton, and uses the default parameter values for `busio.UART`."""
//|     ...
//|
#if CIRCUITPY_BOARD_UART
STATIC mp_obj_t board_uart_0(void) {
    return common_hal_board_create_uart(0);
}
#else
STATIC mp_obj_t board_uart_0(void) {
    mp_raise_NotImplementedError_varg(translate("No default %q bus"), MP_QSTR_UART);
    return MP_ROM_NONE;
}
#endif
MP_DEFINE_CONST_FUN_OBJ_0(board_uart_obj, board_uart_0);

const mp_obj_module_t board_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&board_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_board, board_module, CIRCUITPY_BOARD);
