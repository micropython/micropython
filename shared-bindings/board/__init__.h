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

#include "shared-bindings/microcontroller/Pin.h"  // for the pin definitions

extern const mp_obj_dict_t board_module_globals;

mp_obj_t common_hal_board_get_i2c(void);
mp_obj_t common_hal_board_create_i2c(void);
MP_DECLARE_CONST_FUN_OBJ_0(board_i2c_obj);

mp_obj_t common_hal_board_get_spi(void);
mp_obj_t common_hal_board_create_spi(void);
MP_DECLARE_CONST_FUN_OBJ_0(board_spi_obj);

mp_obj_t common_hal_board_get_uart(void);
mp_obj_t common_hal_board_create_uart(void);
MP_DECLARE_CONST_FUN_OBJ_0(board_uart_obj);

#endif  // MICROPY_INCLUDED_SHARED_BINDINGS_BOARD___INIT___H
