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

#include <stdint.h>

#include "py/bc.h"
#include "py/binary.h"
#include "py/gc.h"
#include "py/obj.h"
#include "py/objarray.h"
#include "py/objfun.h"
#include "py/objint.h"
#include "py/objstr.h"
#include "py/objtype.h"
#include "py/runtime.h"

#include "shared-bindings/uheap/__init__.h"

#define VERIFY_PTR(ptr) ( \
    (void *)ptr >= (void *)MP_STATE_MEM(gc_pool_start)         /* must be above start of pool */ \
    && (void *)ptr < (void *)MP_STATE_MEM(gc_pool_end)            /* must be below end of pool */ \
    )

static void indent(uint8_t levels) {
    for (int i = 0; i < levels; i++) {
        mp_printf(&mp_plat_print, "  ");
    }
}

static uint32_t object_size(uint8_t indent_level, mp_obj_t obj);

static uint32_t int_size(uint8_t indent_level, mp_obj_t obj) {
    if (mp_obj_is_small_int(obj)) {
        return 0;
    }
    if (!VERIFY_PTR(obj)) {
        return 0;
    }
    #if MICROPY_LONGINT_IMPL == MICROPY_LONGINT_IMPL_MPZ
    mp_obj_int_t *i = MP_OBJ_TO_PTR(obj);
    return gc_nbytes(obj) + gc_nbytes(i->mpz.dig);
    #else
    return gc_nbytes(obj);
    #endif
}

static uint32_t string_size(uint8_t indent_level, mp_obj_t obj) {
    if (mp_obj_is_qstr(obj)) {
        qstr qs = MP_OBJ_QSTR_VALUE(obj);
        const char *s = qstr_str(qs);
        if (!VERIFY_PTR(s)) {
            return 0;
        }
        indent(indent_level);
        mp_printf(&mp_plat_print, "%s\n", s);
        return 0;
    } else { // mp_obj_is_type(o, &mp_type_str)
        mp_obj_str_t *s = MP_OBJ_TO_PTR(obj);
        return gc_nbytes(s) + gc_nbytes(s->data);
    }
}

static uint32_t map_size(uint8_t indent_level, const mp_map_t *map) {
    uint32_t total_size = gc_nbytes(map->table);
    for (int i = 0; i < map->used; i++) {
        uint32_t this_size = 0;
        indent(indent_level);
        if (map->table[i].key != NULL) {
            mp_print_str(&mp_plat_print, "key: ");
            mp_obj_print_helper(&mp_plat_print, map->table[i].key, PRINT_STR);
            mp_print_str(&mp_plat_print, "\n");
        } else {
            mp_print_str(&mp_plat_print, "null key\n");
        }
        this_size += object_size(indent_level + 1, map->table[i].key);
        this_size += object_size(indent_level + 1, map->table[i].value);

        indent(indent_level);
        mp_printf(&mp_plat_print, "Entry size: %u\n\n", this_size);
        total_size += this_size;
    }

    return total_size;
}

static uint32_t dict_size(uint8_t indent_level, mp_obj_dict_t *dict) {
    uint32_t total_size = gc_nbytes(dict);

    indent(indent_level);
    mp_printf(&mp_plat_print, "Dictionary @%x\n", dict);

    total_size += map_size(indent_level, &dict->map);

    return total_size;
}

static uint32_t function_size(uint8_t indent_level, mp_obj_t obj) {
    // indent(indent_level);
    // mp_print_str(&mp_plat_print, "function\n");
    if (mp_obj_is_type(obj, &mp_type_fun_builtin_0)) {
        return 0;
    } else if (mp_obj_is_type(obj, &mp_type_fun_builtin_1)) {
        return 0;
    } else if (mp_obj_is_type(obj, &mp_type_fun_builtin_2)) {
        return 0;
    } else if (mp_obj_is_type(obj, &mp_type_fun_builtin_3)) {
        return 0;
    } else if (mp_obj_is_type(obj, &mp_type_fun_builtin_var)) {
        return 0;
    } else if (mp_obj_is_type(obj, &mp_type_fun_bc)) {
        mp_obj_fun_bc_t *fn = MP_OBJ_TO_PTR(obj);
        uint32_t total_size = gc_nbytes(fn) + gc_nbytes(fn->bytecode) + gc_nbytes(fn->const_table);
        #if MICROPY_DEBUG_PRINTERS
        mp_printf(&mp_plat_print, "BYTECODE START\n");
        mp_bytecode_print(fn, fn->bytecode, gc_nbytes(fn->bytecode), fn->const_table);
        mp_printf(&mp_plat_print, "BYTECODE END\n");
        #endif
        return total_size;
    #if MICROPY_EMIT_NATIVE
    } else if (mp_obj_is_type(obj, &mp_type_fun_native)) {
        return 0;
    #endif
    #if MICROPY_EMIT_NATIVE
    } else if (mp_obj_is_type(obj, &mp_obj_fun_viper_t)) {
        return 0;
    #endif
    #if MICROPY_EMIT_THUMB
    } else if (mp_obj_is_type(obj, &mp_type_fun_asm)) {
        return 0;
    #endif
    }
    return 0;
}

static uint32_t array_size(uint8_t indent_level, mp_obj_array_t *array) {
    uint32_t total_size = gc_nbytes(array);

    uint32_t item_size = gc_nbytes(array->items);
    total_size += item_size;
    indent(indent_level);
    mp_printf(&mp_plat_print, "Array of size: %u\n\n", item_size);

    return total_size;
}

static uint32_t memoryview_size(uint8_t indent_level, mp_obj_array_t *array) {
    uint32_t total_size = gc_nbytes(array);

    indent(indent_level);
    mp_printf(&mp_plat_print, "memoryview\n");

    return total_size;
}

static uint32_t type_size(uint8_t indent_level, mp_obj_type_t *type) {
    uint32_t total_size = gc_nbytes(type);

    // mp_obj_base_t base;
    // qstr name;
    // total_size += string_size(indent_level, MP_OBJ_TO_PTR(type->name));
    // mp_print_fun_t print;
    // mp_make_new_fun_t make_new;     // to make an instance of the type
    //
    // mp_call_fun_t call;
    // mp_unary_op_fun_t unary_op;     // can return MP_OBJ_NULL if op not supported
    // mp_binary_op_fun_t binary_op;   // can return MP_OBJ_NULL if op not supported
    //
    // // implements load, store and delete attribute
    // //
    // // dest[0] = MP_OBJ_NULL means load
    // //  return: for fail, do nothing
    // //          for attr, dest[0] = value
    // //          for method, dest[0] = method, dest[1] = self
    // //
    // // dest[0,1] = {MP_OBJ_SENTINEL, MP_OBJ_NULL} means delete
    // // dest[0,1] = {MP_OBJ_SENTINEL, object} means store
    // //  return: for fail, do nothing
    // //          for success set dest[0] = MP_OBJ_NULL
    // mp_attr_fun_t attr;
    //
    // mp_subscr_fun_t subscr;         // implements load, store, delete subscripting
    //                                 // value=MP_OBJ_NULL means delete, value=MP_OBJ_SENTINEL means load, else store
    //                                 // can return MP_OBJ_NULL if op not supported
    //
    // mp_fun_1_t getiter;             // corresponds to __iter__ special method
    // mp_fun_1_t iternext; // may return MP_OBJ_STOP_ITERATION as an optimisation instead of raising StopIteration() (with no args)
    //
    // mp_buffer_p_t buffer_p;
    // // One of disjoint protocols (interfaces), like mp_stream_p_t, etc.
    // const void *protocol;
    //
    // // these are for dynamically created types (classes)
    // struct _mp_obj_tuple_t *bases_tuple;
    // struct _mp_obj_dict_t *locals_dict;
    if (type->locals_dict != NULL) {
        total_size += dict_size(indent_level, type->locals_dict);
    }

    indent(indent_level);
    mp_printf(&mp_plat_print, "TYPE\n");
    return total_size;
}


static uint32_t instance_size(uint8_t indent_level, mp_obj_instance_t *instance) {
    uint32_t total_size = gc_nbytes(instance);

    total_size += map_size(indent_level, &instance->members);

    return total_size;
}

static uint32_t module_size(uint8_t indent_level, mp_obj_module_t *module) {
    uint32_t total_size = gc_nbytes(module);

    indent(indent_level);
    mp_printf(&mp_plat_print, ".globals\n");

    total_size += dict_size(indent_level + 1, module->globals);

    indent(indent_level);
    mp_printf(&mp_plat_print, "Module size: %u\n", total_size);
    return total_size;
}

static uint32_t object_size(uint8_t indent_level, mp_obj_t obj) {
    if (obj == NULL) {
        return 0;
    }
    if (mp_obj_is_int(obj)) {
        return int_size(indent_level, MP_OBJ_TO_PTR(obj));
    } else if (mp_obj_is_str(obj)) {
        return string_size(indent_level, MP_OBJ_TO_PTR(obj));
    } else if (mp_obj_is_fun(obj)) {
        return function_size(indent_level, MP_OBJ_TO_PTR(obj));
    }
    if (!VERIFY_PTR(obj)) {
        // indent(indent_level);
        // mp_printf(&mp_plat_print, "In ROM\n");
        return 0;
    }
    mp_obj_t type = MP_OBJ_FROM_PTR(mp_obj_get_type(obj));

    if (type == &mp_type_module) {
        return module_size(indent_level, MP_OBJ_TO_PTR(obj));
    } else if (type == &mp_type_dict) {
        return dict_size(indent_level, MP_OBJ_TO_PTR(obj));
    } else if (type == &mp_type_type) {
        return type_size(indent_level, MP_OBJ_TO_PTR(obj));
    } else if (type == &mp_type_bytearray || type == &mp_type_array) {
        return array_size(indent_level, MP_OBJ_TO_PTR(obj));
    } else if (type == &mp_type_memoryview) {
        return memoryview_size(indent_level, MP_OBJ_TO_PTR(obj));
    } else if (mp_obj_is_obj(obj) && VERIFY_PTR(type)) {
        return instance_size(indent_level, MP_OBJ_TO_PTR(obj));
    }

    indent(indent_level);
    mp_printf(&mp_plat_print, "unknown type %x\n", type);
    return 0;
}

uint32_t shared_module_uheap_info(mp_obj_t obj) {
    if (!VERIFY_PTR(obj)) {
        mp_printf(&mp_plat_print, "Object not on heap.\n");
        return 0;
    }
    return object_size(0, obj);
}
