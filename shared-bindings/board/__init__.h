// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "py/objstr.h"

#include "shared-bindings/microcontroller/Pin.h"  // for the pin definitions

extern const mp_obj_dict_t board_module_globals;
static const MP_DEFINE_STR_OBJ(board_module_id_obj, CIRCUITPY_BOARD_ID);

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
    static mp_obj_t board_##name(void) { \
        return common_hal_board_create_##bus(instance); \
    } \
    MP_DEFINE_CONST_FUN_OBJ_0(board_##name##_obj, board_##name);

#define CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS \
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_board) }, \
    { MP_ROM_QSTR(MP_QSTR_board_id), MP_ROM_PTR(&board_module_id_obj) },
