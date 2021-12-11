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

#if CIRCUITPY_BOARD_I2C || CIRCUITPY_BOARD_SPI || CIRCUITPY_BOARD_UART
STATIC mp_int_t board_get_instance(size_t n_args, const mp_obj_t *args, const mp_int_t bus_in) {
    if (n_args == 0) {
        return 0;
    }
    const mp_int_t instance = mp_obj_get_int(args[0]);
    if (instance >= bus_in || instance < 0) {
        mp_raise_ValueError_varg(translate("No default %q bus"), MP_QSTR_UART);
    }
    return instance;
}
#endif

//| def I2C(instance: Optional[int] = 0) -> busio.I2C:
//|     """Returns the `busio.I2C` object for the board's designated I2C bus(es). It is a singleton.
//|     The object created uses the default parameter values for `busio.I2C`."""
//|     ...
//|
#if CIRCUITPY_BOARD_I2C
mp_obj_t board_i2c(size_t n_args, const mp_obj_t *args) {
    const mp_int_t instance = board_get_instance(n_args, args, CIRCUITPY_BOARD_I2C);
    const mp_obj_t singleton = common_hal_board_get_i2c(instance);
    if (singleton != NULL && !common_hal_busio_i2c_deinited(singleton)) {
        return singleton;
    }
    return common_hal_board_create_i2c(instance);
}
#else
mp_obj_t board_i2c(size_t n_args, const mp_obj_t *args) {
    mp_raise_NotImplementedError_varg(translate("No default %q bus"), MP_QSTR_I2C);
    return MP_ROM_NONE;
}
#endif
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(board_i2c_obj, 0, 1, board_i2c);

//| def SPI(instance: Optional[int] = 0) -> busio.SPI:
//|     """Returns the `busio.SPI` object for the board's designated SPI bus(es). It is a singleton.
//|     The object created uses the default parameter values for `busio.SPI`."""
//|     ...
//|
#if CIRCUITPY_BOARD_SPI
mp_obj_t board_spi(size_t n_args, const mp_obj_t *args) {
    const mp_int_t instance = board_get_instance(n_args, args, CIRCUITPY_BOARD_SPI);
    const mp_obj_t singleton = common_hal_board_get_spi(instance);
    if (singleton != NULL && !common_hal_busio_spi_deinited(singleton)) {
        return singleton;
    }
    return common_hal_board_create_spi(instance);
}
#else
mp_obj_t board_spi(size_t n_args, const mp_obj_t *args) {
    mp_raise_NotImplementedError_varg(translate("No default %q bus"), MP_QSTR_SPI);
    return MP_ROM_NONE;
}
#endif
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(board_spi_obj, 0, 1, board_spi);

//| def UART(instance: Optional[int] = 0) -> busio.UART:
//|     """Returns the `busio.UART` object for the board's designated UART bus(es). It is a singleton.
//|     The object created uses the default parameter values for `busio.UART`."""
//|     ...
//|
#if CIRCUITPY_BOARD_UART
mp_obj_t board_uart(size_t n_args, const mp_obj_t *args) {
    const mp_int_t instance = board_get_instance(n_args, args, CIRCUITPY_BOARD_UART);
    const mp_obj_t singleton = common_hal_board_get_uart(instance);
    if (singleton != NULL && !common_hal_busio_uart_deinited(singleton)) {
        return singleton;
    }
    return common_hal_board_create_uart(instance);
}
#else
mp_obj_t board_uart(size_t n_args, const mp_obj_t *args) {
    mp_raise_NotImplementedError_varg(translate("No default %q bus"), MP_QSTR_UART);
    return MP_ROM_NONE;
}
#endif
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(board_uart_obj, 0, 1, board_uart);

const mp_obj_module_t board_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&board_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_board, board_module, CIRCUITPY_BOARD);
