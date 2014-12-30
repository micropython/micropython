/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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

#include <stdint.h>
#include <stdio.h>
#include "mpconfig.h"
#include "misc.h"
#include "qstr.h"
#include "obj.h"
#include "builtin.h"
#include "stackctrl.h"
#include "gc.h"

// Various builtins specific to MicroPython runtime,
// living in micropython module

#if MICROPY_MEM_STATS
STATIC mp_obj_t mp_micropython_mem_total() {
    return MP_OBJ_NEW_SMALL_INT(m_get_total_bytes_allocated());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mp_micropython_mem_total_obj, mp_micropython_mem_total);

STATIC mp_obj_t mp_micropython_mem_current() {
    return MP_OBJ_NEW_SMALL_INT(m_get_current_bytes_allocated());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mp_micropython_mem_current_obj, mp_micropython_mem_current);

STATIC mp_obj_t mp_micropython_mem_peak() {
    return MP_OBJ_NEW_SMALL_INT(m_get_peak_bytes_allocated());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mp_micropython_mem_peak_obj, mp_micropython_mem_peak);

mp_obj_t mp_micropython_mem_info(mp_uint_t n_args, const mp_obj_t *args) {
    printf("mem: total=" UINT_FMT ", current=" UINT_FMT ", peak=" UINT_FMT "\n",
        m_get_total_bytes_allocated(), m_get_current_bytes_allocated(), m_get_peak_bytes_allocated());
    printf("stack: " UINT_FMT "\n", mp_stack_usage());
#if MICROPY_ENABLE_GC
    gc_dump_info();
    if (n_args == 1) {
        // arg given means dump gc allocation table
        gc_dump_alloc_table();
    }
#endif
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_micropython_mem_info_obj, 0, 1, mp_micropython_mem_info);

STATIC mp_obj_t qstr_info(void) {
    mp_uint_t n_pool, n_qstr, n_str_data_bytes, n_total_bytes;
    qstr_pool_info(&n_pool, &n_qstr, &n_str_data_bytes, &n_total_bytes);
    printf("qstr pool: n_pool=" UINT_FMT ", n_qstr=" UINT_FMT ", n_str_data_bytes=" UINT_FMT ", n_total_bytes=" UINT_FMT "\n",
        n_pool, n_qstr, n_str_data_bytes, n_total_bytes);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mp_micropython_qstr_info_obj, qstr_info);
#endif

#if MICROPY_ENABLE_EMERGENCY_EXCEPTION_BUF && (MICROPY_EMERGENCY_EXCEPTION_BUF_SIZE == 0)
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_alloc_emergency_exception_buf_obj, mp_alloc_emergency_exception_buf);
#endif

STATIC const mp_map_elem_t mp_module_micropython_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_micropython) },
#if MICROPY_MEM_STATS
    { MP_OBJ_NEW_QSTR(MP_QSTR_mem_total), (mp_obj_t)&mp_micropython_mem_total_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_mem_current), (mp_obj_t)&mp_micropython_mem_current_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_mem_peak), (mp_obj_t)&mp_micropython_mem_peak_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_mem_info), (mp_obj_t)&mp_micropython_mem_info_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_qstr_info), (mp_obj_t)&mp_micropython_qstr_info_obj },
#endif
#if MICROPY_ENABLE_EMERGENCY_EXCEPTION_BUF && (MICROPY_EMERGENCY_EXCEPTION_BUF_SIZE == 0)
    { MP_OBJ_NEW_QSTR(MP_QSTR_alloc_emergency_exception_buf), (mp_obj_t)&mp_alloc_emergency_exception_buf_obj },
#endif
};

STATIC MP_DEFINE_CONST_DICT(mp_module_micropython_globals, mp_module_micropython_globals_table);

const mp_obj_module_t mp_module_micropython = {
    .base = { &mp_type_module },
    .name = MP_QSTR_micropython,
    .globals = (mp_obj_dict_t*)&mp_module_micropython_globals,
};
