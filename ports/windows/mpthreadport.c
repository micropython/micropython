/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Andrew Leech
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

#if MICROPY_PY_THREAD

#include <windows.h>
#include "shared/runtime/gchelper.h"

// This value seems to be about right for both 32-bit and 64-bit builds.
#define THREAD_STACK_OVERFLOW_MARGIN (8192)

// this structure forms a linked list, one node per active thread.
typedef struct _thread_t {
    DWORD id;               // system id of thread
    HANDLE handle;          // handle of thread
    int ready;              // whether the thread is ready and running
    void *(*entry)(void *); // entrypoint, to be passed into thread wrapper
    void *arg;              // thread Python args, a GC root pointer
    struct _thread_t *next;
} thread_t;

static DWORD tls_key;
static thread_t *thread;
static HANDLE criticalSection = INVALID_HANDLE_VALUE;

static DWORD wait_for_event(HANDLE event, bool wait) {
    // WaitForSingleObjectEx will exit with WAIT_IO_COMPLETION when the thread
    // runs an APC function (mp_thread_gc below) so we ignore this and wait
    // for a normal event retult.
    DWORD ret = WAIT_IO_COMPLETION;
    while (WAIT_IO_COMPLETION == ret) {
        ret = WaitForSingleObjectEx(event, (wait)?INFINITE:0, TRUE);
    }
    return ret;
}

void mp_thread_windows_begin_atomic_section(void) {
    if (criticalSection != INVALID_HANDLE_VALUE) {
        wait_for_event(criticalSection, TRUE);
    }
}

void mp_thread_windows_end_atomic_section(void) {
    if (criticalSection != INVALID_HANDLE_VALUE) {
        ReleaseMutex(criticalSection);
    }
}

void mp_thread_init(void) {
    tls_key = TlsAlloc();
    TlsSetValue(tls_key, (LPVOID)&mp_state_ctx.thread);

    // Needs to be a recursive mutex to emulate the behavior of
    // BEGIN_ATOMIC_SECTION on bare metal. This is the default
    // behavior of win32 Mutex and CriticalSection objects.
    criticalSection = CreateMutex(NULL, FALSE, NULL);

    // create first entry in linked list of all threads.
    thread = malloc(sizeof(thread_t));
    thread->id = GetCurrentThreadId();
    thread->ready = 1;
    thread->arg = NULL;
    thread->next = NULL;

    // The handle is used for running gc via QueueUserAPC below.
    thread->handle = OpenThread(THREAD_SET_CONTEXT, FALSE, thread->id);
}

void mp_thread_deinit(void) {
    mp_thread_windows_begin_atomic_section();
    while (thread->next != NULL) {
        thread_t *th = thread;
        thread = thread->next;
        TerminateThread(th->handle, -1);
        free(th);
    }
    mp_thread_windows_end_atomic_section();
    assert(thread->id == GetCurrentThreadId());
    free(thread);
}

// this signal handler is used to scan the regs and stack of a thread.
static void WINAPI mp_thread_gc(ULONG_PTR parameter) {
    HANDLE thread_signal_done = (HANDLE)parameter;
    gc_helper_collect_regs_and_stack();
    #if MICROPY_ENABLE_PYSTACK
    void **ptrs = (void **)(void *)MP_STATE_THREAD(pystack_start);
    gc_collect_root(ptrs, (MP_STATE_THREAD(pystack_cur) - MP_STATE_THREAD(pystack_start)) / sizeof(void *));
    #endif
    SetEvent(thread_signal_done);
}

// This function scans all pointers that are external to the current thread.
// It does this by signalling all other threads and getting them to scan their
// own registers and stack.  Note that there may still be some edge cases left
// with race conditions and root-pointer scanning: a given thread may manipulate
// the global root pointers (in mp_state_ctx) while another thread is doing a
// garbage collection and tracing these pointers.
void mp_thread_gc_others(void) {
    mp_thread_windows_begin_atomic_section();
    for (thread_t *th = thread; th != NULL; th = th->next) {
        gc_collect_root(&th->arg, 1);
        if (th->id == GetCurrentThreadId()) {
            continue;
        }
        if (!th->ready) {
            continue;
        }

        // Used to signal when targetted thread gc is complete.
        HANDLE thread_signal_done = CreateEvent(NULL, FALSE, FALSE, NULL);

        // Run mp_thread_gc function on target thread (soon).
        QueueUserAPC(mp_thread_gc, th->handle, (ULONG_PTR)thread_signal_done);

        // wait for it to finish running.
        wait_for_event(thread_signal_done, TRUE);
    }
    mp_thread_windows_end_atomic_section();
}

mp_state_thread_t *mp_thread_get_state(void) {
    return (mp_state_thread_t *)TlsGetValue(tls_key);
}

void mp_thread_set_state(mp_state_thread_t *state) {
    TlsSetValue(tls_key, (LPVOID)state);
}

void mp_thread_start(void) {
    mp_thread_windows_begin_atomic_section();
    for (thread_t *th = thread; th != NULL; th = th->next) {
        if (th->id == GetCurrentThreadId()) {
            th->ready = 1;
            break;
        }
    }
    mp_thread_windows_end_atomic_section();
}

static DWORD WINAPI win32_entry(PVOID args) {
    // Win32 threads require different function form to posix.
    thread_t *th = (thread_t *)args;
    th->entry(th->arg);
    return 0;
}

void mp_thread_create(void *(*entry)(void *), void *arg, size_t *stack_size) {
    // default stack size is 8k machine-words.
    if (*stack_size == 0) {
        *stack_size = 8192 * sizeof(void *);
    }

    // ensure there is enough stack to include a stack-overflow margin.
    if (*stack_size < 2 * THREAD_STACK_OVERFLOW_MARGIN) {
        *stack_size = 2 * THREAD_STACK_OVERFLOW_MARGIN;
    }

    mp_thread_windows_begin_atomic_section();

    // create thread
    DWORD id;

    thread_t *th = malloc(sizeof(thread_t));
    th->entry = entry;
    th->arg = arg;

    HANDLE thread_handle = CreateThread(NULL, *stack_size, win32_entry, th, 0, &id);

    if (thread_handle == NULL) {
        free(th);
        mp_thread_windows_end_atomic_section();
        goto er;
    }

    // adjust stack_size to provide room to recover from hitting the limit.
    *stack_size -= THREAD_STACK_OVERFLOW_MARGIN;

    // add thread to linked list of all threads.
    th->id = id;
    th->handle = thread_handle;
    th->ready = 0;
    th->next = thread;
    thread = th;

    mp_thread_windows_end_atomic_section();

    return;

er:
    mp_raise_OSError(-1);
}

void mp_thread_finish(void) {
    mp_thread_windows_begin_atomic_section();
    thread_t *prev = NULL;
    for (thread_t *th = thread; th != NULL; th = th->next) {
        if (th->id == GetCurrentThreadId()) {
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
    mp_thread_windows_end_atomic_section();
}

void mp_thread_mutex_init(mp_thread_mutex_t *mutex) {
    // The win32 Mutex is re-entrant, unlike the posix equivalent.
    // To avoid this a sepaphore with size of 1 is used.
    *mutex = CreateSemaphore(NULL, 1, 1, NULL);
}

int mp_thread_mutex_lock(mp_thread_mutex_t *mutex, int wait) {
    if (wait_for_event(*mutex, wait) == 0) {
        return 1;
    } else {
        return 0;
    }
}

void mp_thread_mutex_unlock(mp_thread_mutex_t *mutex) {
    ReleaseSemaphore(*mutex, 1, NULL);
}

#endif // MICROPY_PY_THREAD
