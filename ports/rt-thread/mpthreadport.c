/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Armink (armink.ztl@gmail.com)
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

#include "stdio.h"

#include "py/mpconfig.h"
#include "py/mpstate.h"
#include "py/gc.h"
#include "py/mpthread.h"
#include "mpthreadport.h"

#include <rthw.h>

#if MICROPY_PY_THREAD

#define MP_THREAD_MIN_STACK_SIZE                 (4 * 1024)
#define MP_THREAD_DEFAULT_STACK_SIZE             (MP_THREAD_MIN_STACK_SIZE + 1024)

#define MP_THREAD_STATUS_READY                   0
#define MP_THREAD_STATUS_RUNNING                 1
#define MP_THREAD_STATUS_FINISH                  2

typedef struct _thread_t
{
    rt_thread_t id;         // system id of thread
    int status;              // whether the thread is ready, running and finish
    void *arg;              // thread Python args, a GC root pointer
    void *stack;            // pointer to the stack
    size_t stack_len;       // number of words in the stack
    struct _thread_t *next;
} thread_t;

// the mutex controls access to the linked list
STATIC mp_thread_mutex_t thread_mutex;
STATIC thread_t thread_root_node;
STATIC thread_t *thread_root; // root pointer, handled by mp_thread_gc_others

/**
 * thread port initialization
 *
 * @param stack MicroPython main thread stack start address
 * @param stack_len number of words in the stack
 */
void mp_thread_init(void *stack, uint32_t stack_len)
{
    mp_thread_set_state(&mp_state_ctx.thread);

    thread_root = &thread_root_node;
    thread_root->id = rt_thread_self();
    thread_root->status = MP_THREAD_STATUS_RUNNING;
    thread_root->arg = NULL;
    thread_root->stack = stack;
    thread_root->stack_len = stack_len;
    thread_root->next = NULL;

    mp_thread_mutex_init(&thread_mutex);
}

void mp_thread_gc_others(void)
{

    mp_thread_mutex_lock(&thread_mutex, 1);
    for (thread_t *th = thread_root; th != NULL; th = th->next)
    {
        // the root node not using the mpy heap
        if (th != &thread_root_node)
        {
            gc_collect_root((void **)&th, 1);
            gc_collect_root(&th->arg, 1); // probably not needed
        }

        if (th->status == MP_THREAD_STATUS_READY)
        {
            continue;
        }

        gc_collect_root((void **) &th->id, 1); // probably not needed
        gc_collect_root(th->stack, th->stack_len); // probably not needed
    }
    mp_thread_mutex_unlock(&thread_mutex);
}

mp_state_thread_t *mp_thread_get_state(void)
{
    return (mp_state_thread_t *)(rt_thread_self()->user_data);
}

void mp_thread_set_state(mp_state_thread_t *state)
{
    rt_thread_self()->user_data = (rt_uint32_t)state;
}

void mp_thread_start(void)
{
    mp_thread_mutex_lock(&thread_mutex, 1);
    for (thread_t *th = thread_root; th != NULL; th = th->next)
    {
        if (th->id == rt_thread_self())
        {
            th->status = MP_THREAD_STATUS_RUNNING;
            break;
        }
    }
    mp_thread_mutex_unlock(&thread_mutex);
}

void mp_thread_create_ex(void *(*entry)(void *), void *arg, size_t *stack_size, int priority, char *name)
{
    static uint8_t count = 0;

    if (*stack_size == 0)
    {
        *stack_size = MP_THREAD_DEFAULT_STACK_SIZE; // default stack size
    }
    else if (*stack_size < MP_THREAD_MIN_STACK_SIZE)
    {
        *stack_size = MP_THREAD_MIN_STACK_SIZE; // minimum stack size
    }

    // allocate the linked-list node, TCB and stack (must be outside thread_mutex lock)
    thread_t *th = m_new_obj(thread_t);
    if (th == NULL)
    {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "can't create thread obj"));
    }
    th->id = m_new_obj(struct rt_thread);
    if (th->id == NULL)
    {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "can't create thread id"));
    }
    th->stack = m_new(uint8_t, *stack_size);
    if (th->stack == NULL)
    {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "can't create thread stack"));
    }

    mp_thread_mutex_lock(&thread_mutex, 1);

    // adjust the stack_size to provide room to recover from hitting the limit
    *stack_size -= 1024;

    // add thread to linked list of all threads
    th->status = MP_THREAD_STATUS_READY;
    th->arg = arg;
    th->stack_len = *stack_size / 4;
    th->next = thread_root;
    thread_root = th;

    rt_thread_init(th->id, name, (void (*)(void *))entry, arg, th->stack, *stack_size, priority, count++);
    rt_thread_startup(th->id);

    mp_thread_mutex_unlock(&thread_mutex);
}

void mp_thread_create(void *(*entry)(void *), void *arg, size_t *stack_size)
{
    static uint8_t count = 0;
    int priority = rt_thread_self()->current_priority;
    char name[RT_NAME_MAX];

    if (priority > 0)
    {
        priority --;
    }

    /* build name */
    rt_snprintf(name, sizeof(name), "mp%02d", count++);

    mp_thread_create_ex(entry, arg, stack_size, priority, name);
}

void mp_thread_finish(void)
{
    thread_t *prev = NULL;

    mp_thread_mutex_lock(&thread_mutex, 1);
    for (thread_t *th = thread_root; th != NULL; prev = th, th = th->next)
    {
        // unlink the node from the list
        if (th->id == rt_thread_self())
        {

            if (prev != NULL)
            {
                prev->next = th->next;
            }
            else
            {
                // move the start pointer
                thread_root = th->next;
            }
            th->status = MP_THREAD_STATUS_FINISH;
            // explicitly release all its memory
            m_del_obj(struct rt_thread, th->id);
            m_del(uint8_t, th->stack, th->stack_len);
            m_del_obj(thread_t, th);
            break;
        }
    }
    mp_thread_mutex_unlock(&thread_mutex);
}

void mp_thread_mutex_init(mp_thread_mutex_t *mutex)
{
    static uint8_t count = 0;
    char name[RT_NAME_MAX];

    if (!mutex->is_init)
    {
        /* build name */
        rt_snprintf(name, sizeof(name), "mp%02d", count++);
        rt_mutex_init(&(mutex->mutex), name, RT_IPC_FLAG_FIFO);
        mutex->is_init = 1;
    }
}

int mp_thread_mutex_lock(mp_thread_mutex_t *mutex, int wait)
{
    return (RT_EOK == rt_mutex_take(&(mutex->mutex), wait ? RT_WAITING_FOREVER : 0));
}

void mp_thread_mutex_unlock(mp_thread_mutex_t *mutex)
{
    rt_mutex_release(&(mutex->mutex));
}

void mp_thread_deinit(void)
{
    // detach all ready and running mpy thread
    for (thread_t *th = thread_root; th != NULL; th = th->next)
    {
        if (th != &thread_root_node && th->status != MP_THREAD_STATUS_FINISH)
        {
            rt_thread_detach(th->id);
        }
    }

    // allow RT-Thread to clean-up the threads
    rt_thread_delay(200);
}

#endif /* MICROPY_PY_THREAD */
