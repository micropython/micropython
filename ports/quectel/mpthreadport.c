/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
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

#include "py/runtime.h"
#include "py/gc.h"
#include "py/mpthread.h"
#include "py/mphal.h"
#include "mpthreadport.h"
#include "helios_os.h"

#if MICROPY_PY_THREAD

// this structure forms a linked list, one node per active thread
typedef struct _thread_t {
    Helios_Thread_t id;     // system id of thread
    int ready;              // whether the thread is ready and running
    void *arg;              // thread Python args, a GC root pointer
    void *stack;            // pointer to the stack
    size_t stack_len;       // number of words in the stack
    struct _thread_t *next;
} thread_t;

// the mutex controls access to the linked list
static mp_thread_mutex_t thread_mutex = 0;
static thread_t thread_entry0;
static thread_t *thread = NULL; // root pointer, handled by mp_thread_gc_others

void mp_thread_init(void *stack, uint32_t stack_len) {
    mp_thread_set_state(&mp_state_ctx.thread);
    // create the first entry in the linked list of all threads
    thread = &thread_entry0;
    thread->id = Helios_Thread_GetID();
    thread->ready = 1;
    thread->arg = NULL;
    thread->stack = stack;
    thread->stack_len = stack_len;
    thread->next = NULL;
    mp_thread_mutex_init(&thread_mutex);
}

void _vPortCleanUpTCB(void *tcb) {
    if (thread == NULL) {
        // threading not yet initialised
        return;
    }
    thread_t *prev = NULL;
    mp_thread_mutex_lock(&thread_mutex, 1);
    for (thread_t *th = thread; th != NULL; prev = th, th = th->next) {
        // unlink the node from the list
        if ((void *)th->id == tcb) {
            if (prev != NULL) {
                prev->next = th->next;
            } else {
                // move the start pointer
                thread = th->next;
            }
            // explicitly release all its memory
            m_del(thread_t, th, 1);
            break;
        }
    }
    mp_thread_mutex_unlock(&thread_mutex);
}

void mp_thread_gc_others(void) {
    mp_thread_mutex_lock(&thread_mutex, 1);
    for (thread_t *th = thread; th != NULL; th = th->next) {
        gc_collect_root((void **)&th, 1);
        gc_collect_root(&th->arg, 1); // probably not needed
        if (th->id == Helios_Thread_GetID()) {
            continue;
        }
        if (!th->ready) {
            continue;
        }
        gc_collect_root(th->stack, th->stack_len); // probably not needed
    }
    mp_thread_mutex_unlock(&thread_mutex);
}

mp_state_thread_t *mp_thread_get_state(void) {
    return (mp_state_thread_t *)Helios_Thread_GetSpecific();
}

void mp_thread_set_state(mp_state_thread_t *state) {
    Helios_Thread_SetSpecific((mp_state_thread_t *)state);
}

void mp_thread_start(void) {
    mp_thread_mutex_lock(&thread_mutex, 1);
    for (thread_t *th = thread; th != NULL; th = th->next) {
        if (th->id == Helios_Thread_GetID()) {
            th->ready = 1;
            break;
        }
    }
    mp_thread_mutex_unlock(&thread_mutex);
}

static void *(*ext_thread_entry)(void *) = NULL;
static void thread_entry(void *arg) {
    if (ext_thread_entry) {
        ext_thread_entry(arg);
    }
	_vPortCleanUpTCB((void*)Helios_Thread_GetID());
    Helios_Thread_Exit();
}


int mp_thread_create_ex(void *(*entry)(void *), void *arg, size_t *stack_size, int priority, char *name) {
    // store thread entry function into a global variable so we can access it
	ext_thread_entry = entry;
	if (*stack_size == 0) {
        *stack_size = MP_THREAD_DEFAULT_STACK_SIZE; // default stack size
    } else if (*stack_size < MP_THREAD_MIN_STACK_SIZE) {
        *stack_size = MP_THREAD_MIN_STACK_SIZE; // minimum stack size
    }

    // Allocate linked-list node (must be outside thread_mutex lock)
    thread_t *th = m_new_obj(thread_t);

    mp_thread_mutex_lock(&thread_mutex, 1);

    // create thread
    Helios_ThreadAttr ThreadAttr = {
        .name = name,
        .stack_size = *stack_size / sizeof(uint32_t),
        .priority = priority,
        .entry = thread_entry,
        .argv = arg
    };
    Helios_Thread_t thread_id = Helios_Thread_Create(&ThreadAttr);
	if(thread_id == 0)
	{
		mp_thread_mutex_unlock(&thread_mutex);
		mp_raise_msg(&mp_type_OSError, (mp_rom_error_text_t)"can't create thread");
	}

    // add thread to linked list of all threads
    th->id = thread_id;
    th->ready = 0;
    th->arg = arg;
    th->stack = Helios_Thread_GetStaskPtr(th->id);

	//stack_len must be 1/4 of ThreadAttr.stack_size 
    th->stack_len = ThreadAttr.stack_size / sizeof(uint32_t);
    th->next = thread;
    thread = th;

    // adjust the stack_size to provide room to recover from hitting the limit
    *stack_size -= 1024;

    mp_thread_mutex_unlock(&thread_mutex);

    return (int)th;//return task_node
}

//forrest.liu@20210408 increase the priority for that python thread can,t be scheduled 
mp_uint_t mp_thread_create(void *(*entry)(void *), void *arg, size_t *stack_size) {
    int th_node = mp_thread_create_ex(entry, arg, stack_size, (MP_THREAD_PRIORITY - 1), "mp_thread");
    return th_node;
}

void mp_new_thread_add(uint32_t th_id, uint32_t stack_size) {
    // Allocate linked-list node (must be outside thread_mutex lock)
    thread_t *th = m_new_obj(thread_t);
    // add thread to linked list of all threads
    th->id = th_id;
    th->ready = 0;
    th->arg = NULL;
    th->stack = Helios_Thread_GetStaskPtr((Helios_Thread_t)th->id);
	
    th->stack_len = stack_size / sizeof(uint32_t);
    th->next = thread;
    thread = th;
}

void mp_thread_finish(void) {
    mp_thread_mutex_lock(&thread_mutex, 1);
    for (thread_t *th = thread; th != NULL; th = th->next) {
        if (th->id == Helios_Thread_GetID()) {
            th->ready = 0;
            break;
        }
    }
    mp_thread_mutex_unlock(&thread_mutex);
}

bool mp_thread_finish_by_threadid(int thread_id) {
    mp_thread_mutex_lock(&thread_mutex, 1);
    for (thread_t *th = thread; th != NULL; th = th->next) {
        if (th->id == (Helios_Thread_t)thread_id) {
            th->ready = 0;
            mp_thread_mutex_unlock(&thread_mutex);
            return true;
        }
    }
    mp_thread_mutex_unlock(&thread_mutex);
    return false;
}

bool mp_is_python_thread(void) {
	Helios_Thread_t curr_id = Helios_Thread_GetID(); 
    mp_thread_mutex_lock(&thread_mutex, 1);
    for (thread_t *th = thread; th != NULL; th = th->next) {
        if (th->id == curr_id) {
        	mp_thread_mutex_unlock(&thread_mutex);
            return true;
        }
    }
    mp_thread_mutex_unlock(&thread_mutex);
    return false;
}

int mp_thread_get_current_tsknode(void) {
    Helios_Thread_t curr_id = Helios_Thread_GetID(); 
    mp_thread_mutex_lock(&thread_mutex, 1);
    for (thread_t *th = thread; th != NULL; th = th->next) {
        if (th->id == (Helios_Thread_t)curr_id) {
            mp_thread_mutex_unlock(&thread_mutex);
            return (int)th;
        }
    }
    mp_thread_mutex_unlock(&thread_mutex);
    return 0;
}

int mp_thread_get_tskid_by_tsknode(void *th_node) {
    thread_t *th_id = (thread_t *)th_node;
    mp_thread_mutex_lock(&thread_mutex, 1);
    for (thread_t *th = thread; th != NULL; th = th->next) {
        if (th == th_id) {
            mp_thread_mutex_unlock(&thread_mutex);
            return (int)th->id;
        }
    }
    mp_thread_mutex_unlock(&thread_mutex);
    return 0;
}

mp_uint_t mp_thread_get_id(void) {
    return (mp_uint_t)Helios_Thread_GetID();
}

void mp_thread_mutex_init(mp_thread_mutex_t *mutex) {
    *mutex = Helios_Mutex_Create();
}

int mp_thread_mutex_lock(mp_thread_mutex_t *mutex, int wait) {
    return Helios_Mutex_Lock(*mutex, wait ? QPY_WAIT_FOREVER : QPY_NO_WAIT);
}

void mp_thread_mutex_unlock(mp_thread_mutex_t *mutex) {
    Helios_Mutex_Unlock(*mutex);
}

//Added by Freddy @20210818 delete a lock
void mp_thread_mutex_del(mp_thread_mutex_t *mutex) {
    Helios_Mutex_Delete(*mutex);
}

void mp_thread_semphore_init(mp_thread_semphore_t *sem, uint32_t initcount) {
    *sem = Helios_Semaphore_Create(initcount, initcount);
}

int mp_thread_semphore_acquire(mp_thread_semphore_t *sem, int wait) {
    return Helios_Semaphore_Acquire(*sem, wait);
}

void mp_thread_semphore_release(mp_thread_semphore_t *sem) {
    Helios_Semaphore_Release(*sem);
}

void mp_thread_semphore_del(mp_thread_semphore_t *sem) {
    Helios_Semaphore_Delete(*sem);
}

void mp_thread_deinit(void) {
    for (;;) {
        // Find a task to delete
        int id = 0;
        mp_thread_mutex_lock(&thread_mutex, 1);
        for (thread_t *th = thread; th != NULL; th = th->next) {
            // Don't delete the current task
            if (th->id != Helios_Thread_GetID()) {
                id = th->id;
                break;
            }
        }
        mp_thread_mutex_unlock(&thread_mutex);

        if (id == 0) {
            // No tasks left to delete
            break;
        } else {
            // Call qpy_thread_delete to delete the task (it will call vPortCleanUpTCB)
            Helios_Thread_Delete(id);
			_vPortCleanUpTCB((void*)id);
        }
    }
}

unsigned int mp_get_available_memory_size(void) 
{  
    return Helios_GetAvailableMemorySize();
}


#endif // MICROPY_PY_THREAD