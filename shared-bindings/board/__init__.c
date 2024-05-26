// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

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
//| Another common use of this module is to use serial communication buses with
//| the default pins and settings.  For more information about serial communcication
//| in CircuitPython, see the :mod:`busio`.
//|
//| For more information regarding the typical usage of :py:mod:`board`, refer to the `CircuitPython
//| Essentials Learn guide
//| <https://learn.adafruit.com/circuitpython-essentials/circuitpython-pins-and-modules>`_
//|
//| .. warning:: The board module varies by board. The APIs documented here may or may not be
//|              available on a specific board."""

//| board_id: str
//| """Board ID string. The unique identifier for the board model in
//| circuitpython, as well as on circuitpython.org.
//| Example: "hallowing_m0_express"."""
//|

//| def I2C() -> busio.I2C:
//|     """Returns the `busio.I2C` object for the board's designated I2C bus(es).
//|     The object created is a singleton, and uses the default parameter values for `busio.I2C`.
//|     """
//|     ...
//|
#if CIRCUITPY_BOARD_I2C
static mp_obj_t board_i2c_0(void) {
    return common_hal_board_create_i2c(0);
}
#else
static mp_obj_t board_i2c_0(void) {
    mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("No default %q bus"), MP_QSTR_I2C);
    return MP_ROM_NONE;
}
#endif
MP_DEFINE_CONST_FUN_OBJ_0(board_i2c_obj, board_i2c_0);

//| def SPI() -> busio.SPI:
//|     """Returns the `busio.SPI` object for the board's designated SPI bus(es).
//|     The object created is a singleton, and uses the default parameter values for `busio.SPI`.
//|     """
//|     ...
//|
#if CIRCUITPY_BOARD_SPI
static mp_obj_t board_spi_0(void) {
    return common_hal_board_create_spi(0);
}
#else
static mp_obj_t board_spi_0(void) {
    mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("No default %q bus"), MP_QSTR_SPI);
    return MP_ROM_NONE;
}
#endif
MP_DEFINE_CONST_FUN_OBJ_0(board_spi_obj, board_spi_0);

//| def UART() -> busio.UART:
//|     """Returns the `busio.UART` object for the board's designated UART bus(es).
//|     The object created is a singleton, and uses the default parameter values for `busio.UART`.
//|     """
//|     ...
//|
#if CIRCUITPY_BOARD_UART
static mp_obj_t board_uart_0(void) {
    return common_hal_board_create_uart(0);
}
#else
static mp_obj_t board_uart_0(void) {
    mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("No default %q bus"), MP_QSTR_UART);
    return MP_ROM_NONE;
}
#endif
MP_DEFINE_CONST_FUN_OBJ_0(board_uart_obj, board_uart_0);

const mp_obj_module_t board_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&board_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_board, board_module);
