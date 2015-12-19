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

#include "py/mpstate.h"
#include "py/obj.h"
#include "py/gc.h"

#if MICROPY_PY_GC && MICROPY_ENABLE_GC

/// \module gc - control the garbage collector

/// \function collect()
/// Run a garbage collection.
STATIC mp_obj_t py_gc_collect(void) {
    gc_collect();
#if MICROPY_PY_GC_COLLECT_RETVAL
    return MP_OBJ_NEW_SMALL_INT(MP_STATE_MEM(gc_collected));
#else
    return mp_const_none;
#endif
}
MP_DEFINE_CONST_FUN_OBJ_0(gc_collect_obj, py_gc_collect);

/// \function disable()
/// Disable the garbage collector.
STATIC mp_obj_t gc_disable(void) {
    MP_STATE_MEM(gc_auto_collect_enabled) = 0;
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(gc_disable_obj, gc_disable);

/// \function enable()
/// Enable the garbage collector.
STATIC mp_obj_t gc_enable(void) {
    MP_STATE_MEM(gc_auto_collect_enabled) = 1;
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(gc_enable_obj, gc_enable);

STATIC mp_obj_t gc_isenabled(void) {
    return mp_obj_new_bool(MP_STATE_MEM(gc_auto_collect_enabled));
}
MP_DEFINE_CONST_FUN_OBJ_0(gc_isenabled_obj, gc_isenabled);

/// \function mem_free()
/// Return the number of bytes of available heap RAM.
STATIC mp_obj_t gc_mem_free(void) {
    gc_info_t info;
    gc_info(&info);
    return MP_OBJ_NEW_SMALL_INT(info.free);
}
MP_DEFINE_CONST_FUN_OBJ_0(gc_mem_free_obj, gc_mem_free);

/// \function mem_alloc()
/// Return the number of bytes of heap RAM that are allocated.
STATIC mp_obj_t gc_mem_alloc(void) {
    gc_info_t info;
    gc_info(&info);
    return MP_OBJ_NEW_SMALL_INT(info.used);
}
MP_DEFINE_CONST_FUN_OBJ_0(gc_mem_alloc_obj, gc_mem_alloc);

STATIC const mp_rom_map_elem_t mp_module_gc_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_gc) },
    { MP_ROM_QSTR(MP_QSTR_collect), MP_ROM_PTR(&gc_collect_obj) },
    { MP_ROM_QSTR(MP_QSTR_disable), MP_ROM_PTR(&gc_disable_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable), MP_ROM_PTR(&gc_enable_obj) },
    { MP_ROM_QSTR(MP_QSTR_isenabled), MP_ROM_PTR(&gc_isenabled_obj) },
    { MP_ROM_QSTR(MP_QSTR_mem_free), MP_ROM_PTR(&gc_mem_free_obj) },
    { MP_ROM_QSTR(MP_QSTR_mem_alloc), MP_ROM_PTR(&gc_mem_alloc_obj) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_gc_globals, mp_module_gc_globals_table);

const mp_obj_module_t mp_module_gc = {
    .base = { &mp_type_module },
    .name = MP_QSTR_gc,
    .globals = (mp_obj_dict_t*)&mp_module_gc_globals,
};

#endif
