/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Damien P. George
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

#include <stdio.h>

#include "py/nlr.h"
#include "py/obj.h"
#include "py/gc.h"
#include "gccollect.h"
#include "pyexec.h"
#include MICROPY_HAL_H
#include "user_interface.h"
#include "modpyb.h"

STATIC mp_obj_t pyb_info(mp_uint_t n_args, const mp_obj_t *args) {
    // print info about memory
    {
        printf("_text_start=%p\n", &_text_start);
        printf("_text_end=%p\n", &_text_end);
        printf("_irom0_text_start=%p\n", &_irom0_text_start);
        printf("_irom0_text_end=%p\n", &_irom0_text_end);
        printf("_data_start=%p\n", &_data_start);
        printf("_data_end=%p\n", &_data_end);
        printf("_rodata_start=%p\n", &_rodata_start);
        printf("_rodata_end=%p\n", &_rodata_end);
        printf("_bss_start=%p\n", &_bss_start);
        printf("_bss_end=%p\n", &_bss_end);
        printf("_heap_start=%p\n", &_heap_start);
        printf("_heap_end=%p\n", &_heap_end);
    }

    // qstr info
    {
        mp_uint_t n_pool, n_qstr, n_str_data_bytes, n_total_bytes;
        qstr_pool_info(&n_pool, &n_qstr, &n_str_data_bytes, &n_total_bytes);
        printf("qstr:\n  n_pool=" UINT_FMT "\n  n_qstr=" UINT_FMT "\n  n_str_data_bytes=" UINT_FMT "\n  n_total_bytes=" UINT_FMT "\n", n_pool, n_qstr, n_str_data_bytes, n_total_bytes);
    }

    // GC info
    {
        gc_info_t info;
        gc_info(&info);
        printf("GC:\n");
        printf("  " UINT_FMT " total\n", info.total);
        printf("  " UINT_FMT " : " UINT_FMT "\n", info.used, info.free);
        printf("  1=" UINT_FMT " 2=" UINT_FMT " m=" UINT_FMT "\n", info.num_1block, info.num_2block, info.max_block);
    }

    if (n_args == 1) {
        // arg given means dump gc allocation table
        gc_dump_alloc_table();
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_info_obj, 0, 1, pyb_info);

STATIC mp_obj_t pyb_freq(mp_uint_t n_args, const mp_obj_t *args) {
    if (n_args == 0) {
        // get
        return mp_obj_new_int(mp_hal_get_cpu_freq());
    } else {
        // set
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "can't change freq"));
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_freq_obj, 0, 1, pyb_freq);

STATIC mp_obj_t pyb_sync(void) {
    //storage_flush();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(pyb_sync_obj, pyb_sync);

STATIC mp_obj_t pyb_millis(void) {
    return MP_OBJ_NEW_SMALL_INT(HAL_GetTick());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(pyb_millis_obj, pyb_millis);

STATIC mp_obj_t pyb_elapsed_millis(mp_obj_t start) {
    uint32_t startMillis = mp_obj_get_int(start);
    uint32_t currMillis = HAL_GetTick();
    return MP_OBJ_NEW_SMALL_INT((currMillis - startMillis) & 0x3fffffff);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_elapsed_millis_obj, pyb_elapsed_millis);

STATIC mp_obj_t pyb_micros(void) {
    return MP_OBJ_NEW_SMALL_INT(0);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(pyb_micros_obj, pyb_micros);

STATIC mp_obj_t pyb_elapsed_micros(mp_obj_t start) {
    uint32_t startMicros = mp_obj_get_int(start);
    uint32_t currMicros = 0;
    return MP_OBJ_NEW_SMALL_INT((currMicros - startMicros) & 0x3fffffff);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_elapsed_micros_obj, pyb_elapsed_micros);

STATIC mp_obj_t pyb_delay(mp_obj_t ms_in) {
    mp_int_t ms = mp_obj_get_int(ms_in);
    if (ms >= 0) {
        HAL_Delay(ms);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_delay_obj, pyb_delay);

STATIC mp_obj_t pyb_udelay(mp_obj_t usec_in) {
    mp_int_t usec = mp_obj_get_int(usec_in);
    if (usec >= 0) {
        mp_hal_udelay(usec);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_udelay_obj, pyb_udelay);

STATIC mp_obj_t pyb_hard_reset(void) {
    system_restart();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(pyb_hard_reset_obj, pyb_hard_reset);

STATIC const mp_map_elem_t pyb_module_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_pyb) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_info), (mp_obj_t)&pyb_info_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_freq), (mp_obj_t)&pyb_freq_obj },

    { MP_OBJ_NEW_QSTR(MP_QSTR_millis), (mp_obj_t)&pyb_millis_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_elapsed_millis), (mp_obj_t)&pyb_elapsed_millis_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_micros), (mp_obj_t)&pyb_micros_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_elapsed_micros), (mp_obj_t)&pyb_elapsed_micros_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_delay), (mp_obj_t)&pyb_delay_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_udelay), (mp_obj_t)&pyb_udelay_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_sync), (mp_obj_t)&pyb_sync_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_hard_reset), (mp_obj_t)&pyb_hard_reset_obj },

    { MP_OBJ_NEW_QSTR(MP_QSTR_Pin), (mp_obj_t)&pyb_pin_type },
};

STATIC MP_DEFINE_CONST_DICT(pyb_module_globals, pyb_module_globals_table);

const mp_obj_module_t pyb_module = {
    .base = { &mp_type_module },
    .name = MP_QSTR_pyb,
    .globals = (mp_obj_dict_t*)&pyb_module_globals,
};
