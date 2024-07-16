/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Damien P. George on behalf of Pycom Ltd
 * Copyright (c) 2017 Pycom Limited
 * Copyright (c) 2024 Daniel Campora on behalf of REMOTE TECH LTD
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

#include "py/runtime.h"
#include "py/gc.h"
#include "py/mpthread.h"
#include "py/mphal.h"

#if MICROPY_PY_THREAD

#define DEBUG_printf(...) // printk("_thread: " __VA_ARGS__)

#define MP_THREAD_MIN_STACK_SIZE                        (4 * 1024)
#define MP_THREAD_DEFAULT_STACK_SIZE                    (MP_THREAD_MIN_STACK_SIZE + 1024)
#define MP_THREAD_PRIORITY                              (k_thread_priority_get(k_current_get()))    // same priority as the main thread
#define MP_THREAD_MAXIMUM_USER_THREADS                  (4)

typedef enum {
    MP_THREAD_STATUS_CREATED = 0,
    MP_THREAD_STATUS_READY,
    MP_THREAD_STATUS_FINISHED,
} mp_thread_status_t;

typedef struct _mp_thread_slot_t {
    bool used;
} mp_thread_stack_slot_t;

// this structure forms a linked list, one node per active thread
typedef struct _mp_thread_t {
    k_tid_t id;                 // system id of thread (this is actually a pointer to z_thread below)
    struct k_thread z_thread;   // the zephyr thread object
    mp_thread_status_t status;  // whether the thread is created, ready, or finished
    int16_t alive;              // whether the thread is still visible by the kernel
    int16_t slot;               // slot index in the stack pool
    void *arg;                  // thread Python args, a GC root pointer
    void *stack;                // pointer to the stack
    size_t stack_len;           // number of words in the stack
    struct _mp_thread_t *next;
} mp_thread_t;

// the mutex controls access to the linked list
static mp_thread_mutex_t thread_mutex;
static mp_thread_t thread_entry0;
static mp_thread_t *thread = NULL; // root pointer, handled by mp_thread_gc_others
static uint8_t mp_thread_counter;
static mp_thread_stack_slot_t stack_slot[MP_THREAD_MAXIMUM_USER_THREADS];

K_THREAD_STACK_ARRAY_DEFINE(mp_thread_stack_array, MP_THREAD_MAXIMUM_USER_THREADS, MP_THREAD_DEFAULT_STACK_SIZE);

static void mp_thread_iterate_threads_cb(const struct k_thread *thread, void *user_data);
static int32_t mp_thread_find_stack_slot(void);

void mp_thread_init(void *stack, uint32_t stack_len) {
    mp_thread_set_state(&mp_state_ctx.thread);
    // create the first entry in the linked list of all threads
    thread_entry0.id = k_current_get();
    thread_entry0.status = MP_THREAD_STATUS_READY;
    thread_entry0.alive = 1;
    thread_entry0.arg = NULL;
    thread_entry0.stack = stack;
    thread_entry0.stack_len = stack_len;
    thread_entry0.next = NULL;
    k_thread_name_set(thread_entry0.id, "mp_main");
    mp_thread_counter = 0;
    mp_thread_mutex_init(&thread_mutex);

    // memory barrier to ensure above data is committed
    __sync_synchronize();

    thread = &thread_entry0;
}

void mp_thread_gc_others(void) {
    mp_thread_t *prev = NULL;

    if (thread == NULL) {
        // threading not yet initialised
        return;
    }

    mp_thread_mutex_lock(&thread_mutex, 1);

    // get the kernel to iterate over all the existing threads
    DEBUG_printf("Iterating...\n");
    k_thread_foreach(mp_thread_iterate_threads_cb, NULL);
    for (mp_thread_t *th = thread; th != NULL; th = th->next) {
        // unlink non-alive thread nodes from the list
        if ((th->status == MP_THREAD_STATUS_FINISHED) && !th->alive) {
            if (prev != NULL) {
                prev->next = th->next;
            } else {
                // move the start pointer
                thread = th->next;
            }
            stack_slot[th->slot].used = false;
            mp_thread_counter--;
            DEBUG_printf("Collecting thread %s\n", k_thread_name_get(th->id));
            // The "th" memory will eventually be reclaimed by the GC
        } else {
            th->alive = 0;
            prev = th;
        }
    }

    DEBUG_printf("mp_thread_gc_others from %s\n", k_thread_name_get(k_current_get()));

    for (mp_thread_t *th = thread; th != NULL; th = th->next) {
        DEBUG_printf("%s\n", k_thread_name_get(th->id));
        gc_collect_root((void **)&th, 1);
        gc_collect_root(&th->arg, 1);
        // gc_collect_root(&th->stack, 1);  // will be needed later when the stack is allocated from the gc heap
        if (th->id == k_current_get()) {
            continue;
        }
        if (th->status != MP_THREAD_STATUS_READY) {
            continue;
        }
        gc_collect_root(th->stack, th->stack_len);
    }
    mp_thread_mutex_unlock(&thread_mutex);
}

mp_state_thread_t *mp_thread_get_state(void) {
    return (mp_state_thread_t *)k_thread_custom_data_get();
}

void mp_thread_set_state(mp_state_thread_t *state) {
    k_thread_custom_data_set((void *)state);
}

mp_uint_t mp_thread_get_id(void) {
    return (mp_uint_t)k_current_get();
}

void mp_thread_start(void) {
    mp_thread_mutex_lock(&thread_mutex, 1);
    for (mp_thread_t *th = thread; th != NULL; th = th->next) {
        if (th->id == k_current_get()) {
            th->status = MP_THREAD_STATUS_READY;
            break;
        }
    }
    mp_thread_mutex_unlock(&thread_mutex);
}

static void zephyr_entry(void *arg1, void *arg2, void *arg3) {
    (void)arg3;

    // arg1 contains the python thread entry point
    if (arg1) {
        void *(*entry)(void *) = arg1;
        entry(arg2);
    }
    k_thread_abort(k_current_get());
    for (;;) {;
    }
}

mp_uint_t mp_thread_create_ex(void *(*entry)(void *), void *arg, size_t *stack_size, int priority, char *name) {
    // TODO: we need to support for CONFIG_DYNAMIC_THREAD in order to dynamically create allocate the stack of a thread
    // if (*stack_size == 0) {
    //     *stack_size = MP_THREAD_DEFAULT_STACK_SIZE; // default stack size
    // } else if (*stack_size < MP_THREAD_MIN_STACK_SIZE) {
    //     *stack_size = MP_THREAD_MIN_STACK_SIZE; // minimum stack size
    // }

    // in case some threads have finished but their stack has not been collected yet
    gc_collect();

    // Allocate linked-list node (must be outside thread_mutex lock)
    mp_thread_t *th = m_new_obj(mp_thread_t);

    mp_thread_mutex_lock(&thread_mutex, 1);

    int32_t _slot = mp_thread_find_stack_slot();
    if (_slot >= 0) {
        // create thread
        th->id = k_thread_create(&th->z_thread, mp_thread_stack_array[_slot], K_THREAD_STACK_SIZEOF(mp_thread_stack_array[_slot]),
            zephyr_entry, entry, arg, NULL, priority, 0, K_NO_WAIT);

        if (th->id == NULL) {
            mp_thread_mutex_unlock(&thread_mutex);
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("can't create thread"));
        }

        k_thread_name_set(th->id, (const char *)name);

    } else {
        mp_thread_mutex_unlock(&thread_mutex);
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("maximum number of threads reached"));
    }

    // add thread to linked list of all threads
    th->status = MP_THREAD_STATUS_CREATED;
    th->alive = 0;
    th->slot = _slot;
    th->arg = arg;
    th->stack = (void *)th->z_thread.stack_info.start;
    th->stack_len = th->z_thread.stack_info.size / sizeof(uintptr_t);
    th->next = thread;
    thread = th;

    stack_slot[_slot].used = true;
    mp_thread_counter++;

    // adjust the stack_size to provide room to recover from hitting the limit
    *stack_size = th->z_thread.stack_info.size - 1024;

    mp_thread_mutex_unlock(&thread_mutex);

    return (mp_uint_t)th->id;
}

mp_uint_t mp_thread_create(void *(*entry)(void *), void *arg, size_t *stack_size) {
    char _name[16];
    snprintf(_name, sizeof(_name), "mp_thread_%d", mp_thread_counter);
    return mp_thread_create_ex(entry, arg, stack_size, MP_THREAD_PRIORITY, _name);
}

void mp_thread_finish(void) {
    mp_thread_mutex_lock(&thread_mutex, 1);
    for (mp_thread_t *th = thread; th != NULL; th = th->next) {
        if (th->id == k_current_get()) {
            th->status = MP_THREAD_STATUS_FINISHED;
            DEBUG_printf("Finishing thread %s\n", k_thread_name_get(th->id));
            break;
        }
    }
    mp_thread_mutex_unlock(&thread_mutex);
}

void mp_thread_mutex_init(mp_thread_mutex_t *mutex) {
    // Need a binary semaphore so a lock can be acquired on one Python thread
    // and then released on another.
    k_sem_init(&mutex->handle, 0, 1);
    k_sem_give(&mutex->handle);
}

int mp_thread_mutex_lock(mp_thread_mutex_t *mutex, int wait) {
    return k_sem_take(&mutex->handle, wait ? K_FOREVER : K_NO_WAIT) == 0;
}

void mp_thread_mutex_unlock(mp_thread_mutex_t *mutex) {
    k_sem_give(&mutex->handle);
    k_yield();
}

void mp_thread_deinit(void) {
    // abort all threads except for the main thread
    mp_thread_mutex_lock(&thread_mutex, 1);
    for (mp_thread_t *th = thread; th != NULL; th = th->next) {
        // don't delete the current task
        if ((th->id != k_current_get()) && (th->status != MP_THREAD_STATUS_FINISHED)) {
            th->status = MP_THREAD_STATUS_FINISHED;
            DEBUG_printf("De-initializing thread %s\n", k_thread_name_get(th->id));
            k_thread_abort(th->id);
        }
    }
    mp_thread_mutex_unlock(&thread_mutex);
}

static void mp_thread_iterate_threads_cb(const struct k_thread *z_thread, void *user_data) {
    for (mp_thread_t *th = thread; th != NULL; th = th->next) {
        if (th->id == (struct k_thread *)z_thread) {
            th->alive = 1;
            DEBUG_printf("Found thread %s\n", k_thread_name_get(th->id));
        }
    }
}

static int32_t mp_thread_find_stack_slot(void) {
    for (int i = 0; i < MP_THREAD_MAXIMUM_USER_THREADS; i++) {
        if (!stack_slot[i].used) {
            DEBUG_printf("Allocating stack slot %d\n", i);
            return i;
        }
    }
    return -1;
}

#endif // MICROPY_PY_THREAD
