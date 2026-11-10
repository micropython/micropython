/*
 * This file is part of the MicroPython project, http://micropython.org/
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
#include "py/runtime.h"

#if MICROPY_PY_GC && MICROPY_ENABLE_GC

// collect(): run a garbage collection
static mp_obj_t py_gc_collect(void) {
    gc_collect();
    #if MICROPY_PY_GC_COLLECT_RETVAL
    return MP_OBJ_NEW_SMALL_INT(MP_STATE_MEM(gc_collected));
    #else
    return mp_const_none;
    #endif
}
MP_DEFINE_CONST_FUN_OBJ_0(gc_collect_obj, py_gc_collect);

// disable(): disable the garbage collector
static mp_obj_t gc_disable(void) {
    MP_STATE_MEM(gc_auto_collect_enabled) = 0;
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(gc_disable_obj, gc_disable);

// enable(): enable the garbage collector
static mp_obj_t gc_enable(void) {
    MP_STATE_MEM(gc_auto_collect_enabled) = 1;
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(gc_enable_obj, gc_enable);

static mp_obj_t gc_isenabled(void) {
    return mp_obj_new_bool(MP_STATE_MEM(gc_auto_collect_enabled));
}
MP_DEFINE_CONST_FUN_OBJ_0(gc_isenabled_obj, gc_isenabled);

// mem_free(): return the number of bytes of available heap RAM
static mp_obj_t gc_mem_free(void) {
    gc_info_t info;
    gc_info(&info);
    #if MICROPY_GC_SPLIT_HEAP_AUTO
    // Include max_new_split value here as a more useful heuristic
    return MP_OBJ_NEW_SMALL_INT(info.free + info.max_new_split);
    #else
    return MP_OBJ_NEW_SMALL_INT(info.free);
    #endif
}
MP_DEFINE_CONST_FUN_OBJ_0(gc_mem_free_obj, gc_mem_free);

// mem_alloc(): return the number of bytes of heap RAM that are allocated
static mp_obj_t gc_mem_alloc(void) {
    gc_info_t info;
    gc_info(&info);
    return MP_OBJ_NEW_SMALL_INT(info.used);
}
MP_DEFINE_CONST_FUN_OBJ_0(gc_mem_alloc_obj, gc_mem_alloc);

#if MICROPY_GC_ALLOC_THRESHOLD
static mp_obj_t gc_threshold(size_t n_args, const mp_obj_t *args) {
    if (n_args == 0) {
        if (MP_STATE_MEM(gc_alloc_threshold) == (size_t)-1) {
            return MP_OBJ_NEW_SMALL_INT(-1);
        }
        return mp_obj_new_int(MP_STATE_MEM(gc_alloc_threshold) * MICROPY_BYTES_PER_GC_BLOCK);
    }
    mp_int_t val = mp_obj_get_int(args[0]);
    if (val < 0) {
        MP_STATE_MEM(gc_alloc_threshold) = (size_t)-1;
    } else {
        MP_STATE_MEM(gc_alloc_threshold) = val / MICROPY_BYTES_PER_GC_BLOCK;
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(gc_threshold_obj, 0, 1, gc_threshold);
#endif

#if MICROPY_GC_AREA_SELECT
// areas(): return a list of (start, total, free, max_free, attrs) tuples, one per heap area
static mp_obj_t gc_areas(void) {
    mp_obj_t list = mp_obj_new_list(0, NULL);
    gc_area_info_t info;
    for (size_t i = 0; gc_area_info(i, &info); i++) {
        mp_obj_t items[5] = {
            mp_obj_new_int_from_uint((uintptr_t)info.start),
            mp_obj_new_int_from_uint(info.total),
            mp_obj_new_int_from_uint(info.free),
            mp_obj_new_int_from_uint(info.max_free),
            MP_OBJ_NEW_SMALL_INT(info.attrs),
        };
        mp_obj_list_append(list, mp_obj_new_tuple(5, items));
    }
    return list;
}
static MP_DEFINE_CONST_FUN_OBJ_0(gc_areas_obj, gc_areas);

// area_of(obj): return the index of the heap area holding obj's buffer or obj
static mp_obj_t gc_area_of(mp_obj_t obj) {
    const void *ptr = NULL;
    mp_buffer_info_t bufinfo;
    if (mp_get_buffer(obj, &bufinfo, MP_BUFFER_READ)) {
        ptr = bufinfo.buf;
    } else if (mp_obj_is_obj(obj)) {
        ptr = MP_OBJ_TO_PTR(obj);
    }
    int index = gc_area_index(ptr);
    return index < 0 ? mp_const_none : MP_OBJ_NEW_SMALL_INT(index);
}
static MP_DEFINE_CONST_FUN_OBJ_1(gc_area_of_obj, gc_area_of);

// Context manager returned by area().
typedef struct _mp_obj_gc_area_t {
    mp_obj_base_t base;
    uint32_t sel;
    uint32_t align;
    uint32_t prev_sel;
    uint32_t prev_align;
} mp_obj_gc_area_t;

static mp_obj_t gc_area_enter(mp_obj_t self_in) {
    mp_obj_gc_area_t *self = MP_OBJ_TO_PTR(self_in);
    self->prev_sel = gc_area_select(self->sel);
    self->prev_align = gc_alloc_align(self->align);
    return self_in;
}
static MP_DEFINE_CONST_FUN_OBJ_1(gc_area_enter_obj, gc_area_enter);

static mp_obj_t gc_area_exit(size_t n_args, const mp_obj_t *args) {
    mp_obj_gc_area_t *self = MP_OBJ_TO_PTR(args[0]);
    gc_area_select(self->prev_sel);
    gc_alloc_align(self->prev_align);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(gc_area_exit_obj, 4, 4, gc_area_exit);

static const mp_rom_map_elem_t gc_area_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&gc_area_enter_obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&gc_area_exit_obj) },
};
static MP_DEFINE_CONST_DICT(gc_area_locals_dict, gc_area_locals_dict_table);

static MP_DEFINE_CONST_OBJ_TYPE(
    gc_area_type,
    MP_QSTR_area,
    MP_TYPE_FLAG_NONE,
    locals_dict, &gc_area_locals_dict
    );

// area(*indices, attrs=0, strict=False, align=0): return a context manager that
// makes allocations go in the given heap areas, with the given alignment
static mp_obj_t gc_area(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_attrs, ARG_strict, ARG_align };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_attrs, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_strict, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_align, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(0, NULL, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    uint32_t sel = 0;
    for (size_t i = 0; i < n_args; i++) {
        mp_int_t index = mp_obj_get_int(pos_args[i]);
        gc_area_info_t info;
        if (index < 0 || index > GC_AREA_INDEX_MAX || !gc_area_info(index, &info)) {
            mp_raise_ValueError(MP_ERROR_TEXT("no such heap area"));
        }
        sel |= 1u << index;
    }
    if (args[ARG_attrs].u_int != 0) {
        uint32_t with_attrs = gc_area_with_attrs(args[ARG_attrs].u_int);
        sel = n_args == 0 ? with_attrs : (sel & with_attrs);
        if (sel == 0) {
            mp_raise_ValueError(MP_ERROR_TEXT("no such heap area"));
        }
    }
    if (sel != 0 && args[ARG_strict].u_bool) {
        sel |= GC_AREA_SEL_STRICT;
    }
    mp_int_t align = args[ARG_align].u_int;
    if (align < 0 || align > 0x40000000 || (align & (align - 1)) != 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("align must be a power of 2"));
    }

    mp_obj_gc_area_t *self = mp_obj_malloc(mp_obj_gc_area_t, &gc_area_type);
    self->sel = sel;
    self->align = align;
    self->prev_sel = 0;
    self->prev_align = 0;
    return MP_OBJ_FROM_PTR(self);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(gc_area_obj, 0, gc_area);
#endif

static const mp_rom_map_elem_t mp_module_gc_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_gc) },
    { MP_ROM_QSTR(MP_QSTR_collect), MP_ROM_PTR(&gc_collect_obj) },
    { MP_ROM_QSTR(MP_QSTR_disable), MP_ROM_PTR(&gc_disable_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable), MP_ROM_PTR(&gc_enable_obj) },
    { MP_ROM_QSTR(MP_QSTR_isenabled), MP_ROM_PTR(&gc_isenabled_obj) },
    { MP_ROM_QSTR(MP_QSTR_mem_free), MP_ROM_PTR(&gc_mem_free_obj) },
    { MP_ROM_QSTR(MP_QSTR_mem_alloc), MP_ROM_PTR(&gc_mem_alloc_obj) },
    #if MICROPY_GC_ALLOC_THRESHOLD
    { MP_ROM_QSTR(MP_QSTR_threshold), MP_ROM_PTR(&gc_threshold_obj) },
    #endif
    #if MICROPY_GC_AREA_SELECT
    { MP_ROM_QSTR(MP_QSTR_areas), MP_ROM_PTR(&gc_areas_obj) },
    { MP_ROM_QSTR(MP_QSTR_area), MP_ROM_PTR(&gc_area_obj) },
    { MP_ROM_QSTR(MP_QSTR_area_of), MP_ROM_PTR(&gc_area_of_obj) },
    { MP_ROM_QSTR(MP_QSTR_AREA_EXTERNAL), MP_ROM_INT(GC_AREA_ATTR_EXTERNAL) },
    { MP_ROM_QSTR(MP_QSTR_AREA_DMA), MP_ROM_INT(GC_AREA_ATTR_DMA) },
    #endif
};

static MP_DEFINE_CONST_DICT(mp_module_gc_globals, mp_module_gc_globals_table);

const mp_obj_module_t mp_module_gc = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_gc_globals,
};

MP_REGISTER_MODULE(MP_QSTR_gc, mp_module_gc);

#endif
