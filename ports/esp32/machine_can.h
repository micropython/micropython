/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Musumeci Salvatore
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
#ifndef MICROPY_INCLUDED_ESP32_CAN_H
#define MICROPY_INCLUDED_ESP32_CAN_H

#include "modmachine.h"

#include "py/obj.h"

#define DEVICE_NAME "CAN"

#define CAN_BAUDRATE_25k 25
#define CAN_BAUDRATE_50k 50
#define CAN_BAUDRATE_100k 100
#define CAN_BAUDRATE_125k 125
#define CAN_BAUDRATE_250k 250
#define CAN_BAUDRATE_500k 500
#define CAN_BAUDRATE_800k 800
#define CAN_BAUDRATE_1M 1000


typedef struct _machine_can_config_t {
    const can_timing_config_t *timing;
    const can_filter_config_t *filter;
    const can_general_config_t *general;
    uint16_t baudrate;
    bool initialized;
} machine_can_config_t;

typedef struct _machine_can_obj_t {
    mp_obj_base_t base;
    machine_can_config_t *config;
} machine_can_obj_t;

//Functions signature definition
mp_obj_t machine_hw_can_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args);
STATIC mp_obj_t machine_hw_can_init_helper(const machine_can_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
STATIC mp_obj_t machine_hw_can_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args);
STATIC mp_obj_t machine_hw_can_deinit(const mp_obj_t self_in);
STATIC void machine_hw_can_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);

STATIC mp_obj_t machine_hw_can_recv(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
STATIC mp_obj_t machine_hw_can_state(mp_obj_t self_in);
STATIC mp_obj_t machine_hw_can_info(size_t n_args, const mp_obj_t *args);
STATIC mp_obj_t machine_hw_can_any(mp_obj_t self_in, mp_obj_t fifo_in);
STATIC mp_obj_t machine_hw_can_send(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
STATIC mp_obj_t machine_hw_can_clear_tx_queue(mp_obj_t self_in);
STATIC mp_obj_t machine_hw_can_clear_rx_queue(mp_obj_t self_in);

//Python function declarations
MP_DEFINE_CONST_FUN_OBJ_1(machine_hw_can_deinit_obj, machine_hw_can_deinit);
MP_DEFINE_CONST_FUN_OBJ_KW(machine_hw_can_init_obj, 4, machine_hw_can_init);
MP_DEFINE_CONST_FUN_OBJ_1(machine_hw_can_state_obj, machine_hw_can_state);
MP_DEFINE_CONST_FUN_OBJ_KW(machine_hw_can_send_obj, 3, machine_hw_can_send);
MP_DEFINE_CONST_FUN_OBJ_KW(machine_hw_can_recv_obj, 0, machine_hw_can_recv);
MP_DEFINE_CONST_FUN_OBJ_1(machine_hw_can_clear_tx_queue_obj, machine_hw_can_clear_tx_queue);
MP_DEFINE_CONST_FUN_OBJ_1(machine_hw_can_clear_rx_queue_obj, machine_hw_can_clear_rx_queue);
MP_DEFINE_CONST_FUN_OBJ_2(machine_hw_can_any_obj, machine_hw_can_any);
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_hw_can_info_obj, 1, 2, machine_hw_can_info);

//Python objects list declaration
STATIC const mp_rom_map_elem_t machine_can_locals_dict_table[] = {
    //CAN_ATTRIBUTES
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_CAN) },
    //CAN_FUNCTIONS
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_hw_can_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&machine_hw_can_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_state), MP_ROM_PTR(&machine_hw_can_state_obj) },
    { MP_ROM_QSTR(MP_QSTR_info), MP_ROM_PTR(&machine_hw_can_info_obj) },
    { MP_ROM_QSTR(MP_QSTR_any), MP_ROM_PTR(&machine_hw_can_any_obj) },
    { MP_ROM_QSTR(MP_QSTR_send), MP_ROM_PTR(&machine_hw_can_send_obj)},
    { MP_ROM_QSTR(MP_QSTR_recv), MP_ROM_PTR(&machine_hw_can_recv_obj)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_clear_tx_queue), MP_ROM_PTR(&machine_hw_can_clear_tx_queue_obj)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_clear_tx_queue), MP_ROM_PTR(&machine_hw_can_clear_rx_queue_obj)},
    //CAN_MODE
    { MP_ROM_QSTR(MP_QSTR_MODE_NORMAL), MP_ROM_INT(CAN_MODE_NORMAL) },
    { MP_ROM_QSTR(MP_QSTR_MODE_NO_ACK), MP_ROM_INT(CAN_MODE_NO_ACK) },
    { MP_ROM_QSTR(MP_QSTR_MODE_LISTEN_ONLY), MP_ROM_INT(CAN_MODE_LISTEN_ONLY) },
    //CAN_STATE
    { MP_ROM_QSTR(MP_QSTR_STATE_STOPPED), MP_ROM_INT(CAN_STATE_STOPPED) },
    { MP_ROM_QSTR(MP_QSTR_STATE_RUNNING), MP_ROM_INT(CAN_STATE_RUNNING) },
    { MP_ROM_QSTR(MP_QSTR_STATE_BUS_OFF), MP_ROM_INT(CAN_STATE_BUS_OFF) },
    { MP_ROM_QSTR(MP_QSTR_STATE_RECOVERING), MP_ROM_INT(CAN_STATE_RECOVERING) },
    //CAN_BAUDRATE
    { MP_ROM_QSTR(MP_QSTR_BAUDRATE_25k), MP_ROM_INT(CAN_BAUDRATE_25k) },
    { MP_ROM_QSTR(MP_QSTR_BAUDRATE_50k), MP_ROM_INT(CAN_BAUDRATE_50k) },
    { MP_ROM_QSTR(MP_QSTR_BAUDRATE_100k), MP_ROM_INT(CAN_BAUDRATE_100k) },
    { MP_ROM_QSTR(MP_QSTR_BAUDRATE_125k), MP_ROM_INT(CAN_BAUDRATE_125k) },
    { MP_ROM_QSTR(MP_QSTR_BAUDRATE_250k), MP_ROM_INT(CAN_BAUDRATE_250k) },
    { MP_ROM_QSTR(MP_QSTR_BAUDRATE_500k), MP_ROM_INT(CAN_BAUDRATE_500k) },
    { MP_ROM_QSTR(MP_QSTR_BAUDRATE_800k), MP_ROM_INT(CAN_BAUDRATE_800k) },
    { MP_ROM_QSTR(MP_QSTR_BAUDRATE_1M),   MP_ROM_INT(CAN_BAUDRATE_1M) },
};
STATIC MP_DEFINE_CONST_DICT(machine_can_locals_dict, machine_can_locals_dict_table);

//Python object definition
const mp_obj_type_t machine_can_type = {
    { &mp_type_type },
    .name = MP_QSTR_CAN,
    .print = machine_hw_can_print, // give it a print-function
    .make_new = machine_hw_can_make_new,  // give it a constructor
    .locals_dict = (mp_obj_dict_t*)&machine_can_locals_dict, // and the global members
};

#endif // MICROPY_INCLUDED_ESP32_CAN_H
