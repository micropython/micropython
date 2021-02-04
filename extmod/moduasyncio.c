/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Damien P. George
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
#include "py/smallint.h"
#include "py/pairheap.h"
#include "py/mphal.h"

#if MICROPY_PY_UASYNCIO

typedef struct _mp_obj_task_t {
    mp_pairheap_t pairheap;
    mp_obj_t coro;
    mp_obj_t data;
    mp_obj_t waiting;

    mp_obj_t ph_key;
} mp_obj_task_t;

typedef struct _mp_obj_task_queue_t {
    mp_obj_base_t base;
    mp_obj_task_t *heap;
} mp_obj_task_queue_t;

STATIC const mp_obj_type_t task_queue_type;
STATIC const mp_obj_type_t task_type;

STATIC mp_obj_t task_queue_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args);

/******************************************************************************/
// Ticks for task ordering in pairing heap

STATIC mp_obj_t ticks(void) {
    return MP_OBJ_NEW_SMALL_INT(mp_hal_ticks_ms() & (MICROPY_PY_UTIME_TICKS_PERIOD - 1));
}

STATIC mp_int_t ticks_diff(mp_obj_t t1_in, mp_obj_t t0_in) {
    mp_uint_t t0 = MP_OBJ_SMALL_INT_VALUE(t0_in);
    mp_uint_t t1 = MP_OBJ_SMALL_INT_VALUE(t1_in);
    mp_int_t diff = ((t1 - t0 + MICROPY_PY_UTIME_TICKS_PERIOD / 2) & (MICROPY_PY_UTIME_TICKS_PERIOD - 1))
        - MICROPY_PY_UTIME_TICKS_PERIOD / 2;
    return diff;
}

STATIC int task_lt(mp_pairheap_t *n1, mp_pairheap_t *n2) {
    mp_obj_task_t *t1 = (mp_obj_task_t *)n1;
    mp_obj_task_t *t2 = (mp_obj_task_t *)n2;
    return MP_OBJ_SMALL_INT_VALUE(ticks_diff(t1->ph_key, t2->ph_key)) < 0;
}

/******************************************************************************/
// TaskQueue class

STATIC mp_obj_t task_queue_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    (void)args;
    mp_arg_check_num(n_args, n_kw, 0, 0, false);
    mp_obj_task_queue_t *self = m_new_obj(mp_obj_task_queue_t);
    self->base.type = type;
    self->heap = (mp_obj_task_t *)mp_pairheap_new(task_lt);
    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t task_queue_peek(mp_obj_t self_in) {
    mp_obj_task_queue_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->heap == NULL) {
        return mp_const_none;
    } else {
        return MP_OBJ_FROM_PTR(self->heap);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(task_queue_peek_obj, task_queue_peek);

STATIC mp_obj_t task_queue_push_sorted(size_t n_args, const mp_obj_t *args) {
    mp_obj_task_queue_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_obj_task_t *task = MP_OBJ_TO_PTR(args[1]);
    task->data = mp_const_none;
    if (n_args == 2) {
        task->ph_key = ticks();
    } else {
        assert(mp_obj_is_small_int(args[2]));
        task->ph_key = args[2];
    }
    self->heap = (mp_obj_task_t *)mp_pairheap_push(task_lt, &self->heap->pairheap, &task->pairheap);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(task_queue_push_sorted_obj, 2, 3, task_queue_push_sorted);

STATIC mp_obj_t task_queue_pop_head(mp_obj_t self_in) {
    mp_obj_task_queue_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_task_t *head = (mp_obj_task_t *)mp_pairheap_peek(task_lt, &self->heap->pairheap);
    if (head == NULL) {
        mp_raise_msg(&mp_type_IndexError, MP_ERROR_TEXT("empty heap"));
    }
    self->heap = (mp_obj_task_t *)mp_pairheap_pop(task_lt, &self->heap->pairheap);
    return MP_OBJ_FROM_PTR(head);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(task_queue_pop_head_obj, task_queue_pop_head);

STATIC mp_obj_t task_queue_remove(mp_obj_t self_in, mp_obj_t task_in) {
    mp_obj_task_queue_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_task_t *task = MP_OBJ_TO_PTR(task_in);
    self->heap = (mp_obj_task_t *)mp_pairheap_delete(task_lt, &self->heap->pairheap, &task->pairheap);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(task_queue_remove_obj, task_queue_remove);

STATIC const mp_rom_map_elem_t task_queue_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_peek), MP_ROM_PTR(&task_queue_peek_obj) },
    { MP_ROM_QSTR(MP_QSTR_push_sorted), MP_ROM_PTR(&task_queue_push_sorted_obj) },
    { MP_ROM_QSTR(MP_QSTR_push_head), MP_ROM_PTR(&task_queue_push_sorted_obj) },
    { MP_ROM_QSTR(MP_QSTR_pop_head), MP_ROM_PTR(&task_queue_pop_head_obj) },
    { MP_ROM_QSTR(MP_QSTR_remove), MP_ROM_PTR(&task_queue_remove_obj) },
};
STATIC MP_DEFINE_CONST_DICT(task_queue_locals_dict, task_queue_locals_dict_table);

STATIC const mp_obj_type_t task_queue_type = {
    { &mp_type_type },
    .name = MP_QSTR_TaskQueue,
    .make_new = task_queue_make_new,
    .locals_dict = (mp_obj_dict_t *)&task_queue_locals_dict,
};

/******************************************************************************/
// Task class

// For efficiency, the task object is stored to the coro entry when the task is done.
#define TASK_IS_DONE(task) ((task)->coro == MP_OBJ_FROM_PTR(task))

// This is the core uasyncio context with cur_task, _task_queue and CancelledError.
STATIC mp_obj_t uasyncio_context = MP_OBJ_NULL;

STATIC mp_obj_t task_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 2, false);
    mp_obj_task_t *self = m_new_obj(mp_obj_task_t);
    self->pairheap.base.type = type;
    mp_pairheap_init_node(task_lt, &self->pairheap);
    self->coro = args[0];
    self->data = mp_const_none;
    self->waiting = mp_const_none;
    self->ph_key = MP_OBJ_NEW_SMALL_INT(0);
    if (n_args == 2) {
        uasyncio_context = args[1];
    }
    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t task_done(mp_obj_t self_in) {
    mp_obj_task_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(TASK_IS_DONE(self));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(task_done_obj, task_done);

STATIC mp_obj_t task_cancel(mp_obj_t self_in) {
    mp_obj_task_t *self = MP_OBJ_TO_PTR(self_in);
    // Check if task is already finished.
    if (TASK_IS_DONE(self)) {
        return mp_const_false;
    }
    // Can't cancel self (not supported yet).
    mp_obj_t cur_task = mp_obj_dict_get(uasyncio_context, MP_OBJ_NEW_QSTR(MP_QSTR_cur_task));
    if (self_in == cur_task) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("can't cancel self"));
    }
    // If Task waits on another task then forward the cancel to the one it's waiting on.
    while (mp_obj_is_subclass_fast(MP_OBJ_FROM_PTR(mp_obj_get_type(self->data)), MP_OBJ_FROM_PTR(&task_type))) {
        self = MP_OBJ_TO_PTR(self->data);
    }

    mp_obj_t _task_queue = mp_obj_dict_get(uasyncio_context, MP_OBJ_NEW_QSTR(MP_QSTR__task_queue));

    // Reschedule Task as a cancelled task.
    mp_obj_t dest[3];
    mp_load_method_maybe(self->data, MP_QSTR_remove, dest);
    if (dest[0] != MP_OBJ_NULL) {
        // Not on the main running queue, remove the task from the queue it's on.
        dest[2] = MP_OBJ_FROM_PTR(self);
        mp_call_method_n_kw(1, 0, dest);
        // _task_queue.push_head(self)
        dest[0] = _task_queue;
        dest[1] = MP_OBJ_FROM_PTR(self);
        task_queue_push_sorted(2, dest);
    } else if (ticks_diff(self->ph_key, ticks()) > 0) {
        // On the main running queue but scheduled in the future, so bring it forward to now.
        // _task_queue.remove(self)
        task_queue_remove(_task_queue, MP_OBJ_FROM_PTR(self));
        // _task_queue.push_head(self)
        dest[0] = _task_queue;
        dest[1] = MP_OBJ_FROM_PTR(self);
        task_queue_push_sorted(2, dest);
    }

    self->data = mp_obj_dict_get(uasyncio_context, MP_OBJ_NEW_QSTR(MP_QSTR_CancelledError));

    return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(task_cancel_obj, task_cancel);

STATIC mp_obj_t task_throw(mp_obj_t self_in, mp_obj_t value_in) {
    // This task raised an exception which was uncaught; handle that now.
    mp_obj_task_t *self = MP_OBJ_TO_PTR(self_in);
    // Set the data because it was cleared by the main scheduling loop.
    self->data = value_in;
    if (self->waiting == mp_const_none) {
        // Nothing await'ed on the task so call the exception handler.
        mp_obj_t _exc_context = mp_obj_dict_get(uasyncio_context, MP_OBJ_NEW_QSTR(MP_QSTR__exc_context));
        mp_obj_dict_store(_exc_context, MP_OBJ_NEW_QSTR(MP_QSTR_exception), value_in);
        mp_obj_dict_store(_exc_context, MP_OBJ_NEW_QSTR(MP_QSTR_future), self_in);
        mp_obj_t Loop = mp_obj_dict_get(uasyncio_context, MP_OBJ_NEW_QSTR(MP_QSTR_Loop));
        mp_obj_t call_exception_handler = mp_load_attr(Loop, MP_QSTR_call_exception_handler);
        mp_call_function_1(call_exception_handler, _exc_context);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(task_throw_obj, task_throw);

STATIC void task_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    mp_obj_task_t *self = MP_OBJ_TO_PTR(self_in);
    if (dest[0] == MP_OBJ_NULL) {
        // Load
        if (attr == MP_QSTR_coro) {
            dest[0] = self->coro;
        } else if (attr == MP_QSTR_data) {
            dest[0] = self->data;
        } else if (attr == MP_QSTR_waiting) {
            if (self->waiting != mp_const_none && self->waiting != mp_const_false) {
                dest[0] = self->waiting;
            }
        } else if (attr == MP_QSTR_done) {
            dest[0] = MP_OBJ_FROM_PTR(&task_done_obj);
            dest[1] = self_in;
        } else if (attr == MP_QSTR_cancel) {
            dest[0] = MP_OBJ_FROM_PTR(&task_cancel_obj);
            dest[1] = self_in;
        } else if (attr == MP_QSTR_throw) {
            dest[0] = MP_OBJ_FROM_PTR(&task_throw_obj);
            dest[1] = self_in;
        } else if (attr == MP_QSTR_ph_key) {
            dest[0] = self->ph_key;
        }
    } else if (dest[1] != MP_OBJ_NULL) {
        // Store
        if (attr == MP_QSTR_coro) {
            self->coro = dest[1];
            dest[0] = MP_OBJ_NULL;
        } else if (attr == MP_QSTR_data) {
            self->data = dest[1];
            dest[0] = MP_OBJ_NULL;
        } else if (attr == MP_QSTR_waiting) {
            self->waiting = dest[1];
            dest[0] = MP_OBJ_NULL;
        }
    }
}

STATIC mp_obj_t task_getiter(mp_obj_t self_in, mp_obj_iter_buf_t *iter_buf) {
    (void)iter_buf;
    mp_obj_task_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->waiting == mp_const_none) {
        // The is the first access of the "waiting" entry.
        if (TASK_IS_DONE(self)) {
            // Signal that the completed-task has been await'ed on.
            self->waiting = mp_const_false;
        } else {
            // Lazily allocate the waiting queue.
            self->waiting = task_queue_make_new(&task_queue_type, 0, 0, NULL);
        }
    }
    return self_in;
}

STATIC mp_obj_t task_iternext(mp_obj_t self_in) {
    mp_obj_task_t *self = MP_OBJ_TO_PTR(self_in);
    if (TASK_IS_DONE(self)) {
        // Task finished, raise return value to caller so it can continue.
        nlr_raise(self->data);
    } else {
        // Put calling task on waiting queue.
        mp_obj_t cur_task = mp_obj_dict_get(uasyncio_context, MP_OBJ_NEW_QSTR(MP_QSTR_cur_task));
        mp_obj_t args[2] = { self->waiting, cur_task };
        task_queue_push_sorted(2, args);
        // Set calling task's data to this task that it waits on, to double-link it.
        ((mp_obj_task_t *)MP_OBJ_TO_PTR(cur_task))->data = self_in;
    }
    return mp_const_none;
}

STATIC const mp_obj_type_t task_type = {
    { &mp_type_type },
    .name = MP_QSTR_Task,
    .make_new = task_make_new,
    .attr = task_attr,
    .getiter = task_getiter,
    .iternext = task_iternext,
};

/******************************************************************************/
// C-level uasyncio module

STATIC const mp_rom_map_elem_t mp_module_uasyncio_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR__uasyncio) },
    { MP_ROM_QSTR(MP_QSTR_TaskQueue), MP_ROM_PTR(&task_queue_type) },
    { MP_ROM_QSTR(MP_QSTR_Task), MP_ROM_PTR(&task_type) },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_uasyncio_globals, mp_module_uasyncio_globals_table);

const mp_obj_module_t mp_module_uasyncio = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_uasyncio_globals,
};

#endif // MICROPY_PY_UASYNCIO
