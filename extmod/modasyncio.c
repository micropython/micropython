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

#if MICROPY_PY_ASYNCIO

// Used when task cannot be guaranteed to be non-NULL.
#define TASK_PAIRHEAP(task) ((task) ? &(task)->pairheap : NULL)

#define TASK_STATE_RUNNING_NOT_WAITED_ON (mp_const_true)
#define TASK_STATE_DONE_NOT_WAITED_ON (mp_const_none)
#define TASK_STATE_DONE_WAS_WAITED_ON (mp_const_false)

#define TASK_IS_DONE(task) ( \
    (task)->state == TASK_STATE_DONE_NOT_WAITED_ON \
    || (task)->state == TASK_STATE_DONE_WAS_WAITED_ON)

typedef struct _mp_obj_task_t {
    mp_pairheap_t pairheap;
    mp_obj_t coro;
    mp_obj_t data;
    mp_obj_t state;
    mp_obj_t ph_key;
} mp_obj_task_t;

typedef struct _mp_obj_task_queue_t {
    mp_obj_base_t base;
    mp_obj_task_t *heap;
    #if MICROPY_PY_ASYNCIO_TASK_QUEUE_PUSH_CALLBACK
    mp_obj_t push_callback;
    #endif
} mp_obj_task_queue_t;

static const mp_obj_type_t task_queue_type;
static const mp_obj_type_t task_type;

static mp_obj_t task_queue_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args);

/******************************************************************************/
// Ticks for task ordering in pairing heap

static mp_obj_t ticks(void) {
    return MP_OBJ_NEW_SMALL_INT(mp_hal_ticks_ms() & (MICROPY_PY_TIME_TICKS_PERIOD - 1));
}

static mp_int_t ticks_diff(mp_obj_t t1_in, mp_obj_t t0_in) {
    mp_uint_t t0 = MP_OBJ_SMALL_INT_VALUE(t0_in);
    mp_uint_t t1 = MP_OBJ_SMALL_INT_VALUE(t1_in);
    mp_int_t diff = ((t1 - t0 + MICROPY_PY_TIME_TICKS_PERIOD / 2) & (MICROPY_PY_TIME_TICKS_PERIOD - 1))
        - MICROPY_PY_TIME_TICKS_PERIOD / 2;
    return diff;
}

static int task_lt(mp_pairheap_t *n1, mp_pairheap_t *n2) {
    mp_obj_task_t *t1 = (mp_obj_task_t *)n1;
    mp_obj_task_t *t2 = (mp_obj_task_t *)n2;
    return MP_OBJ_SMALL_INT_VALUE(ticks_diff(t1->ph_key, t2->ph_key)) < 0;
}

/******************************************************************************/
// TaskQueue class

static mp_obj_t task_queue_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    (void)args;
    mp_arg_check_num(n_args, n_kw, 0, MICROPY_PY_ASYNCIO_TASK_QUEUE_PUSH_CALLBACK ? 1 : 0, false);
    mp_obj_task_queue_t *self = mp_obj_malloc(mp_obj_task_queue_t, type);
    self->heap = (mp_obj_task_t *)mp_pairheap_new(task_lt);
    #if MICROPY_PY_ASYNCIO_TASK_QUEUE_PUSH_CALLBACK
    if (n_args == 1) {
        self->push_callback = args[0];
    } else {
        self->push_callback = MP_OBJ_NULL;
    }
    #endif
    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t task_queue_peek(mp_obj_t self_in) {
    mp_obj_task_queue_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->heap == NULL) {
        return mp_const_none;
    } else {
        return MP_OBJ_FROM_PTR(self->heap);
    }
}
static MP_DEFINE_CONST_FUN_OBJ_1(task_queue_peek_obj, task_queue_peek);

static mp_obj_t task_queue_push(size_t n_args, const mp_obj_t *args) {
    mp_obj_task_queue_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_obj_task_t *task = MP_OBJ_TO_PTR(args[1]);
    task->data = mp_const_none;
    if (n_args == 2) {
        task->ph_key = ticks();
    } else {
        assert(mp_obj_is_small_int(args[2]));
        task->ph_key = args[2];
    }
    self->heap = (mp_obj_task_t *)mp_pairheap_push(task_lt, TASK_PAIRHEAP(self->heap), TASK_PAIRHEAP(task));
    #if MICROPY_PY_ASYNCIO_TASK_QUEUE_PUSH_CALLBACK
    if (self->push_callback != MP_OBJ_NULL) {
        mp_call_function_1(self->push_callback, MP_OBJ_NEW_SMALL_INT(0));
    }
    #endif
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(task_queue_push_obj, 2, 3, task_queue_push);

static mp_obj_t task_queue_pop(mp_obj_t self_in) {
    mp_obj_task_queue_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_task_t *head = (mp_obj_task_t *)mp_pairheap_peek(task_lt, &self->heap->pairheap);
    if (head == NULL) {
        mp_raise_msg(&mp_type_IndexError, MP_ERROR_TEXT("empty heap"));
    }
    self->heap = (mp_obj_task_t *)mp_pairheap_pop(task_lt, &self->heap->pairheap);
    return MP_OBJ_FROM_PTR(head);
}
static MP_DEFINE_CONST_FUN_OBJ_1(task_queue_pop_obj, task_queue_pop);

static mp_obj_t task_queue_remove(mp_obj_t self_in, mp_obj_t task_in) {
    mp_obj_task_queue_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_task_t *task = MP_OBJ_TO_PTR(task_in);
    self->heap = (mp_obj_task_t *)mp_pairheap_delete(task_lt, &self->heap->pairheap, &task->pairheap);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(task_queue_remove_obj, task_queue_remove);

static const mp_rom_map_elem_t task_queue_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_peek), MP_ROM_PTR(&task_queue_peek_obj) },
    { MP_ROM_QSTR(MP_QSTR_push), MP_ROM_PTR(&task_queue_push_obj) },
    { MP_ROM_QSTR(MP_QSTR_pop), MP_ROM_PTR(&task_queue_pop_obj) },
    { MP_ROM_QSTR(MP_QSTR_remove), MP_ROM_PTR(&task_queue_remove_obj) },
};
static MP_DEFINE_CONST_DICT(task_queue_locals_dict, task_queue_locals_dict_table);

static MP_DEFINE_CONST_OBJ_TYPE(
    task_queue_type,
    MP_QSTR_TaskQueue,
    MP_TYPE_FLAG_NONE,
    make_new, task_queue_make_new,
    locals_dict, &task_queue_locals_dict
    );

/******************************************************************************/
// Task class

// This is the core asyncio context with cur_task, _task_queue and CancelledError.
mp_obj_t mp_asyncio_context = MP_OBJ_NULL;

static mp_obj_t task_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 2, false);
    mp_obj_task_t *self = m_new_obj(mp_obj_task_t);
    self->pairheap.base.type = type;
    mp_pairheap_init_node(task_lt, &self->pairheap);
    self->coro = args[0];
    self->data = mp_const_none;
    self->state = TASK_STATE_RUNNING_NOT_WAITED_ON;
    self->ph_key = MP_OBJ_NEW_SMALL_INT(0);
    if (n_args == 2) {
        mp_asyncio_context = args[1];
    }
    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t task_done(mp_obj_t self_in) {
    mp_obj_task_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(TASK_IS_DONE(self));
}
static MP_DEFINE_CONST_FUN_OBJ_1(task_done_obj, task_done);

static mp_obj_t task_cancel(mp_obj_t self_in) {
    mp_obj_task_t *self = MP_OBJ_TO_PTR(self_in);
    // Check if task is already finished.
    if (TASK_IS_DONE(self)) {
        return mp_const_false;
    }
    // Can't cancel self (not supported yet).
    mp_obj_t cur_task = mp_obj_dict_get(mp_asyncio_context, MP_OBJ_NEW_QSTR(MP_QSTR_cur_task));
    if (self_in == cur_task) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("can't cancel self"));
    }
    // If Task waits on another task then forward the cancel to the one it's waiting on.
    while (mp_obj_is_subclass_fast(MP_OBJ_FROM_PTR(mp_obj_get_type(self->data)), MP_OBJ_FROM_PTR(&task_type))) {
        self = MP_OBJ_TO_PTR(self->data);
    }

    mp_obj_t _task_queue = mp_obj_dict_get(mp_asyncio_context, MP_OBJ_NEW_QSTR(MP_QSTR__task_queue));

    // Reschedule Task as a cancelled task.
    mp_obj_t dest[3];
    mp_load_method_maybe(self->data, MP_QSTR_remove, dest);
    if (dest[0] != MP_OBJ_NULL) {
        // Not on the main running queue, remove the task from the queue it's on.
        dest[2] = MP_OBJ_FROM_PTR(self);
        mp_call_method_n_kw(1, 0, dest);
        // _task_queue.push(self)
        dest[0] = _task_queue;
        dest[1] = MP_OBJ_FROM_PTR(self);
        task_queue_push(2, dest);
    } else if (ticks_diff(self->ph_key, ticks()) > 0) {
        // On the main running queue but scheduled in the future, so bring it forward to now.
        // _task_queue.remove(self)
        task_queue_remove(_task_queue, MP_OBJ_FROM_PTR(self));
        // _task_queue.push(self)
        dest[0] = _task_queue;
        dest[1] = MP_OBJ_FROM_PTR(self);
        task_queue_push(2, dest);
    }

    self->data = mp_obj_dict_get(mp_asyncio_context, MP_OBJ_NEW_QSTR(MP_QSTR_CancelledError));

    return mp_const_true;
}
static MP_DEFINE_CONST_FUN_OBJ_1(task_cancel_obj, task_cancel);

static void task_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    mp_obj_task_t *self = MP_OBJ_TO_PTR(self_in);
    if (dest[0] == MP_OBJ_NULL) {
        // Load
        if (attr == MP_QSTR_coro) {
            dest[0] = self->coro;
        } else if (attr == MP_QSTR_data) {
            dest[0] = self->data;
        } else if (attr == MP_QSTR_state) {
            dest[0] = self->state;
        } else if (attr == MP_QSTR_done) {
            dest[0] = MP_OBJ_FROM_PTR(&task_done_obj);
            dest[1] = self_in;
        } else if (attr == MP_QSTR_cancel) {
            dest[0] = MP_OBJ_FROM_PTR(&task_cancel_obj);
            dest[1] = self_in;
        } else if (attr == MP_QSTR_ph_key) {
            dest[0] = self->ph_key;
        }
    } else if (dest[1] != MP_OBJ_NULL) {
        // Store
        if (attr == MP_QSTR_data) {
            self->data = dest[1];
            dest[0] = MP_OBJ_NULL;
        } else if (attr == MP_QSTR_state) {
            self->state = dest[1];
            dest[0] = MP_OBJ_NULL;
        }
    }
}

static mp_obj_t task_getiter(mp_obj_t self_in, mp_obj_iter_buf_t *iter_buf) {
    (void)iter_buf;
    mp_obj_task_t *self = MP_OBJ_TO_PTR(self_in);
    if (TASK_IS_DONE(self)) {
        // Signal that the completed-task has been await'ed on.
        self->state = TASK_STATE_DONE_WAS_WAITED_ON;
    } else if (self->state == TASK_STATE_RUNNING_NOT_WAITED_ON) {
        // Allocate the waiting queue.
        self->state = task_queue_make_new(&task_queue_type, 0, 0, NULL);
    } else if (mp_obj_get_type(self->state) != &task_queue_type) {
        // Task has state used for another purpose, so can't also wait on it.
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("can't wait"));
    }
    return self_in;
}

static mp_obj_t task_iternext(mp_obj_t self_in) {
    mp_obj_task_t *self = MP_OBJ_TO_PTR(self_in);
    if (TASK_IS_DONE(self)) {
        // Task finished, raise return value to caller so it can continue.
        nlr_raise(self->data);
    } else {
        // Put calling task on waiting queue.
        mp_obj_t cur_task = mp_obj_dict_get(mp_asyncio_context, MP_OBJ_NEW_QSTR(MP_QSTR_cur_task));
        mp_obj_t args[2] = { self->state, cur_task };
        task_queue_push(2, args);
        // Set calling task's data to this task that it waits on, to double-link it.
        ((mp_obj_task_t *)MP_OBJ_TO_PTR(cur_task))->data = self_in;
    }
    return mp_const_none;
}

static const mp_getiter_iternext_custom_t task_getiter_iternext = {
    .getiter = task_getiter,
    .iternext = task_iternext,
};

static MP_DEFINE_CONST_OBJ_TYPE(
    task_type,
    MP_QSTR_Task,
    MP_TYPE_FLAG_ITER_IS_CUSTOM,
    make_new, task_make_new,
    attr, task_attr,
    iter, &task_getiter_iternext
    );

/******************************************************************************/
// C-level asyncio module

static const mp_rom_map_elem_t mp_module_asyncio_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR__asyncio) },
    { MP_ROM_QSTR(MP_QSTR_TaskQueue), MP_ROM_PTR(&task_queue_type) },
    { MP_ROM_QSTR(MP_QSTR_Task), MP_ROM_PTR(&task_type) },
};
static MP_DEFINE_CONST_DICT(mp_module_asyncio_globals, mp_module_asyncio_globals_table);

const mp_obj_module_t mp_module_asyncio = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_asyncio_globals,
};

MP_REGISTER_MODULE(MP_QSTR__asyncio, mp_module_asyncio);

#endif // MICROPY_PY_ASYNCIO
