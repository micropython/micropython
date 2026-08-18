/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 OpenMV LLC.
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
 *
 * MMOSAL implementation for MicroPython.
 *
 * This maps the RTOS abstraction layer that morselib is written against onto
 * the cooperative scheduler in halow_sched.c.  Allocation is served from a
 * dedicated static heap rather than the MicroPython GC heap, because morselib
 * allocates from PendSV context where the GC must not run.
 */

// stdarg.h must come first: py/mpprint.h only declares mp_vprintf() if va_list
// is already available.
#include <stdarg.h>

#include "py/mphal.h"

#if MICROPY_PY_NETWORK_HALOW

#include <string.h>

#include "py/mpprint.h"
#include "py/runtime.h"

#include "mmosal.h"
#include "halow.h"
#include "halow_sched.h"
#include "halow_osal.h"

// Bytes of heap reserved for morselib.  This covers packet memory as well, so
// it is sized to match the 95 KB heap the Morse reference ports use.
#ifndef MICROPY_HW_HALOW_HEAP_SIZE
#define MICROPY_HW_HALOW_HEAP_SIZE  (96 * 1024)
#endif

/*******************************************************************************/
// Heap

// A first-fit allocator over a static pool.  Blocks are kept in address order in
// a single list so that adjacent free blocks can be coalesced on free.
typedef struct _halow_block_t {
    struct _halow_block_t *next;    // next block, in address order
    size_t size;                    // usable bytes in this block
    bool used;                      // true if handed out to a caller
} halow_block_t;

#define HALOW_BLOCK_ALIGN   (8)
#define HALOW_BLOCK_ROUND(n) (((n) + (HALOW_BLOCK_ALIGN - 1)) & ~(size_t)(HALOW_BLOCK_ALIGN - 1))
#define HALOW_BLOCK_HDR     (HALOW_BLOCK_ROUND(sizeof(halow_block_t)))
#define HALOW_BLOCK_DATA(b) ((void *)((uint8_t *)(b) + HALOW_BLOCK_HDR))
#define HALOW_DATA_BLOCK(p) ((halow_block_t *)((uint8_t *)(p) - HALOW_BLOCK_HDR))

// The pool itself is a single block taken from the MicroPython heap when the
// driver is brought up, and held by a root pointer so the GC keeps it alive.
// Sub-allocation out of it is done here rather than by the GC, because morselib
// allocates from PendSV context where the GC must not run.
static halow_block_t *halow_heap_head;

bool halow_osal_init(void) {
    if (MP_STATE_PORT(halow_heap) != NULL) {
        return true;
    }
    uint8_t *heap = m_malloc_maybe(MICROPY_HW_HALOW_HEAP_SIZE);
    if (heap == NULL) {
        return false;
    }
    MP_STATE_PORT(halow_heap) = heap;

    halow_heap_head = (halow_block_t *)heap;
    halow_heap_head->next = NULL;
    halow_heap_head->size = MICROPY_HW_HALOW_HEAP_SIZE - HALOW_BLOCK_HDR;
    halow_heap_head->used = false;
    return true;
}

void *halow_osal_malloc(size_t size) {
    if (size == 0) {
        return NULL;
    }
    // Also stops HALOW_BLOCK_ROUND wrapping to zero for sizes near SIZE_MAX,
    // which would otherwise pass every first-fit test and return a pointer into
    // the block list itself.  Buffer sizes can be derived from received frames.
    if (size > MICROPY_HW_HALOW_HEAP_SIZE) {
        return NULL;
    }
    size = HALOW_BLOCK_ROUND(size);

    if (halow_heap_head == NULL) {
        return NULL;
    }

    mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
    void *ptr = NULL;
    for (halow_block_t *b = halow_heap_head; b != NULL; b = b->next) {
        if (b->used || b->size < size) {
            continue;
        }
        // Split the block if the remainder can hold a header and something useful.
        if (b->size >= size + HALOW_BLOCK_HDR + HALOW_BLOCK_ALIGN) {
            halow_block_t *split = (halow_block_t *)((uint8_t *)b + HALOW_BLOCK_HDR + size);
            split->next = b->next;
            split->size = b->size - size - HALOW_BLOCK_HDR;
            split->used = false;
            b->next = split;
            b->size = size;
        }
        b->used = true;
        ptr = HALOW_BLOCK_DATA(b);
        break;
    }
    MICROPY_END_ATOMIC_SECTION(atomic_state);
    return ptr;
}

void halow_osal_free(void *ptr) {
    if (ptr == NULL || halow_heap_head == NULL) {
        return;
    }
    mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
    HALOW_DATA_BLOCK(ptr)->used = false;
    // Coalesce the whole list; it is short and this keeps free O(n) without a
    // back pointer per block.
    for (halow_block_t *b = halow_heap_head; b != NULL && b->next != NULL;) {
        if (!b->used && !b->next->used) {
            b->size += HALOW_BLOCK_HDR + b->next->size;
            b->next = b->next->next;
        } else {
            b = b->next;
        }
    }
    MICROPY_END_ATOMIC_SECTION(atomic_state);
}

// Named up here because teardown has to drop the list before the pool the nodes
// live in goes back to the GC heap; the timers themselves are further down.
static struct mmosal_timer *halow_timer_list;

void halow_osal_deinit(void) {
    // Nodes are allocated from the pool that is about to go back to the GC
    // heap.  morselib normally deletes its timers first, but a fatal error or a
    // wedged shutdown does not, and the list outlives halow_init() otherwise.
    halow_timer_list = NULL;
    halow_heap_head = NULL;
    if (MP_STATE_PORT(halow_heap) != NULL) {
        m_free(MP_STATE_PORT(halow_heap));
        MP_STATE_PORT(halow_heap) = NULL;
    }
}

void *mmosal_malloc_(size_t size) {
    return halow_osal_malloc(size);
}

void *mmosal_calloc_(size_t nitems, size_t size) {
    // nitems * size wraps for large inputs, which would allocate a small block
    // while the caller believes it owns the full product; its first write past
    // the block corrupts the heap. Fail the allocation instead.
    if (nitems != 0 && size > SIZE_MAX / nitems) {
        return NULL;
    }
    size_t total = nitems * size;
    void *ptr = halow_osal_malloc(total);
    if (ptr != NULL) {
        memset(ptr, 0, total);
    }
    return ptr;
}

void *mmosal_realloc_(void *ptr, size_t size) {
    if (ptr == NULL) {
        return halow_osal_malloc(size);
    }
    if (size == 0) {
        halow_osal_free(ptr);
        return NULL;
    }
    size_t old_size = HALOW_DATA_BLOCK(ptr)->size;
    if (old_size >= size) {
        return ptr;
    }
    void *new_ptr = halow_osal_malloc(size);
    if (new_ptr != NULL) {
        memcpy(new_ptr, ptr, old_size);
        halow_osal_free(ptr);
    }
    return new_ptr;
}

void mmosal_free(void *ptr) {
    halow_osal_free(ptr);
}

void *mmosal_malloc_dbg(size_t size, const char *name, unsigned line_number) {
    (void)name;
    (void)line_number;
    return mmosal_malloc_(size);
}

void *mmosal_calloc_dbg(size_t nitems, size_t size, const char *name, unsigned line_number) {
    (void)name;
    (void)line_number;
    return mmosal_calloc_(nitems, size);
}

void *mmosal_realloc_dbg(void *ptr, size_t size, const char *name, unsigned line_number) {
    (void)name;
    (void)line_number;
    return mmosal_realloc_(ptr, size);
}

/*******************************************************************************/
// Tasks

static unsigned halow_critical_nesting;
static mp_uint_t halow_critical_state;

struct mmosal_task *mmosal_task_create(mmosal_task_fn_t task_fn, void *argument,
    enum mmosal_task_priority priority, unsigned stack_size_u32, const char *name) {
    // Priorities are ignored: tasks run to their next blocking point in creation
    // order, so there is nothing to prioritise between.
    (void)priority;
    return (struct mmosal_task *)halow_sched_task_create(task_fn, argument, stack_size_u32, name);
}

void mmosal_task_delete(struct mmosal_task *task) {
    halow_sched_task_delete((halow_task_t *)task);
}

struct mmosal_task *mmosal_task_get_active(void) {
    return (struct mmosal_task *)halow_sched_task_current();
}

void mmosal_task_yield(void) {
    halow_sched_yield();
}

static bool halow_deadline_passed(void *arg) {
    return (int32_t)(mp_hal_ticks_ms() - *(uint32_t *)arg) >= 0;
}

void mmosal_task_sleep(uint32_t duration_ms) {
    if (duration_ms == 0) {
        // A zero delay is a yield on an RTOS, not a no-op.
        halow_sched_yield();
        return;
    }
    uint32_t deadline = mp_hal_ticks_ms() + duration_ms;
    halow_sched_wait(halow_deadline_passed, &deadline, duration_ms);
}

// morselib only ever holds a critical section across straight-line work (list
// and counter updates), never across a blocking call. That matters here: the
// nesting count is global rather than per-task, so a task that blocked while
// holding one would leave interrupts disabled for whatever ran next.
void mmosal_task_enter_critical(void) {
    mp_uint_t state = MICROPY_BEGIN_ATOMIC_SECTION();
    if (halow_critical_nesting++ == 0) {
        halow_critical_state = state;
    }
}

// Whether a critical section is open.  The bus path checks this before giving
// up a turn: yielding here would park the section on the suspended task's stack
// with interrupts still disabled, and the count is global, not per task.
bool halow_osal_in_critical(void) {
    return halow_critical_nesting > 0;
}

void mmosal_task_exit_critical(void) {
    if (halow_critical_nesting > 0 && --halow_critical_nesting == 0) {
        MICROPY_END_ATOMIC_SECTION(halow_critical_state);
    }
}

void mmosal_disable_interrupts(void) {
    mmosal_task_enter_critical();
}

void mmosal_enable_interrupts(void) {
    mmosal_task_exit_critical();
}

const char *mmosal_task_name(void) {
    halow_task_t *task = halow_sched_task_current();
    return task != NULL ? task->name : "main";
}

static bool halow_task_is_dead(void *arg) {
    return ((halow_task_t *)arg)->state == HALOW_TASK_DEAD;
}

void mmosal_task_join(struct mmosal_task *task) {
    halow_sched_wait(halow_task_is_dead, task, UINT32_MAX);
}

static bool halow_task_notified(void *arg) {
    halow_task_t *task = arg;
    mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
    bool notified = task->notify != 0;
    if (notified) {
        task->notify--;
    }
    MICROPY_END_ATOMIC_SECTION(atomic_state);
    return notified;
}

bool mmosal_task_wait_for_notification(uint32_t timeout_ms) {
    halow_task_t *task = halow_sched_task_current();
    if (task == NULL) {
        // Only tasks can wait for notifications.
        return false;
    }
    return halow_sched_wait(halow_task_notified, task, timeout_ms);
}

void mmosal_task_notify(struct mmosal_task *task) {
    if (task == NULL) {
        return;
    }
    mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
    ((halow_task_t *)task)->notify++;
    MICROPY_END_ATOMIC_SECTION(atomic_state);
}

void mmosal_task_notify_from_isr(struct mmosal_task *task) {
    mmosal_task_notify(task);
}

/*******************************************************************************/
// Mutexes

struct mmosal_mutex {
    volatile halow_task_t *owner;
    volatile uint32_t taken_ms;
    volatile bool locked;
};

struct mmosal_mutex *mmosal_mutex_create(const char *name) {
    (void)name;
    struct mmosal_mutex *mutex = halow_osal_malloc(sizeof(*mutex));
    if (mutex != NULL) {
        mutex->owner = NULL;
        mutex->locked = false;
    }
    return mutex;
}

void mmosal_mutex_delete(struct mmosal_mutex *mutex) {
    halow_osal_free(mutex);
}

static bool halow_mutex_acquired(void *arg) {
    struct mmosal_mutex *mutex = arg;
    mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
    bool acquired = !mutex->locked;
    if (acquired) {
        mutex->locked = true;
        mutex->owner = halow_sched_task_current();
        mutex->taken_ms = mp_hal_ticks_ms();
    }
    MICROPY_END_ATOMIC_SECTION(atomic_state);
    return acquired;
}

bool mmosal_mutex_get(struct mmosal_mutex *mutex, uint32_t timeout_ms) {
    if (mutex == NULL) {
        return false;
    }
    if (timeout_ms == UINT32_MAX) {
        // A held mutex is normally released in well under a second; nine is a
        // pathology worth naming before settling in to wait it out.
        if (halow_sched_wait(halow_mutex_acquired, mutex, 9000)) {
            return true;
        }
        const halow_task_t *owner = (const halow_task_t *)mutex->owner;
        mp_printf(&mp_plat_print, "halow: mutex %p slow (owner=%s state=%u held=%ums)\n",
            mutex, owner != NULL ? owner->name : "thread",
            owner != NULL ? (unsigned)owner->state : 0u,
            (unsigned)(mp_hal_ticks_ms() - mutex->taken_ms));
    }
    return halow_sched_wait(halow_mutex_acquired, mutex, timeout_ms);
}

bool mmosal_mutex_release(struct mmosal_mutex *mutex) {
    if (mutex == NULL) {
        return false;
    }
    mutex->owner = NULL;
    mutex->locked = false;
    return true;
}

bool mmosal_mutex_is_held_by_active_task(struct mmosal_mutex *mutex) {
    return mutex != NULL && mutex->locked && mutex->owner == halow_sched_task_current();
}

/*******************************************************************************/
// Semaphores

struct mmosal_sem {
    volatile unsigned count;
    unsigned max_count;
};

struct mmosal_sem *mmosal_sem_create(unsigned max_count, unsigned initial_count, const char *name) {
    (void)name;
    struct mmosal_sem *sem = halow_osal_malloc(sizeof(*sem));
    if (sem != NULL) {
        sem->count = initial_count;
        sem->max_count = max_count;
    }
    return sem;
}

void mmosal_sem_delete(struct mmosal_sem *sem) {
    halow_osal_free(sem);
}

bool mmosal_sem_give(struct mmosal_sem *sem) {
    if (sem == NULL) {
        return false;
    }
    mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
    bool given = sem->count < sem->max_count;
    if (given) {
        sem->count++;
    }
    MICROPY_END_ATOMIC_SECTION(atomic_state);
    return given;
}

bool mmosal_sem_give_from_isr(struct mmosal_sem *sem) {
    return mmosal_sem_give(sem);
}

static bool halow_sem_taken(void *arg) {
    struct mmosal_sem *sem = arg;
    mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
    bool taken = sem->count > 0;
    if (taken) {
        sem->count--;
    }
    MICROPY_END_ATOMIC_SECTION(atomic_state);
    return taken;
}

bool mmosal_sem_wait(struct mmosal_sem *sem, uint32_t timeout_ms) {
    if (sem == NULL) {
        return false;
    }
    return halow_sched_wait(halow_sem_taken, sem, timeout_ms);
}

uint32_t mmosal_sem_get_count(struct mmosal_sem *sem) {
    return sem != NULL ? sem->count : 0;
}

/*******************************************************************************/
// Binary semaphores

struct mmosal_semb {
    volatile bool signalled;
};

struct mmosal_semb *mmosal_semb_create(const char *name) {
    (void)name;
    struct mmosal_semb *semb = halow_osal_malloc(sizeof(*semb));
    if (semb != NULL) {
        semb->signalled = false;
    }
    return semb;
}

void mmosal_semb_delete(struct mmosal_semb *semb) {
    halow_osal_free(semb);
}

bool mmosal_semb_give(struct mmosal_semb *semb) {
    if (semb == NULL) {
        return false;
    }
    semb->signalled = true;
    return true;
}

bool mmosal_semb_give_from_isr(struct mmosal_semb *semb) {
    return mmosal_semb_give(semb);
}

static bool halow_semb_taken(void *arg) {
    struct mmosal_semb *semb = arg;
    mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
    bool taken = semb->signalled;
    semb->signalled = false;
    MICROPY_END_ATOMIC_SECTION(atomic_state);
    return taken;
}

bool mmosal_semb_wait(struct mmosal_semb *semb, uint32_t timeout_ms) {
    if (semb == NULL) {
        return false;
    }
    return halow_sched_wait(halow_semb_taken, semb, timeout_ms);
}

/*******************************************************************************/
// Queues

struct mmosal_queue {
    size_t num_items;
    size_t item_size;
    volatile size_t head;
    volatile size_t tail;
    volatile size_t count;
    uint8_t *items;
};

struct mmosal_queue *mmosal_queue_create(size_t num_items, size_t item_size, const char *name) {
    (void)name;
    if (item_size != 0 && num_items > (SIZE_MAX - sizeof(struct mmosal_queue)) / item_size) {
        return NULL;
    }
    struct mmosal_queue *queue = halow_osal_malloc(sizeof(*queue) + num_items * item_size);
    if (queue != NULL) {
        queue->num_items = num_items;
        queue->item_size = item_size;
        queue->head = 0;
        queue->tail = 0;
        queue->count = 0;
        queue->items = (uint8_t *)(queue + 1);
    }
    return queue;
}

void mmosal_queue_delete(struct mmosal_queue *queue) {
    halow_osal_free(queue);
}

bool mmosal_queue_pop_from_isr(struct mmosal_queue *queue, void *item) {
    if (queue == NULL) {
        return false;
    }
    mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
    bool popped = queue->count > 0;
    if (popped) {
        memcpy(item, queue->items + queue->head * queue->item_size, queue->item_size);
        queue->head = (queue->head + 1) % queue->num_items;
        queue->count--;
    }
    MICROPY_END_ATOMIC_SECTION(atomic_state);
    return popped;
}

bool mmosal_queue_push_from_isr(struct mmosal_queue *queue, const void *item) {
    if (queue == NULL) {
        return false;
    }
    mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
    bool pushed = queue->count < queue->num_items;
    if (pushed) {
        memcpy(queue->items + queue->tail * queue->item_size, item, queue->item_size);
        queue->tail = (queue->tail + 1) % queue->num_items;
        queue->count++;
    }
    MICROPY_END_ATOMIC_SECTION(atomic_state);
    return pushed;
}

// Bundles a queue with the item being transferred, for the wait callbacks below.
typedef struct _halow_queue_op_t {
    struct mmosal_queue *queue;
    void *item;
} halow_queue_op_t;

static bool halow_queue_popped(void *arg) {
    halow_queue_op_t *op = arg;
    return mmosal_queue_pop_from_isr(op->queue, op->item);
}

static bool halow_queue_pushed(void *arg) {
    halow_queue_op_t *op = arg;
    return mmosal_queue_push_from_isr(op->queue, op->item);
}

bool mmosal_queue_pop(struct mmosal_queue *queue, void *item, uint32_t timeout_ms) {
    halow_queue_op_t op = { queue, item };
    return halow_sched_wait(halow_queue_popped, &op, timeout_ms);
}

bool mmosal_queue_push(struct mmosal_queue *queue, const void *item, uint32_t timeout_ms) {
    halow_queue_op_t op = { queue, (void *)item };
    return halow_sched_wait(halow_queue_pushed, &op, timeout_ms);
}

/*******************************************************************************/
// Time

uint32_t mmosal_get_time_ms(void) {
    return mp_hal_ticks_ms();
}

uint32_t mmosal_get_time_ticks(void) {
    return mp_hal_ticks_ms();
}

uint32_t mmosal_ticks_per_second(void) {
    return 1000;
}

/*******************************************************************************/
// Timers

struct mmosal_timer {
    struct mmosal_timer *next;
    const char *name;
    uint32_t period_ms;
    uint32_t expires_at;
    bool auto_reload;
    volatile bool active;
    void *arg;
    timer_callback_t callback;
};

struct mmosal_timer *mmosal_timer_create(const char *name, uint32_t timer_period_ms,
    bool auto_reload, void *arg, timer_callback_t callback) {
    struct mmosal_timer *timer = halow_osal_malloc(sizeof(*timer));
    if (timer == NULL) {
        return NULL;
    }
    timer->name = name;
    timer->period_ms = timer_period_ms;
    timer->expires_at = 0;
    timer->auto_reload = auto_reload;
    timer->active = false;
    timer->arg = arg;
    timer->callback = callback;

    mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
    timer->next = halow_timer_list;
    halow_timer_list = timer;
    MICROPY_END_ATOMIC_SECTION(atomic_state);
    return timer;
}

void mmosal_timer_delete(struct mmosal_timer *timer) {
    if (timer == NULL) {
        return;
    }
    mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
    for (struct mmosal_timer **t = &halow_timer_list; *t != NULL; t = &(*t)->next) {
        if (*t == timer) {
            *t = timer->next;
            break;
        }
    }
    MICROPY_END_ATOMIC_SECTION(atomic_state);
    halow_osal_free(timer);
}

bool mmosal_timer_start(struct mmosal_timer *timer) {
    if (timer == NULL) {
        return false;
    }
    timer->expires_at = mp_hal_ticks_ms() + timer->period_ms;
    timer->active = true;
    return true;
}

bool mmosal_timer_stop(struct mmosal_timer *timer) {
    if (timer == NULL) {
        return false;
    }
    timer->active = false;
    return true;
}

bool mmosal_timer_change_period(struct mmosal_timer *timer, uint32_t new_period) {
    if (timer == NULL) {
        return false;
    }
    timer->period_ms = new_period;
    if (timer->active) {
        timer->expires_at = mp_hal_ticks_ms() + new_period;
    }
    return true;
}

void *mmosal_timer_get_arg(struct mmosal_timer *timer) {
    return timer != NULL ? timer->arg : NULL;
}

bool mmosal_is_timer_active(struct mmosal_timer *timer) {
    return timer != NULL && timer->active;
}

void halow_osal_timer_poll(void) {
    uint32_t now = mp_hal_ticks_ms();
    struct mmosal_timer *timer = halow_timer_list;
    while (timer != NULL) {
        // Read before the callback: deleting the timer from inside its own
        // callback is allowed, and that frees the node standing here.
        struct mmosal_timer *next = timer->next;
        if (timer->active && (int32_t)(now - timer->expires_at) >= 0) {
            if (timer->auto_reload) {
                timer->expires_at = now + timer->period_ms;
            } else {
                timer->active = false;
            }
            timer->callback(timer);
        }
        timer = next;
    }
}

/*******************************************************************************/
// Failure handling

int mmosal_printf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    int ret = mp_vprintf(&mp_plat_print, format, args);
    va_end(args);
    return ret;
}

// The console dies with many of the faults this reports, so the record also
// lands in RAM where a debug probe can read it post-mortem.
struct halow_fatal_record halow_fatal_record;

void mmosal_log_failure_info(const struct mmosal_failure_info *info) {
    halow_fatal_record.pc = info->pc;
    halow_fatal_record.lr = info->lr;
    halow_fatal_record.fileid = info->fileid;
    halow_fatal_record.line = info->line;
    halow_fatal_record.ticks_ms = mp_hal_ticks_ms();
    halow_fatal_record.magic = HALOW_FATAL_MAGIC;
    mp_printf(&mp_plat_print, "halow: failure at pc=0x%08x lr=0x%08x file=%u line=%u\n",
        (unsigned int)info->pc, (unsigned int)info->lr,
        (unsigned int)info->fileid, (unsigned int)info->line);
}

bool mmosal_extract_failure_info(struct mmosal_failure_info *buf, uint32_t *failure_count) {
    (void)buf;
    if (failure_count != NULL) {
        *failure_count = 0;
    }
    return false;
}

void mmosal_impl_assert(void) {
    mp_printf(&mp_plat_print, "halow: assertion failed\n");
    #ifdef MICROPY_BOARD_FATAL_ERROR
    MICROPY_BOARD_FATAL_ERROR("halow assertion failed");
    #endif
    for (;;) {
    }
}

// morselib links against parts of libc that expect a hosted environment.  These
// are only reachable from abort(), which cannot happen here: mmosal_impl_assert()
// handles failures first.
MP_WEAK int _kill(int pid, int sig) {
    (void)pid;
    (void)sig;
    return -1;
}

MP_WEAK int _getpid(void) {
    return 0;
}

int mmosal_main(mmosal_app_init_cb_t app_init_cb) {
    // The MicroPython runtime owns main(); morselib is driven from halow_poll().
    (void)app_init_cb;
    return -1;
}

#endif // MICROPY_PY_NETWORK_HALOW
