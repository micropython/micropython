/**
 * @file module_thread.c
 *
 * @section License
 * Copyright (C) 2016, Erik Moqvist
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * This file is part of the Pumbaa project.
 */

#include "pumbaa.h"

/**
 * This structure forms a linked list, one node per active thread.
 */
struct thread_t {
    struct thrd_t *thrd_p;
    int ready;
    void *state_p;
    void *stack_p;
    struct thread_t *next_p;
};

/* The mutex controls access to the linked list of threads. */
static mp_thread_mutex_t thread_mutex;
static struct thread_t thread_entry0;
static struct thread_t *threads_p;

void module_thread_init(void)
{
    mp_thread_mutex_init(&thread_mutex);

    /* Create first entry in linked list of all threads. */
    thread_entry0.thrd_p = thrd_self();
    thread_entry0.ready = 0;
    thread_entry0.state_p = &mp_state_ctx.thread;
    thread_entry0.next_p = NULL;
    threads_p = &thread_entry0;
}

void mp_thread_gc_others(void)
{
    struct thread_t *thread_p;

    mp_thread_mutex_lock(&thread_mutex, 1);

    thread_p = threads_p;

    while (thread_p != NULL) {
        if (thread_p->thrd_p != thrd_self()) {
            if (thread_p->ready == 1) {
                gc_collect_root((void**)&thread_p, 1);
            }
        }

        thread_p = thread_p->next_p;
    }

    mp_thread_mutex_unlock(&thread_mutex);
}

mp_state_thread_t *mp_thread_get_state(void)
{
    struct thread_t *thread_p;

    mp_thread_mutex_lock(&thread_mutex, 1);

    thread_p = threads_p;

    while (thread_p != NULL) {
        if (thread_p->thrd_p == thrd_self()) {
            break;
        }

        thread_p = thread_p->next_p;
    }

    mp_thread_mutex_unlock(&thread_mutex);

    return (thread_p->state_p);
}

void mp_thread_set_state(void *state_p)
{
    struct thread_t *thread_p;

    mp_thread_mutex_lock(&thread_mutex, 1);

    thread_p = threads_p;

    while (thread_p != NULL) {
        if (thread_p->thrd_p == thrd_self()) {
            break;
        }

        thread_p = thread_p->next_p;
    }

    mp_thread_mutex_unlock(&thread_mutex);

    thread_p->state_p = state_p;
}

void mp_thread_start(void)
{
    struct thread_t *thread_p;

    mp_thread_mutex_lock(&thread_mutex, 1);

    thread_p = threads_p;

    while (thread_p != NULL) {
        if (thread_p->thrd_p == thrd_self()) {
            thread_p->ready = 1;
            break;
        }

        thread_p = thread_p->next_p;
    }

    mp_thread_mutex_unlock(&thread_mutex);
}

void mp_thread_create(void *(*entry)(void*),
                      void *arg_p,
                      size_t *stack_size_p)
{
    struct thread_t *thread_p;

    if (*stack_size_p == 0) {
        *stack_size_p = 4096;
    } else if (*stack_size_p < 2048) {
        *stack_size_p = 2048;
    }

    thread_p = m_new_obj(struct thread_t);
    thread_p->stack_p = m_new(uint64_t, *stack_size_p / sizeof(uint64_t));

    mp_thread_mutex_lock(&thread_mutex, 1);

    /* Create thread. */
    thread_p->thrd_p = thrd_spawn(entry,
                                  arg_p,
                                  0,
                                  thread_p->stack_p,
                                  *stack_size_p);

    /* Add thread to linked list of all threads. */
    thread_p->ready = 0;
    thread_p->next_p = threads_p;
    threads_p = thread_p;

    mp_thread_mutex_unlock(&thread_mutex);

    /* Adjust stack_size to provide room to recover from hitting the
       limit */
    *stack_size_p -= 512;
}

void mp_thread_finish(void)
{
    /* Remove once the thread is correctly removed from the simba
       thread list. */
    thrd_suspend(NULL);
}

void mp_thread_mutex_init(mp_thread_mutex_t *mutex_p)
{
    sem_init(&mutex_p->sem, 0, 1);
}

int mp_thread_mutex_lock(mp_thread_mutex_t *mutex_p, int wait)
{
    return (sem_take(&mutex_p->sem, NULL));
}

void mp_thread_mutex_unlock(mp_thread_mutex_t *mutex_p)
{
    sem_give(&mutex_p->sem, 1);
}
