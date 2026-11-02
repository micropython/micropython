// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// Awaitable functions implemented in C.
//
// CIRCUITPY_DEFINE_ASYNC_FUN_OBJ_KW(obj_name, n_args_min, start, end, cancel) defines obj_name,
// a callable that takes the same arguments as an MP_DEFINE_CONST_FUN_OBJ_KW function but returns
// an awaitable. A call only checks the number of positional arguments; as with a coroutine in
// CPython, nothing runs and the arguments are parsed only when the awaitable is awaited. Then:
//
//   void *start(circuitpy_async_flag_t *done, size_t n_args, const mp_obj_t *pos_args,
//       mp_map_t *kw_args)
//       parses the arguments, starts the operation and returns a context: state of this one
//       operation, or NULL when the object holds it all. The operation sets *done, possibly
//       from an interrupt, once it has finished.
//   mp_obj_t end(mp_obj_t self_in, void *context, circuitpy_async_flag_t *done)
//       finishes the operation once *done is set and returns the result of the await.
//   void cancel(mp_obj_t self_in, void *context, circuitpy_async_flag_t *done)
//       stops the operation, or finishes it if *done is already set. It must not allocate: it
//       may run during GC.
//
// self_in is the first positional argument, the object for a method. done tells which operation
// is meant, if the object has started another one since.
//
// While an operation runs, the awaiting asyncio task waits on asyncio's I/O queue: the awaitable
// is a stream that polls readable once *done is set.

#pragma once

#include <stddef.h>

#include "py/obj.h"
#include "supervisor/shared/async_flag.h"

#if MICROPY_PY_ASYNC_AWAIT

typedef void *(*circuitpy_awaitable_start_fn)(circuitpy_async_flag_t *done, size_t n_args,
    const mp_obj_t *pos_args, mp_map_t *kw_args);
typedef mp_obj_t (*circuitpy_awaitable_end_fn)(mp_obj_t self_in, void *context,
    circuitpy_async_flag_t *done);
typedef void (*circuitpy_awaitable_cancel_fn)(mp_obj_t self_in, void *context,
    circuitpy_async_flag_t *done);

// An async function: calling it returns a circuitpy_awaitable_obj_t.
typedef struct {
    mp_obj_base_t base;
    uint32_t sig;                       // as made by MP_OBJ_FUN_MAKE_SIG
    circuitpy_awaitable_start_fn start;
    circuitpy_awaitable_end_fn end;
    circuitpy_awaitable_cancel_fn cancel;
} circuitpy_async_fun_obj_t;

extern const mp_obj_type_t circuitpy_async_fun_type;

typedef struct _circuitpy_awaitable_obj_t {
    mp_obj_base_t base;
    const circuitpy_async_fun_obj_t *fun;
    void *context;                  // returned by start()
    circuitpy_async_flag_t done;
    uint8_t state;                  // CIRCUITPY_AWAITABLE_NEW, _RUNNING or _FINISHED
    uint16_t n_args;
    uint16_t n_kw;
    struct _circuitpy_awaitable_obj_t *next;   // the next one waiting for the same lock
    mp_obj_t args[];                // positional arguments, then keyword name/value pairs
} circuitpy_awaitable_obj_t;

extern const mp_obj_type_t circuitpy_awaitable_type;

// The awaitable that owns a done flag passed to start(), end() or cancel().
#define CIRCUITPY_AWAITABLE_OF(done_ptr) \
    ((circuitpy_awaitable_obj_t *)((char *)(done_ptr) - offsetof(circuitpy_awaitable_obj_t, done)))

// A lock for asyncio code: `async with lock:`. __aenter__ is an awaitable that takes the lock
// or waits in line for it; __aexit__ hands the lock to the next in line. Embed it in the
// object it guards; it allocates nothing.
typedef struct {
    mp_obj_base_t base;
    bool held;
    circuitpy_awaitable_obj_t *waiting;     // first in line, linked through next
} circuitpy_async_lock_obj_t;

extern const mp_obj_type_t circuitpy_async_lock_type;

void circuitpy_async_lock_init(circuitpy_async_lock_obj_t *lock);
static inline bool circuitpy_async_lock_held(circuitpy_async_lock_obj_t *lock) {
    return lock->held;
}
// For C code: takes the lock if it is free.
bool circuitpy_async_lock_try(circuitpy_async_lock_obj_t *lock);
// Hands the lock to the next in line, or frees it.
void circuitpy_async_lock_release(circuitpy_async_lock_obj_t *lock);
// Wakes everyone in line, for the deinit of the object the lock guards.
void circuitpy_async_lock_deinit(circuitpy_async_lock_obj_t *lock);

enum { CIRCUITPY_AWAITABLE_NEW, CIRCUITPY_AWAITABLE_RUNNING, CIRCUITPY_AWAITABLE_FINISHED };

#define CIRCUITPY_DEFINE_ASYNC_FUN_OBJ_KW(obj_name, n_args_min, start, end, cancel) \
    const circuitpy_async_fun_obj_t obj_name = { \
        {&circuitpy_async_fun_type}, MP_OBJ_FUN_MAKE_SIG(n_args_min, MP_OBJ_FUN_ARGS_MAX, true), \
        start, end, cancel \
    }

#endif // MICROPY_PY_ASYNC_AWAIT
