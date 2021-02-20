/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019, 2020 Yonatan Goldschmidt
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

#define pr_fmt(fmt) "MPY: thread: " fmt

#include <linux/list.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/kthread.h>

#include "py/mpconfig.h"

#if MICROPY_PY_THREAD

#include "internal.h"

#include "py/runtime.h"
#include "py/mpthread.h"
#include "py/gc.h"

typedef struct _thread {
    struct task_struct *owner;
    mp_state_thread_t *ts;
    void *arg; // thread Python args, a GC root pointer
    struct list_head list;
} thread_t; // thread_t, like other ports

STATIC LIST_HEAD(threads);
STATIC DEFINE_RWLOCK(threads_lock);

void mp_thread_gc_others(void) {
    read_lock(&threads_lock);

    thread_t *it;
    list_for_each_entry(it, &threads, list) {
        // iterate root pointers for all threads

        if (it->owner == current) {
            // this is mp_thread_gc_*others*, not current :)
            continue;
        }

        // thread argument
        gc_collect_root(&it->arg, 1);

        // thread stack - we traverse the full stack from the top, because we don't know where Python
        // currently is (no feasible way to access the current stack pointer).
        // stack_top is set by micropython core, but there's a bit of a race here, so check for NULL.
        if (it->ts != NULL && it->ts->stack_top != NULL) {
            unsigned long stack_top = (unsigned long)it->ts->stack_top;
            // stack limit may still be 0, see mp_thread_create. it's okay.
            unsigned long stack_bottom = stack_top - it->ts->stack_limit;

            gc_collect_root((void**)stack_bottom, (stack_top - stack_bottom) / sizeof(void*));
        }

        // TODO: registers? we can check if task is currently running, and if not,
        // collect from its task_pt_regs. can also be applied to get the stack end accurately.
    }

    read_unlock(&threads_lock);
}

thread_t *__get_thread_for_current(void) {
    read_lock(&threads_lock);

    thread_t *it;
    thread_t *found = NULL;
    list_for_each_entry(it, &threads, list) {
        if (it->owner == current) {
            found = it;
            break;
        }
    }

    // okay to reference 'found' outside of the lock context - since this is the *current* thread, only current
    // context can eventually kill it...
    read_unlock(&threads_lock);

    return found;
}

static thread_t *get_thread_for_current(void) {
    thread_t *t = __get_thread_for_current();

    if (t == NULL) {
        // shouldn't happen, really...
        die("can't find MP thread for current!");
    }

    return t;
}

static thread_t *get_and_remove_thread(void) {
    thread_t *t = get_thread_for_current();

    unsigned long flags;
    write_lock_irqsave(&threads_lock, flags);
    list_del(&t->list);
    write_unlock_irqrestore(&threads_lock, flags);

    return t;
}

mp_state_thread_t *mp_thread_get_state(void) {
    return get_thread_for_current()->ts;
}

void mp_thread_set_state(struct _mp_state_thread_t *state) {
    get_thread_for_current()->ts = state;
}

// called by micropython core to signal that the thread is starting
void mp_thread_start(void) {
    // override stack_limit set by thread_entry after it calles mp_thread_set_state.
    set_stack_limit();
}

bool __register_new_thread(struct task_struct *k, void *arg, bool pythread, void *ts) {
    thread_t *t = (thread_t*)kmalloc(sizeof(*t), GFP_ATOMIC);
    if (!t) {
        if (pythread) {
            kthread_stop(k);
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError, "kmalloc failed"));
        } else {
            return false;
        }
    }

    t->owner = k;
    t->ts = (mp_state_thread_t*)ts; // may be NULL for pythreads, see mp_thread_gc_others
    t->arg = arg;

    unsigned long flags;
    write_lock_irqsave(&threads_lock, flags);
    list_add_tail(&t->list, &threads);
    write_unlock_irqrestore(&threads_lock, flags);

    return true;
}

void mp_thread_create(void *(*entry)(void*), void *arg, size_t *stack_size) {
    static atomic_t thread_count = ATOMIC_INIT(0);

    // micropython's thread_entry returns NULL ( == 0), this case is okay as an int.
    int (*kthread_entry)(void *) = (int(*)(void*))entry;

    struct task_struct *k = kthread_create(kthread_entry, arg, "kmpy-%d", atomic_inc_return(&thread_count));
    if (IS_ERR(k)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError, "kthread_create: %ld", PTR_ERR(k)));
    }

    register_new_pythread(k, arg);

    // we don't control the kthread stack size. so instead, we ignore the value requested by MPY
    // and modify it to 0. MPY will later use this value to call mp_stack_set_limit. lastly, just before
    // the thread starts running Python, mp_thread_start is called and it will update thee stack limit
    // acoording to the amount of stack left for the kthread.
    // this 0 also acts as kind of posioned value - if we forget to modify it later, then
    // the first mp_stack_check will fail.
    *stack_size = 0;

    wake_up_process(k);
}

void mp_thread_finish(void) {
    thread_t *t = get_and_remove_thread();
    kfree(t);
}

// called by main thread when we go down.
void mp_thread_deinit(void) {
    // TODO kill all threads

    mp_thread_finish();
}

void mp_thread_init(void) {
    if (!register_new_context(&mp_state_ctx.thread)) {
        die("register_new_context failed for main thread!");
    }
}

#endif // MICROPY_PY_THREAD
