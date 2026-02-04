/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Damien P. George
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

#include "py/gc.h"
#include "py/runtime.h"

#if MICROPY_PY_WEAKREF

// Macros to obfuscate a heap pointer as a small integer object.
#define PTR_TO_INT_OBJ(ptr) (MP_OBJ_NEW_SMALL_INT(((uintptr_t)ptr) >> 1))
#define PTR_FROM_INT_OBJ(obj) ((void *)(MP_OBJ_SMALL_INT_VALUE((obj)) << 1))

// Macros to convert between a weak reference and a heap pointer.
#define WEAK_REFERENCE_FROM_HEAP_PTR(ptr) PTR_TO_INT_OBJ(ptr)
#define WEAK_REFERENCE_TO_HEAP_PTR(weak_ref) PTR_FROM_INT_OBJ(weak_ref)

// Macros to manage ref-finalizer linked-list pointers.
// - mp_obj_ref_t is obfuscated as a small integer object so it's not traced by the GC.
// - mp_obj_finalize_t is stored as-is so it is traced by the GC.
#define REF_FIN_LIST_OBJ_IS_FIN(r) (!mp_obj_is_small_int((r)))
#define REF_FIN_LIST_OBJ_TO_PTR(r) ((mp_obj_ref_t *)(mp_obj_is_small_int((r)) ? PTR_FROM_INT_OBJ((r)) : MP_OBJ_TO_PTR((r))))
#define REF_FIN_LIST_OBJ_FROM_REF(r) (PTR_TO_INT_OBJ((r)))
#define REF_FIN_LIST_OBJ_FROM_FIN(r) (MP_OBJ_FROM_PTR((r)))
#define REF_FIN_LIST_OBJ_TAIL (PTR_TO_INT_OBJ(NULL))

// weakref.ref() instance.
typedef struct _mp_obj_ref_t {
    mp_obj_base_t base;
    mp_obj_t ref_fin_next;
    mp_obj_t obj_weak_ref;
    mp_obj_t callback;
} mp_obj_ref_t;

// weakref.finalize() instance.
// This is an extension of weakref.ref() and shares a lot of code with it.
typedef struct _mp_obj_finalize_t {
    mp_obj_ref_t base;
    size_t n_args;
    size_t n_kw;
    mp_obj_t *args;
} mp_obj_finalize_t;

static const mp_obj_type_t mp_type_ref;
static const mp_obj_type_t mp_type_finalize;

static mp_obj_t ref___del__(mp_obj_t self_in);

void gc_weakref_about_to_be_freed(void *ptr) {
    mp_obj_t idx = PTR_TO_INT_OBJ(ptr);
    mp_map_elem_t *elem = mp_map_lookup(&MP_STATE_VM(mp_weakref_map), idx, MP_MAP_LOOKUP);
    if (elem != NULL) {
        // Mark element as being freed.
        elem->key = mp_const_none;
    }
}

void gc_weakref_sweep(void) {
    mp_map_t *map = &MP_STATE_VM(mp_weakref_map);
    for (size_t i = 0; i < map->alloc; i++) {
        if (map->table[i].key == mp_const_none) {
            // Element was just freed, so call all the registered callbacks.
            --map->used;
            map->table[i].key = MP_OBJ_SENTINEL;
            mp_obj_ref_t *ref = REF_FIN_LIST_OBJ_TO_PTR(map->table[i].value);
            map->table[i].value = MP_OBJ_NULL;
            while (ref != NULL) {
                // Invalidate the weak reference.
                assert(ref->obj_weak_ref != mp_const_none);
                ref->obj_weak_ref = mp_const_none;

                // Call any registered callbacks.
                if (ref->callback != mp_const_none) {
                    nlr_buf_t nlr;
                    if (nlr_push(&nlr) == 0) {
                        if (ref->base.type == &mp_type_ref) {
                            // weakref.ref() type.
                            mp_call_function_1(ref->callback, MP_OBJ_FROM_PTR(ref));
                        } else {
                            // weakref.finalize() type.
                            mp_obj_finalize_t *fin = (mp_obj_finalize_t *)ref;
                            mp_call_function_n_kw(fin->base.callback, fin->n_args, fin->n_kw, fin->args);
                        }
                        nlr_pop();
                    } else {
                        mp_printf(MICROPY_ERROR_PRINTER, "Unhandled exception in weakref callback:\n");
                        mp_obj_print_exception(MICROPY_ERROR_PRINTER, MP_OBJ_FROM_PTR(nlr.ret_val));
                    }
                }

                // Unlink the node.
                mp_obj_ref_t *ref_fin_next = REF_FIN_LIST_OBJ_TO_PTR(ref->ref_fin_next);
                ref->ref_fin_next = REF_FIN_LIST_OBJ_TAIL;
                ref = ref_fin_next;
            }
        }
    }
}

static mp_obj_t mp_obj_ref_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    if (type == &mp_type_ref) {
        // weakref.ref() type.
        mp_arg_check_num(n_args, n_kw, 1, 2, false);
    } else {
        // weakref.finalize() type.
        mp_arg_check_num(n_args, n_kw, 2, MP_OBJ_FUN_ARGS_MAX, true);
    }

    // Validate the input object can have a weakref.
    void *ptr = NULL;
    if (mp_obj_is_obj(args[0])) {
        ptr = MP_OBJ_TO_PTR(args[0]);
        if (gc_nbytes(ptr) == 0) {
            ptr = NULL;
        }
    }
    if (ptr == NULL) {
        mp_raise_TypeError(MP_ERROR_TEXT("not a heap object"));
    }

    // Create or get the entry in mp_weakref_map corresponding to this object.
    mp_obj_t obj_weak_reference = WEAK_REFERENCE_FROM_HEAP_PTR(ptr);
    mp_map_elem_t *elem = mp_map_lookup(&MP_STATE_VM(mp_weakref_map), obj_weak_reference, MP_MAP_LOOKUP_ADD_IF_NOT_FOUND);
    if (elem->value == MP_OBJ_NULL) {
        // This heap object does not have any existing weakref's, so initialise it.
        elem->value = REF_FIN_LIST_OBJ_TAIL;
        gc_weakref_mark(ptr);
    }

    mp_obj_ref_t *self;
    if (type == &mp_type_ref) {
        // Create a new weakref.ref() object.
        self = mp_obj_malloc_with_finaliser(mp_obj_ref_t, type);
        // Link this new ref into the list of all refs/finalizers pointing to this object.
        // To ensure it will *NOT* be traced by the GC (the user must manually hold onto it),
        // store an integer version of the object after any weakref.finalize() objects (so
        // the weakref.finalize() objects continue to be traced by the GC).
        mp_obj_t *link = &elem->value;
        while (REF_FIN_LIST_OBJ_IS_FIN(*link)) {
            link = &REF_FIN_LIST_OBJ_TO_PTR(*link)->ref_fin_next;
        }
        self->ref_fin_next = *link;
        *link = REF_FIN_LIST_OBJ_FROM_REF(self);
    } else {
        // Create a new weakref.finalize() object.
        mp_obj_finalize_t *self_fin = mp_obj_malloc(mp_obj_finalize_t, type);
        self_fin->n_args = n_args - 2;
        self_fin->n_kw = n_kw;
        size_t n_args_kw = self_fin->n_args + self_fin->n_kw * 2;
        if (n_args_kw == 0) {
            self_fin->args = NULL;
        } else {
            self_fin->args = m_new(mp_obj_t, n_args_kw);
            memcpy(self_fin->args, args + 2, n_args_kw * sizeof(mp_obj_t));
        }
        self = &self_fin->base;
        // Link this new finalizer into the list of all refs/finalizers pointing to this object.
        // To ensure it will be traced by the GC, store its pointer at the start of the list.
        self->ref_fin_next = elem->value;
        elem->value = REF_FIN_LIST_OBJ_FROM_FIN(self_fin);
    }

    // Populate the object weak reference, and the callback.
    self->obj_weak_ref = obj_weak_reference;
    if (n_args > 1) {
        self->callback = args[1];
    } else {
        self->callback = mp_const_none;
    }

    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t mp_obj_ref_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_obj_ref_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->obj_weak_ref == mp_const_none) {
        return mp_const_none;
    }
    if (self->base.type == &mp_type_ref) {
        // weakref.ref() type.
        return MP_OBJ_FROM_PTR(WEAK_REFERENCE_TO_HEAP_PTR(self->obj_weak_ref));
    } else {
        // weakref.finalize() type.
        mp_obj_finalize_t *self_fin = MP_OBJ_TO_PTR(self_in);
        ref___del__(self_in);
        return mp_call_function_n_kw(self_fin->base.callback, self_fin->n_args, self_fin->n_kw, self_fin->args);
    }
}

static mp_obj_t ref___del__(mp_obj_t self_in) {
    mp_obj_ref_t *self = MP_OBJ_TO_PTR(self_in);
    mp_map_elem_t *elem = mp_map_lookup(&MP_STATE_VM(mp_weakref_map), self->obj_weak_ref, MP_MAP_LOOKUP);
    if (elem != NULL) {
        for (mp_obj_t *link = &elem->value; REF_FIN_LIST_OBJ_TO_PTR(*link) != NULL; link = &REF_FIN_LIST_OBJ_TO_PTR(*link)->ref_fin_next) {
            if (self == REF_FIN_LIST_OBJ_TO_PTR(*link)) {
                // Unlink and clear this node.
                *link = self->ref_fin_next;
                self->ref_fin_next = REF_FIN_LIST_OBJ_TAIL;
                self->obj_weak_ref = mp_const_none;
                break;
            }
        }
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(ref___del___obj, ref___del__);

static mp_obj_t finalize_peek_detach_helper(mp_obj_t self_in, bool detach) {
    mp_obj_finalize_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->base.obj_weak_ref == mp_const_none) {
        return mp_const_none;
    }
    mp_obj_t tuple[4] = {
        MP_OBJ_FROM_PTR(WEAK_REFERENCE_TO_HEAP_PTR(self->base.obj_weak_ref)),
        self->base.callback,
        mp_obj_new_tuple(self->n_args, self->args),
        mp_obj_dict_make_new(&mp_type_dict, 0, self->n_kw, self->args + self->n_args),
    };
    if (detach) {
        ref___del__(self_in);
    }
    return mp_obj_new_tuple(MP_ARRAY_SIZE(tuple), tuple);
}

static mp_obj_t finalize_peek(mp_obj_t self_in) {
    return finalize_peek_detach_helper(self_in, false);
}
static MP_DEFINE_CONST_FUN_OBJ_1(finalize_peek_obj, finalize_peek);

static mp_obj_t finalize_detach(mp_obj_t self_in) {
    return finalize_peek_detach_helper(self_in, true);
}
static MP_DEFINE_CONST_FUN_OBJ_1(finalize_detach_obj, finalize_detach);

static void mp_obj_finalize_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    if (dest[0] != MP_OBJ_NULL) {
        // Store/delete attribute, unsupported.
        return;
    }

    if (attr == MP_QSTR_alive) {
        mp_obj_finalize_t *self = MP_OBJ_TO_PTR(self_in);
        dest[0] = mp_obj_new_bool(self->base.obj_weak_ref != mp_const_none);
        return;
    } else if (attr == MP_QSTR_peek) {
        dest[0] = MP_OBJ_FROM_PTR(&finalize_peek_obj);
        dest[1] = self_in;
    } else if (attr == MP_QSTR_detach) {
        dest[0] = MP_OBJ_FROM_PTR(&finalize_detach_obj);
        dest[1] = self_in;
    }
}

static const mp_rom_map_elem_t ref_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&ref___del___obj) },
};
static MP_DEFINE_CONST_DICT(ref_locals_dict, ref_locals_dict_table);

static MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_ref,
    MP_QSTR_ref,
    MP_TYPE_FLAG_NONE,
    make_new, mp_obj_ref_make_new,
    call, mp_obj_ref_call,
    locals_dict, &ref_locals_dict
    );

static MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_finalize,
    MP_QSTR_finalize,
    MP_TYPE_FLAG_NONE,
    make_new, mp_obj_ref_make_new,
    call, mp_obj_ref_call,
    attr, mp_obj_finalize_attr
    );

static const mp_rom_map_elem_t mp_module_weakref_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_weakref) },
    { MP_ROM_QSTR(MP_QSTR_ref), MP_ROM_PTR(&mp_type_ref) },
    { MP_ROM_QSTR(MP_QSTR_finalize), MP_ROM_PTR(&mp_type_finalize) },
};
static MP_DEFINE_CONST_DICT(mp_module_weakref_globals, mp_module_weakref_globals_table);

const mp_obj_module_t mp_module_weakref = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_weakref_globals,
};

MP_REGISTER_ROOT_POINTER(mp_map_t mp_weakref_map);
MP_REGISTER_MODULE(MP_QSTR_weakref, mp_module_weakref);

#endif // MICROPY_PY_WEAKREF
