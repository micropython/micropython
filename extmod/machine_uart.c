/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Damien P. George
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

#include "py/runtime.h"
#include "py/stream.h"

#if MICROPY_PY_MACHINE_UART

#include "extmod/modmachine.h"
#include "shared/runtime/mpirq.h"

// The port must provide implementations of these low-level UART functions.

static void mp_machine_uart_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind);
static void mp_machine_uart_init_helper(machine_uart_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
static mp_obj_t mp_machine_uart_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args);
static void mp_machine_uart_deinit(machine_uart_obj_t *self);
static mp_int_t mp_machine_uart_any(machine_uart_obj_t *self);
static bool mp_machine_uart_txdone(machine_uart_obj_t *self);

#if MICROPY_PY_MACHINE_UART_SENDBREAK
static void mp_machine_uart_sendbreak(machine_uart_obj_t *self);
#endif

#if MICROPY_PY_MACHINE_UART_READCHAR_WRITECHAR
static mp_int_t mp_machine_uart_readchar(machine_uart_obj_t *self);
static void mp_machine_uart_writechar(machine_uart_obj_t *self, uint16_t data);
#endif

#if MICROPY_PY_MACHINE_UART_IRQ
static mp_irq_obj_t *mp_machine_uart_irq(machine_uart_obj_t *self, bool any_args, mp_arg_val_t *args);
#endif

static mp_uint_t mp_machine_uart_read(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode);
static mp_uint_t mp_machine_uart_write(mp_obj_t self_in, const void *buf_in, mp_uint_t size, int *errcode);
static mp_uint_t mp_machine_uart_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode);

// The port provides implementations of the above in this file.
#include MICROPY_PY_MACHINE_UART_INCLUDEFILE

// UART.init(...)
static mp_obj_t machine_uart_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    mp_machine_uart_init_helper(MP_OBJ_TO_PTR(args[0]), n_args - 1, args + 1, kw_args);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(machine_uart_init_obj, 1, machine_uart_init);

// UART.deinit()
static mp_obj_t machine_uart_deinit(mp_obj_t self_in) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_machine_uart_deinit(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_uart_deinit_obj, machine_uart_deinit);

// UART.any()
static mp_obj_t machine_uart_any(mp_obj_t self_in) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(mp_machine_uart_any(self));
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_uart_any_obj, machine_uart_any);

// UART.txdone()
static mp_obj_t machine_uart_txdone(mp_obj_t self_in) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(mp_machine_uart_txdone(self));
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_uart_txdone_obj, machine_uart_txdone);

#if MICROPY_PY_MACHINE_UART_SENDBREAK

// UART.sendbreak()
static mp_obj_t machine_uart_sendbreak(mp_obj_t self_in) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_machine_uart_sendbreak(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_uart_sendbreak_obj, machine_uart_sendbreak);

#endif

#if MICROPY_PY_MACHINE_UART_READCHAR_WRITECHAR

// UART.readchar()
static mp_obj_t machine_uart_readchar(mp_obj_t self_in) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(mp_machine_uart_readchar(self));
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_uart_readchar_obj, machine_uart_readchar);

// UART.writechar(char)
static mp_obj_t machine_uart_writechar(mp_obj_t self_in, mp_obj_t char_in) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_machine_uart_writechar(self, mp_obj_get_int(char_in));
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(machine_uart_writechar_obj, machine_uart_writechar);

#endif

#if MICROPY_PY_MACHINE_UART_IRQ

// UART.irq(handler, trigger, hard)
static mp_obj_t machine_uart_irq(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    mp_arg_val_t args[MP_IRQ_ARG_INIT_NUM_ARGS];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_IRQ_ARG_INIT_NUM_ARGS, mp_irq_init_args, args);
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    bool any_args = n_args > 1 || kw_args->used != 0;
    return MP_OBJ_FROM_PTR(mp_machine_uart_irq(self, any_args, args));
}
static MP_DEFINE_CONST_FUN_OBJ_KW(machine_uart_irq_obj, 1, machine_uart_irq);

#endif

static const mp_rom_map_elem_t machine_uart_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_uart_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&machine_uart_deinit_obj) },

    { MP_ROM_QSTR(MP_QSTR_flush), MP_ROM_PTR(&mp_stream_flush_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_stream_read1_obj) },
    { MP_ROM_QSTR(MP_QSTR_readline), MP_ROM_PTR(&mp_stream_unbuffered_readline_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&mp_stream_readinto1_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mp_stream_write1_obj) },

    { MP_ROM_QSTR(MP_QSTR_any), MP_ROM_PTR(&machine_uart_any_obj) },
    { MP_ROM_QSTR(MP_QSTR_txdone), MP_ROM_PTR(&machine_uart_txdone_obj) },

    #if MICROPY_PY_MACHINE_UART_SENDBREAK
    { MP_ROM_QSTR(MP_QSTR_sendbreak), MP_ROM_PTR(&machine_uart_sendbreak_obj) },
    #endif

    #if MICROPY_PY_MACHINE_UART_READCHAR_WRITECHAR
    { MP_ROM_QSTR(MP_QSTR_readchar), MP_ROM_PTR(&machine_uart_readchar_obj) },
    { MP_ROM_QSTR(MP_QSTR_writechar), MP_ROM_PTR(&machine_uart_writechar_obj) },
    #endif

    #if MICROPY_PY_MACHINE_UART_IRQ
    { MP_ROM_QSTR(MP_QSTR_irq), MP_ROM_PTR(&machine_uart_irq_obj) },
    #endif

    // A port must add UART class constants defining the following macro.
    // It can be defined to nothing if there are no constants.
    MICROPY_PY_MACHINE_UART_CLASS_CONSTANTS
};
static MP_DEFINE_CONST_DICT(machine_uart_locals_dict, machine_uart_locals_dict_table);

static const mp_stream_p_t uart_stream_p = {
    .read = mp_machine_uart_read,
    .write = mp_machine_uart_write,
    .ioctl = mp_machine_uart_ioctl,
    .is_text = false,
};

MP_DEFINE_CONST_OBJ_TYPE(
    machine_uart_type,
    MP_QSTR_UART,
    MP_TYPE_FLAG_ITER_IS_STREAM,
    make_new, mp_machine_uart_make_new,
    print, mp_machine_uart_print,
    protocol, &uart_stream_p,
    locals_dict, &machine_uart_locals_dict
    );

#endif // MICROPY_PY_MACHINE_UART
