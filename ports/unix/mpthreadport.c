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
#include <stdlib.h>
#include <errno.h>

#include "py/runtime.h"
#include "py/mpthread.h"
#include "py/gc.h"
#include "stack_size.h"

#if MICROPY_PY_THREAD

#include <fcntl.h>
#include <signal.h>
#include <sched.h>
#include <semaphore.h>

#include "shared/runtime/gchelper.h"

// Some platforms don't have SIGRTMIN but if we do have it, use it to avoid
// potential conflict with other uses of the more commonly used SIGUSR1.
#ifdef SIGRTMIN
#define MP_THREAD_GC_SIGNAL (SIGRTMIN + 5)
#ifdef __ANDROID__
#define MP_THREAD_TERMINATE_SIGNAL (SIGRTMIN + 6)
#endif
#else
#define MP_THREAD_GC_SIGNAL (SIGUSR1)
#ifdef __ANDROID__
#define MP_THREAD_TERMINATE_SIGNAL (SIGUSR2)
#endif
#endif

// This value seems to be about right for both 32-bit and 64-bit builds.
#define THREAD_STACK_OVERFLOW_MARGIN (8192)

// this structure forms a linked list, one node per active thread
typedef struct _mp_thread_t {
    pthread_t id;           // system id of thread
    int ready;              // whether the thread is ready and running
    void *arg;              // thread Python args, a GC root pointer
    struct _mp_thread_t *next;
} mp_thread_t;

static pthread_key_t tls_key;

// The mutex is used for any code in this port that needs to be thread safe.
// Specifically for thread management, access to the linked list is one example.
// But also, e.g. scheduler state.
static mp_thread_recursive_mutex_t thread_mutex;
static mp_thread_t *thread;

// this is used to synchronise the signal handler of the thread
// it's needed because we can't use any pthread calls in a signal handler
#if defined(__APPLE__)
static char thread_signal_done_name[25];
static sem_t *thread_signal_done_p;
#else
static sem_t thread_signal_done;
#endif

void mp_thread_unix_begin_atomic_section(void) {
    mp_thread_recursive_mutex_lock(&thread_mutex, true);
}

void mp_thread_unix_end_atomic_section(void) {
    mp_thread_recursive_mutex_unlock(&thread_mutex);
}

// this signal handler is used to scan the regs and stack of a thread
static void mp_thread_gc(int signo, siginfo_t *info, void *context) {
    (void)info; // unused
    (void)context; // unused
    if (signo == MP_THREAD_GC_SIGNAL) {
        gc_helper_collect_regs_and_stack();
        // We have access to the context (regs, stack) of the thread but it seems
        // that we don't need the extra information, enough is captured by the
        // gc_collect_regs_and_stack function above
        // gc_collect_root((void**)context, sizeof(ucontext_t) / sizeof(uintptr_t));
        #if MICROPY_ENABLE_PYSTACK
        void **ptrs = (void **)(void *)MP_STATE_THREAD(pystack_start);
        gc_collect_root(ptrs, (MP_STATE_THREAD(pystack_cur) - MP_STATE_THREAD(pystack_start)) / sizeof(void *));
        #endif
        #if defined(__APPLE__)
        sem_post(thread_signal_done_p);
        #else
        sem_post(&thread_signal_done);
        #endif
    }
}

// On Android, pthread_cancel and pthread_setcanceltype are not implemented.
// To achieve that result a new signal handler responding on either
// (SIGRTMIN + 6) or SIGUSR2 is installed on every child thread.  The sole
// purpose of this new signal handler is to terminate the thread in a safe
// asynchronous manner.

#ifdef __ANDROID__
static void mp_thread_terminate(int signo, siginfo_t *info, void *context) {
    pthread_exit(NULL);
}
#endif

void mp_thread_init(void) {
    pthread_key_create(&tls_key, NULL);
    pthread_setspecific(tls_key, &mp_state_ctx.thread);

    // Needs to be a recursive mutex to emulate the behavior of
    // BEGIN_ATOMIC_SECTION on bare metal.
    mp_thread_recursive_mutex_init(&thread_mutex);

    // create first entry in linked list of all threads
    thread = malloc(sizeof(mp_thread_t));
    thread->id = pthread_self();
    thread->ready = 1;
    thread->arg = NULL;
    thread->next = NULL;

    #if defined(__APPLE__)
    snprintf(thread_signal_done_name, sizeof(thread_signal_done_name), "micropython_sem_%ld", (long)thread->id);
    thread_signal_done_p = sem_open(thread_signal_done_name, O_CREAT | O_EXCL, 0666, 0);
    #else
    sem_init(&thread_signal_done, 0, 0);
    #endif

    // enable signal handler for garbage collection
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = mp_thread_gc;
    sigemptyset(&sa.sa_mask);
    sigaction(MP_THREAD_GC_SIGNAL, &sa, NULL);

    // Install a signal handler for asynchronous termination if needed.
    #if defined(__ANDROID__)
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = mp_thread_terminate;
    sigemptyset(&sa.sa_mask);
    sigaction(MP_THREAD_TERMINATE_SIGNAL, &sa, NULL);
    #endif
}

void mp_thread_deinit(void) {
    mp_thread_unix_begin_atomic_section();
    while (thread->next != NULL) {
        mp_thread_t *th = thread;
        thread = thread->next;
        #if defined(__ANDROID__)
        pthread_kill(th->id, MP_THREAD_TERMINATE_SIGNAL);
        #else
        pthread_cancel(th->id);
        #endif
        free(th);
    }
    mp_thread_unix_end_atomic_section();
    #if defined(__APPLE__)
    sem_close(thread_signal_done_p);
    sem_unlink(thread_signal_done_name);
    #endif
    assert(thread->id == pthread_self());
    free(thread);
}

// This function scans all pointers that are external to the current thread.
// It does this by signalling all other threads and getting them to scan their
// own registers and stack.  Note that there may still be some edge cases left
// with race conditions and root-pointer scanning: a given thread may manipulate
// the global root pointers (in mp_state_ctx) while another thread is doing a
// garbage collection and tracing these pointers.
void mp_thread_gc_others(void) {
    mp_thread_unix_begin_atomic_section();
    for (mp_thread_t *th = thread; th != NULL; th = th->next) {
        gc_collect_root(&th->arg, 1);
        if (th->id == pthread_self()) {
            continue;
        }
        if (!th->ready) {
            continue;
        }
        pthread_kill(th->id, MP_THREAD_GC_SIGNAL);
        #if defined(__APPLE__)
        sem_wait(thread_signal_done_p);
        #else
        sem_wait(&thread_signal_done);
        #endif
    }
    mp_thread_unix_end_atomic_section();
}

mp_state_thread_t *mp_thread_get_state(void) {
    return (mp_state_thread_t *)pthread_getspecific(tls_key);
}

void mp_thread_set_state(mp_state_thread_t *state) {
    pthread_setspecific(tls_key, state);
}

mp_uint_t mp_thread_get_id(void) {
    return (mp_uint_t)pthread_self();
}

void mp_thread_start(void) {
    // enable realtime priority if `-X realtime` command line parameter was set
    #if defined(__APPLE__)
    if (mp_thread_is_realtime_enabled) {
        mp_thread_set_realtime();
    }
    #endif

    #if !defined(__ANDROID__)
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    #endif
    mp_thread_unix_begin_atomic_section();
    for (mp_thread_t *th = thread; th != NULL; th = th->next) {
        if (th->id == pthread_self()) {
            th->ready = 1;
            break;
        }
    }
    mp_thread_unix_end_atomic_section();
}

mp_uint_t mp_thread_create(void *(*entry)(void *), void *arg, size_t *stack_size) {
    // default stack size
    if (*stack_size == 0) {
        *stack_size = 32768 * UNIX_STACK_MULTIPLIER;
    }

    // minimum stack size is set by pthreads
    if (*stack_size < (size_t)PTHREAD_STACK_MIN) {
        *stack_size = (size_t)PTHREAD_STACK_MIN;
    }

    // ensure there is enough stack to include a stack-overflow margin
    if (*stack_size < 2 * THREAD_STACK_OVERFLOW_MARGIN) {
        *stack_size = 2 * THREAD_STACK_OVERFLOW_MARGIN;
    }

    // set thread attributes
    pthread_attr_t attr;
    int ret = pthread_attr_init(&attr);
    if (ret != 0) {
        goto er;
    }
    ret = pthread_attr_setstacksize(&attr, *stack_size);
    if (ret != 0) {
        goto er;
    }

    ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (ret != 0) {
        goto er;
    }

    mp_thread_unix_begin_atomic_section();

    // create thread
    pthread_t id;
    ret = pthread_create(&id, &attr, entry, arg);
    if (ret != 0) {
        mp_thread_unix_end_atomic_section();
        goto er;
    }

    // adjust stack_size to provide room to recover from hitting the limit
    *stack_size -= THREAD_STACK_OVERFLOW_MARGIN;

    // add thread to linked list of all threads
    mp_thread_t *th = malloc(sizeof(mp_thread_t));
    th->id = id;
    th->ready = 0;
    th->arg = arg;
    th->next = thread;
    thread = th;

    mp_thread_unix_end_atomic_section();

    MP_STATIC_ASSERT(sizeof(mp_uint_t) >= sizeof(pthread_t));
    return (mp_uint_t)id;

er:
    mp_raise_OSError(ret);
}

void mp_thread_finish(void) {
    mp_thread_unix_begin_atomic_section();
    mp_thread_t *prev = NULL;
    for (mp_thread_t *th = thread; th != NULL; th = th->next) {
        if (th->id == pthread_self()) {
            if (prev == NULL) {
                thread = th->next;
            } else {
                prev->next = th->next;
            }
            free(th);
            break;
        }
        prev = th;
    }
    mp_thread_unix_end_atomic_section();
}

void mp_thread_mutex_init(mp_thread_mutex_t *mutex) {
    pthread_mutex_init(mutex, NULL);
}

int mp_thread_mutex_lock(mp_thread_mutex_t *mutex, int wait) {
    int ret;
    if (wait) {
        ret = pthread_mutex_lock(mutex);
        if (ret == 0) {
            return 1;
        }
    } else {
        ret = pthread_mutex_trylock(mutex);
        if (ret == 0) {
            return 1;
        } else if (ret == EBUSY) {
            return 0;
        }
    }
    return -ret;
}

void mp_thread_mutex_unlock(mp_thread_mutex_t *mutex) {
    pthread_mutex_unlock(mutex);
    // TODO check return value
}

#if MICROPY_PY_THREAD_RECURSIVE_MUTEX

void mp_thread_recursive_mutex_init(mp_thread_recursive_mutex_t *mutex) {
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(mutex, &attr);
    pthread_mutexattr_destroy(&attr);
}

int mp_thread_recursive_mutex_lock(mp_thread_recursive_mutex_t *mutex, int wait) {
    return mp_thread_mutex_lock(mutex, wait);
}

void mp_thread_recursive_mutex_unlock(mp_thread_recursive_mutex_t *mutex) {
    mp_thread_mutex_unlock(mutex);
}

#endif // MICROPY_PY_THREAD_RECURSIVE_MUTEX

#endif // MICROPY_PY_THREAD

// this is used even when MICROPY_PY_THREAD is disabled

#if defined(__APPLE__)
#include <mach/mach_error.h>
#include <mach/mach_time.h>
#include <mach/thread_act.h>
#include <mach/thread_policy.h>

bool mp_thread_is_realtime_enabled;

// based on https://developer.apple.com/library/archive/technotes/tn2169/_index.html
void mp_thread_set_realtime(void) {
    mach_timebase_info_data_t timebase_info;

    mach_timebase_info(&timebase_info);

    const uint64_t NANOS_PER_MSEC = 1000000ULL;
    double clock2abs = ((double)timebase_info.denom / (double)timebase_info.numer) * NANOS_PER_MSEC;

    thread_time_constraint_policy_data_t policy;
    policy.period = 0;
    policy.computation = (uint32_t)(5 * clock2abs); // 5 ms of work
    policy.constraint = (uint32_t)(10 * clock2abs);
    policy.preemptible = FALSE;

    int kr = thread_policy_set(pthread_mach_thread_np(pthread_self()),
        THREAD_TIME_CONSTRAINT_POLICY,
        (thread_policy_t)&policy,
        THREAD_TIME_CONSTRAINT_POLICY_COUNT);

    if (kr != KERN_SUCCESS) {
        mach_error("thread_policy_set:", kr);
    }
}
#endif
