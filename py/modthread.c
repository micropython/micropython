/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Damien P. George on behalf of Pycom Ltd
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
#include <string.h>

#include "py/runtime.h"
#include "py/stackctrl.h"

#if MICROPY_PY_THREAD

#include "py/mpthread.h"

#if MICROPY_DEBUG_VERBOSE // print debugging info
#define DEBUG_PRINT (1)
#define DEBUG_printf DEBUG_printf
#else // don't print debugging info
#define DEBUG_PRINT (0)
#define DEBUG_printf(...) (void)0
#endif

/****************************************************************/
// Lock object

STATIC const mp_obj_type_t mp_type_thread_lock;

typedef struct _mp_obj_thread_lock_t {
    mp_obj_base_t base;
    mp_thread_mutex_t mutex;
    volatile bool locked;
} mp_obj_thread_lock_t;

STATIC mp_obj_thread_lock_t *mp_obj_new_thread_lock(void) {
    mp_obj_thread_lock_t *self = m_new_obj(mp_obj_thread_lock_t);
    self->base.type = &mp_type_thread_lock;
    mp_thread_mutex_init(&self->mutex);
    self->locked = false;
    return self;
}

STATIC mp_obj_t thread_lock_acquire(size_t n_args, const mp_obj_t *args) {
    mp_obj_thread_lock_t *self = MP_OBJ_TO_PTR(args[0]);
    bool wait = true;
    if (n_args > 1) {
        wait = mp_obj_get_int(args[1]);
        // TODO support timeout arg
    }
    MP_THREAD_GIL_EXIT();
    int ret = mp_thread_mutex_lock(&self->mutex, wait);
    MP_THREAD_GIL_ENTER();
    if (ret == 0) {
        return mp_const_false;
    } else if (ret == 1) {
        self->locked = true;
        return mp_const_true;
    } else {
        mp_raise_OSError(-ret);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(thread_lock_acquire_obj, 1, 3, thread_lock_acquire);

STATIC mp_obj_t thread_lock_release(mp_obj_t self_in) {
    mp_obj_thread_lock_t *self = MP_OBJ_TO_PTR(self_in);
    if (!self->locked) {
        mp_raise_msg(&mp_type_RuntimeError, NULL);
    }
    self->locked = false;
    MP_THREAD_GIL_EXIT();
    mp_thread_mutex_unlock(&self->mutex);
    MP_THREAD_GIL_ENTER();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(thread_lock_release_obj, thread_lock_release);

STATIC mp_obj_t thread_lock_locked(mp_obj_t self_in) {
    mp_obj_thread_lock_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(self->locked);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(thread_lock_locked_obj, thread_lock_locked);

STATIC mp_obj_t thread_lock___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args; // unused
    return thread_lock_release(args[0]);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(thread_lock___exit___obj, 4, 4, thread_lock___exit__);

STATIC const mp_rom_map_elem_t thread_lock_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_acquire), MP_ROM_PTR(&thread_lock_acquire_obj) },
    { MP_ROM_QSTR(MP_QSTR_release), MP_ROM_PTR(&thread_lock_release_obj) },
    { MP_ROM_QSTR(MP_QSTR_locked), MP_ROM_PTR(&thread_lock_locked_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&thread_lock_acquire_obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&thread_lock___exit___obj) },
};

STATIC MP_DEFINE_CONST_DICT(thread_lock_locals_dict, thread_lock_locals_dict_table);

STATIC const mp_obj_type_t mp_type_thread_lock = {
    { &mp_type_type },
    .name = MP_QSTR_lock,
    .locals_dict = (mp_obj_dict_t*)&thread_lock_locals_dict,
};

/****************************************************************/
// _thread module

STATIC size_t thread_stack_size = 0;

STATIC mp_obj_t mod_thread_get_ident(void) {
    return mp_obj_new_int_from_uint((uintptr_t)mp_thread_get_state());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_thread_get_ident_obj, mod_thread_get_ident);

STATIC mp_obj_t mod_thread_stack_size(size_t n_args, const mp_obj_t *args) {
    mp_obj_t ret = mp_obj_new_int_from_uint(thread_stack_size);
    if (n_args == 0) {
        thread_stack_size = 0;
    } else {
        thread_stack_size = mp_obj_get_int(args[0]);
    }
    return ret;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_thread_stack_size_obj, 0, 1, mod_thread_stack_size);

typedef struct _thread_entry_args_t {
    mp_obj_dict_t *dict_locals;
    mp_obj_dict_t *dict_globals;
    size_t stack_size;
    mp_obj_t fun;
    size_t n_args;
    size_t n_kw;
    mp_obj_t args[];
} thread_entry_args_t;

STATIC void *thread_entry(void *args_in) {
    // Execution begins here for a new thread.  We do not have the GIL.

    thread_entry_args_t *args = (thread_entry_args_t*)args_in;

    mp_state_thread_t ts;
    mp_thread_set_state(&ts);

    mp_stack_set_top(&ts + 1); // need to include ts in root-pointer scan
    mp_stack_set_limit(args->stack_size);

    // set locals and globals from the calling context
    mp_locals_set(args->dict_locals);
    mp_globals_set(args->dict_globals);

    MP_THREAD_GIL_ENTER();

    // signal that we are set up and running
    mp_thread_start();

    // TODO set more thread-specific state here:
    //  mp_pending_exception? (root pointer)
    //  cur_exception (root pointer)

    DEBUG_printf("[thread] start ts=%p args=%p stack=%p\n", &ts, &args, MP_STATE_THREAD(stack_top));

    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_call_function_n_kw(args->fun, args->n_args, args->n_kw, args->args);
        nlr_pop();
    } else {
        // uncaught exception
        // check for SystemExit
        mp_obj_base_t *exc = (mp_obj_base_t*)nlr.ret_val;
        if (mp_obj_is_subclass_fast(MP_OBJ_FROM_PTR(exc->type), MP_OBJ_FROM_PTR(&mp_type_SystemExit))) {
            // swallow exception silently
        } else {
            // print exception out
            mp_printf(MICROPY_ERROR_PRINTER, "Unhandled exception in thread started by ");
            mp_obj_print_helper(MICROPY_ERROR_PRINTER, args->fun, PRINT_REPR);
            mp_printf(MICROPY_ERROR_PRINTER, "\n");
            mp_obj_print_exception(MICROPY_ERROR_PRINTER, MP_OBJ_FROM_PTR(exc));
        }
    }

    DEBUG_printf("[thread] finish ts=%p\n", &ts);

    // signal that we are finished
    mp_thread_finish();

    MP_THREAD_GIL_EXIT();

    return NULL;
}

STATIC mp_obj_t mod_thread_start_new_thread(size_t n_args, const mp_obj_t *args) {
    // This structure holds the Python function and arguments for thread entry.
    // We copy all arguments into this structure to keep ownership of them.
    // We must be very careful about root pointers because this pointer may
    // disappear from our address space before the thread is created.
    thread_entry_args_t *th_args;

    // get positional arguments
    size_t pos_args_len;
    mp_obj_t *pos_args_items;
    mp_obj_get_array(args[1], &pos_args_len, &pos_args_items);

    // check for keyword arguments
    if (n_args == 2) {
        // just position arguments
        th_args = m_new_obj_var(thread_entry_args_t, mp_obj_t, pos_args_len);
        th_args->n_kw = 0;
    } else {
        // positional and keyword arguments
        if (mp_obj_get_type(args[2]) != &mp_type_dict) {
            mp_raise_TypeError("expecting a dict for keyword args");
        }
        mp_map_t *map = &((mp_obj_dict_t*)MP_OBJ_TO_PTR(args[2]))->map;
        th_args = m_new_obj_var(thread_entry_args_t, mp_obj_t, pos_args_len + 2 * map->used);
        th_args->n_kw = map->used;
        // copy across the keyword arguments
        for (size_t i = 0, n = pos_args_len; i < map->alloc; ++i) {
            if (MP_MAP_SLOT_IS_FILLED(map, i)) {
                th_args->args[n++] = map->table[i].key;
                th_args->args[n++] = map->table[i].value;
            }
        }
    }

    // copy agross the positional arguments
    th_args->n_args = pos_args_len;
    memcpy(th_args->args, pos_args_items, pos_args_len * sizeof(mp_obj_t));

    // pass our locals and globals into the new thread
    th_args->dict_locals = mp_locals_get();
    th_args->dict_globals = mp_globals_get();

    // set the stack size to use
    th_args->stack_size = thread_stack_size;

    // set the function for thread entry
    th_args->fun = args[0];

    // spawn the thread!
    mp_thread_create(thread_entry, th_args, &th_args->stack_size);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_thread_start_new_thread_obj, 2, 3, mod_thread_start_new_thread);

STATIC mp_obj_t mod_thread_exit(void) {
    nlr_raise(mp_obj_new_exception(&mp_type_SystemExit));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_thread_exit_obj, mod_thread_exit);

STATIC mp_obj_t mod_thread_allocate_lock(void) {
    return MP_OBJ_FROM_PTR(mp_obj_new_thread_lock());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_thread_allocate_lock_obj, mod_thread_allocate_lock);

STATIC const mp_rom_map_elem_t mp_module_thread_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR__thread) },
    { MP_ROM_QSTR(MP_QSTR_LockType), MP_ROM_PTR(&mp_type_thread_lock) },
    { MP_ROM_QSTR(MP_QSTR_get_ident), MP_ROM_PTR(&mod_thread_get_ident_obj) },
    { MP_ROM_QSTR(MP_QSTR_stack_size), MP_ROM_PTR(&mod_thread_stack_size_obj) },
    { MP_ROM_QSTR(MP_QSTR_start_new_thread), MP_ROM_PTR(&mod_thread_start_new_thread_obj) },
    { MP_ROM_QSTR(MP_QSTR_exit), MP_ROM_PTR(&mod_thread_exit_obj) },
    { MP_ROM_QSTR(MP_QSTR_allocate_lock), MP_ROM_PTR(&mod_thread_allocate_lock_obj) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_thread_globals, mp_module_thread_globals_table);

const mp_obj_module_t mp_module_thread = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_thread_globals,
};

#endif // MICROPY_PY_THREAD
