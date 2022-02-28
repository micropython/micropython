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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_BOARD___INIT___H
#define MICROPY_INCLUDED_SHARED_BINDINGS_BOARD___INIT___H

#include "py/obj.h"
#include "py/objstr.h"

#include "shared-bindings/microcontroller/Pin.h"  // for the pin definitions

extern const mp_obj_dict_t board_module_globals;
STATIC const MP_DEFINE_STR_OBJ(board_module_id_obj, CIRCUITPY_BOARD_ID);

bool common_hal_board_is_i2c(mp_obj_t obj);
mp_obj_t common_hal_board_get_i2c(const mp_int_t instance);
mp_obj_t common_hal_board_create_i2c(const mp_int_t instance);
mp_obj_t board_i2c(size_t n_args, const mp_obj_t *args);
MP_DECLARE_CONST_FUN_OBJ_0(board_i2c_obj);

bool common_hal_board_is_spi(mp_obj_t obj);
mp_obj_t common_hal_board_get_spi(const mp_int_t instance);
mp_obj_t common_hal_board_create_spi(const mp_int_t instance);
mp_obj_t board_spi(size_t n_args, const mp_obj_t *args);
MP_DECLARE_CONST_FUN_OBJ_0(board_spi_obj);

bool common_hal_board_is_uart(mp_obj_t obj);
mp_obj_t common_hal_board_get_uart(const mp_int_t instance);
mp_obj_t common_hal_board_create_uart(const mp_int_t instance);
mp_obj_t board_uart(size_t n_args, const mp_obj_t *args);
MP_DECLARE_CONST_FUN_OBJ_0(board_uart_obj);

#define CIRCUITPY_BOARD_BUS_SINGLETON(name, bus, instance) \
    STATIC mp_obj_t board_##name(void) { \
        return common_hal_board_create_##bus(instance); \
    } \
    MP_DEFINE_CONST_FUN_OBJ_0(board_##name##_obj, board_##name);

#define CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS \
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_board) }, \
    { MP_ROM_QSTR(MP_QSTR_board_id), MP_ROM_PTR(&board_module_id_obj) },

#endif  // MICROPY_INCLUDED_SHARED_BINDINGS_BOARD___INIT___H
