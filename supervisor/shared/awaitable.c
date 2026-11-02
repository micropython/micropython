// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "supervisor/shared/awaitable.h"
#include "py/mperrno.h"
#include "py/runtime.h"
#include "py/stream.h"

#if MICROPY_PY_ASYNC_AWAIT

#if MICROPY_PY_ASYNCIO
// Make the running asyncio task wait on the I/O queue until this awaitable polls readable.
// asyncio.core is found through sys.modules, because mp_asyncio_context in modasyncio.c is left
// pointing at the old heap after a soft reload.
static void asyncio_wait(mp_obj_t awaitable) {
    mp_map_elem_t *core = mp_map_lookup(&MP_STATE_VM(mp_loaded_modules_dict).map,
        MP_OBJ_NEW_QSTR(MP_QSTR_asyncio_dot_core), MP_MAP_LOOKUP);
    if (core == NULL) {
        return;
    }
    mp_obj_t cur_task = mp_load_attr(core->value, MP_QSTR_cur_task);
    if (cur_task == mp_const_none) {
        return;
    }
    mp_obj_t dest[4];
    mp_load_method(mp_load_attr(core->value, MP_QSTR__io_queue), MP_QSTR__enqueue, dest);
    dest[2] = awaitable;
    dest[3] = MP_OBJ_NEW_SMALL_INT(0);  // wait to read
    mp_call_method_n_kw(2, 0, dest);
}
#endif

// ---- the async function ------------------------------------------------------

// Store the arguments; they are parsed by start() when the awaitable is first resumed.
static mp_obj_t async_fun_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    const circuitpy_async_fun_obj_t *fun = MP_OBJ_TO_PTR(self_in);
    mp_arg_check_num_sig(n_args, n_kw, fun->sig);
    size_t n = n_args + 2 * n_kw;
    circuitpy_awaitable_obj_t *aw = mp_obj_malloc_var_with_finaliser(circuitpy_awaitable_obj_t,
        args, mp_obj_t, n, &circuitpy_awaitable_type);
    aw->fun = fun;
    aw->context = NULL;
    aw->state = CIRCUITPY_AWAITABLE_NEW;
    aw->n_args = n_args;
    aw->n_kw = n_kw;
    aw->next = NULL;
    memcpy(aw->args, args, n * sizeof(mp_obj_t));
    return MP_OBJ_FROM_PTR(aw);
}

MP_DEFINE_CONST_OBJ_TYPE(
    circuitpy_async_fun_type,
    MP_QSTR_function,
    MP_TYPE_FLAG_BINDS_SELF,
    call, async_fun_call
    );

// ---- the awaitable -----------------------------------------------------------

// Finishes the awaitable; stops the operation if it has started. A closed awaitable that
// was never awaited does not start later, as with a closed coroutine.
static void awaitable_cancel(circuitpy_awaitable_obj_t *self) {
    uint8_t state = self->state;
    self->state = CIRCUITPY_AWAITABLE_FINISHED;
    if (state == CIRCUITPY_AWAITABLE_RUNNING) {
        self->fun->cancel(self->args[0], self->context, &self->done);
    }
}

static mp_obj_t awaitable_iternext(mp_obj_t self_in) {
    circuitpy_awaitable_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->state == CIRCUITPY_AWAITABLE_FINISHED) {
        // Awaited again: like a finished generator, return None at once.
        MP_STATE_THREAD(stop_iteration_arg) = mp_const_none;
        return MP_OBJ_STOP_ITERATION;
    }
    if (self->state == CIRCUITPY_AWAITABLE_NEW) {
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, self->n_kw, self->args + self->n_args);
        CIRCUITPY_ASYNC_FLAG_INIT(&self->done);
        // An exception from start() (bad arguments) propagates out of the await.
        self->context = self->fun->start(&self->done, self->n_args, self->args, &kw_args);
        self->state = CIRCUITPY_AWAITABLE_RUNNING;
    }
    if (!CIRCUITPY_ASYNC_FLAG_IS_SET(&self->done)) {
        #if MICROPY_PY_ASYNCIO
        asyncio_wait(self_in);
        #endif
        return mp_const_none;
    }
    self->state = CIRCUITPY_AWAITABLE_FINISHED;
    MP_STATE_THREAD(stop_iteration_arg) = self->fun->end(self->args[0], self->context, &self->done);
    return MP_OBJ_STOP_ITERATION;
}

// A value sent in is ignored, as by CPython's awaitables written in C.
static mp_obj_t awaitable_send(mp_obj_t self_in, mp_obj_t value) {
    mp_obj_t ret = awaitable_iternext(self_in);
    if (ret == MP_OBJ_STOP_ITERATION) {
        mp_raise_StopIteration(MP_STATE_THREAD(stop_iteration_arg));
    }
    return ret;
}
static MP_DEFINE_CONST_FUN_OBJ_2(awaitable_send_obj, awaitable_send);

// throw() is how asyncio cancels a task: stop the operation and raise the exception.
static mp_obj_t awaitable_throw(mp_obj_t self_in, mp_obj_t exc) {
    awaitable_cancel(MP_OBJ_TO_PTR(self_in));
    nlr_raise(mp_make_raise_obj(exc));
}
static MP_DEFINE_CONST_FUN_OBJ_2(awaitable_throw_obj, awaitable_throw);

static mp_obj_t awaitable_close(mp_obj_t self_in) {
    awaitable_cancel(MP_OBJ_TO_PTR(self_in));
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(awaitable_close_obj, awaitable_close);

// Readable once the operation has finished.
static mp_uint_t awaitable_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    circuitpy_awaitable_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (request != MP_STREAM_POLL) {
        *errcode = MP_EINVAL;
        return MP_STREAM_ERROR;
    }
    return CIRCUITPY_ASYNC_FLAG_IS_SET(&self->done) ? (arg & MP_STREAM_POLL_RD) : 0;
}

static const mp_stream_p_t awaitable_stream_p = {
    .ioctl = awaitable_ioctl,
};

static const mp_rom_map_elem_t awaitable_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___await__), MP_ROM_PTR(&mp_identity_obj) },
    { MP_ROM_QSTR(MP_QSTR_send), MP_ROM_PTR(&awaitable_send_obj) },
    { MP_ROM_QSTR(MP_QSTR_throw), MP_ROM_PTR(&awaitable_throw_obj) },
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&awaitable_close_obj) },
    // Runs during GC: an operation still in flight must not write into freed memory.
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&awaitable_close_obj) },
};
static MP_DEFINE_CONST_DICT(awaitable_locals_dict, awaitable_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    circuitpy_awaitable_type,
    MP_QSTR_coroutine,
    MP_TYPE_FLAG_ITER_IS_ITERNEXT,
    iter, awaitable_iternext,
    protocol, &awaitable_stream_p,
    locals_dict, &awaitable_locals_dict
    );

// ---- the lock ----------------------------------------------------------------

void circuitpy_async_lock_init(circuitpy_async_lock_obj_t *lock) {
    lock->base.type = &circuitpy_async_lock_type;
    lock->held = false;
    lock->waiting = NULL;
}

bool circuitpy_async_lock_try(circuitpy_async_lock_obj_t *lock) {
    if (lock->held) {
        return false;
    }
    lock->held = true;
    return true;
}

void circuitpy_async_lock_release(circuitpy_async_lock_obj_t *lock) {
    circuitpy_awaitable_obj_t *next = lock->waiting;
    if (next == NULL) {
        lock->held = false;
        return;
    }
    // The lock stays held; it is the next one's now.
    lock->waiting = next->next;
    CIRCUITPY_ASYNC_FLAG_SET(&next->done);
}

// Each one woken gets the lock in turn and passes it on from its __aexit__; what it does with the
// deinited object raises.
void circuitpy_async_lock_deinit(circuitpy_async_lock_obj_t *lock) {
    while (lock->waiting != NULL) {
        circuitpy_async_lock_release(lock);
    }
}

// __aenter__: take the lock, or join the end of the line. done is set when it is ours.
static void *lock_enter_start(circuitpy_async_flag_t *done, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    circuitpy_async_lock_obj_t *lock = MP_OBJ_TO_PTR(pos_args[0]);
    if (circuitpy_async_lock_try(lock)) {
        CIRCUITPY_ASYNC_FLAG_SET(done);
        return NULL;
    }
    circuitpy_awaitable_obj_t *aw = CIRCUITPY_AWAITABLE_OF(done);
    circuitpy_awaitable_obj_t **p = &lock->waiting;
    while (*p != NULL) {
        p = &(*p)->next;
    }
    *p = aw;
    return NULL;
}

static mp_obj_t lock_enter_end(mp_obj_t self_in, void *context, circuitpy_async_flag_t *done) {
    return self_in;
}

// Cancelled while waiting: leave the line. Cancelled after the lock was handed over but before
// the task ran: pass it on.
static void lock_enter_cancel(mp_obj_t self_in, void *context, circuitpy_async_flag_t *done) {
    circuitpy_async_lock_obj_t *lock = MP_OBJ_TO_PTR(self_in);
    if (CIRCUITPY_ASYNC_FLAG_IS_SET(done)) {
        circuitpy_async_lock_release(lock);
        return;
    }
    circuitpy_awaitable_obj_t *aw = CIRCUITPY_AWAITABLE_OF(done);
    for (circuitpy_awaitable_obj_t **p = &lock->waiting; *p != NULL; p = &(*p)->next) {
        if (*p == aw) {
            *p = aw->next;
            return;
        }
    }
}
static CIRCUITPY_DEFINE_ASYNC_FUN_OBJ_KW(lock_enter_obj, 1, lock_enter_start, lock_enter_end, lock_enter_cancel);

// __aexit__ returns the lock itself: awaiting it finishes at once with None, so an exception
// in the block is not swallowed.
static mp_obj_t lock_exit(size_t n_args, const mp_obj_t *args) {
    circuitpy_async_lock_release(MP_OBJ_TO_PTR(args[0]));
    return args[0];
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(lock_exit_obj, 4, 4, lock_exit);

static mp_obj_t lock_iternext(mp_obj_t self_in) {
    MP_STATE_THREAD(stop_iteration_arg) = mp_const_none;
    return MP_OBJ_STOP_ITERATION;
}

static const mp_rom_map_elem_t lock_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___aenter__), MP_ROM_PTR(&lock_enter_obj) },
    { MP_ROM_QSTR(MP_QSTR___aexit__), MP_ROM_PTR(&lock_exit_obj) },
};
static MP_DEFINE_CONST_DICT(lock_locals_dict, lock_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    circuitpy_async_lock_type,
    MP_QSTR_Lock,
    MP_TYPE_FLAG_ITER_IS_ITERNEXT,
    iter, lock_iternext,
    locals_dict, &lock_locals_dict
    );

#endif // MICROPY_PY_ASYNC_AWAIT
