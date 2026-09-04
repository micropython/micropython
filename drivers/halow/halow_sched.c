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
 * Cooperative task scheduler for the Morse Micro WLAN stack.
 */

#include "py/mphal.h"

#if MICROPY_PY_NETWORK_HALOW

#include <string.h>

#include "py/runtime.h"
#include "halow.h"
#include "halow_osal.h"
#include "halow_sched.h"

// Number of morselib tasks that can exist at once.  morselib itself creates two
// (the driver task and the UMAC event loop), plus one more for the SDIO/SPI IRQ
// task when the SDIO transport is in use.
#ifndef MICROPY_HW_HALOW_MAX_TASKS
#define MICROPY_HW_HALOW_MAX_TASKS  (4)
#endif

// Size of the context saved by halow_context_switch(), in words: r3-r11 and lr,
// plus the callee-saved half of the FPU register file when hardware floating
// point is in use.  Kept even so that stacks stay 8-byte aligned.
#if defined(__ARM_FP)
#define HALOW_CONTEXT_WORDS (10 + 16)
#else
#define HALOW_CONTEXT_WORDS (10)
#endif

static halow_task_t halow_tasks[MICROPY_HW_HALOW_MAX_TASKS];

// In creation order, which is the order they run in.
static halow_task_t *halow_task_list;

// The task currently running, or NULL when running the scheduler itself.
static halow_task_t *halow_task_cur;

// Wall clock one halow_sched_run() may spend.  The scheduler is cooperative, so
// a task that stops yielding cannot be preempted -- only denied another turn.
#ifndef HALOW_SCHED_BUDGET_MS
#define HALOW_SCHED_BUDGET_MS (20)
#endif

// Task turns per halow_sched_run() call.
// Ceiling on any single wait, however long the caller asked for.
#ifndef HALOW_WAIT_MAX_MS
#define HALOW_WAIT_MAX_MS (10000)
#endif

#ifndef HALOW_SCHED_PASSES
#define HALOW_SCHED_PASSES (4)
#endif

// Stack pointer of whoever called halow_sched_run(), saved while a task runs.
static void *halow_sched_sp;

// Set while halow_sched_run() is walking the task list, to make it re-entrant.
static volatile bool halow_sched_running;

// Switch from the context described by *from_sp to the one at to_sp.  Both
// contexts are cooperative, so only the callee-saved registers need to be
// preserved; the compiler has already spilled anything else it cares about.
static void __attribute__((naked, noinline)) halow_context_switch(void **from_sp, void *to_sp) {
    __asm volatile (
        "push   {r3-r11, lr}    \n"
        #if defined(__ARM_FP)
        "vpush  {d8-d15}        \n"
        #endif
        "str    sp, [r0]        \n"
        "mov    sp, r1          \n"
        #if defined(__ARM_FP)
        "vpop   {d8-d15}        \n"
        #endif
        "pop    {r3-r11, pc}    \n"
        );
}

// True when running in an exception handler, where the MicroPython event loop
// must not be pumped.
static inline bool halow_in_irq(void) {
    return (__get_IPSR() & IPSR_ISR_Msk) != 0;
}

// Entry trampoline: runs the task main function and then retires the task.
static void halow_task_trampoline(void) {
    halow_task_t *task = halow_task_cur;
    task->entry(task->arg);
    halow_sched_task_delete(NULL);
}

halow_task_t *halow_sched_task_create(void (*entry)(void *), void *arg, size_t stack_words, const char *name) {
    halow_task_t *task = NULL;
    for (size_t i = 0; i < MICROPY_HW_HALOW_MAX_TASKS; i++) {
        if (halow_tasks[i].stack == NULL) {
            task = &halow_tasks[i];
            break;
        }
    }
    if (task == NULL) {
        return NULL;
    }

    // Round the stack up to an even number of words so that it stays 8-byte
    // aligned, and reserve room for the initial context.
    stack_words = (stack_words + 1) & ~(size_t)1;
    if (stack_words < HALOW_CONTEXT_WORDS) {
        stack_words = HALOW_CONTEXT_WORDS;
    }
    uint32_t *stack = halow_osal_malloc(stack_words * sizeof(uint32_t));
    if (stack == NULL) {
        return NULL;
    }

    // Paint the stack so that overflow can be detected after the fact via
    // halow_sched_stack_free_words().  The stack sizes come from morselib and
    // are chosen for a FreeRTOS port, not for this scheduler.
    for (size_t i = 0; i < stack_words; i++) {
        stack[i] = HALOW_STACK_FILL;
    }

    memset(task, 0, sizeof(*task));
    task->stack = stack;
    task->stack_words = stack_words;
    task->name = name;
    task->entry = entry;
    task->arg = arg;
    task->state = HALOW_TASK_READY;

    // Build a context that halow_context_switch() can restore, with the
    // trampoline in the slot it pops into pc.
    uint32_t *sp = (uint32_t *)task->stack + stack_words - HALOW_CONTEXT_WORDS;
    memset(sp, 0, HALOW_CONTEXT_WORDS * sizeof(uint32_t));
    sp[HALOW_CONTEXT_WORDS - 1] = (uint32_t)halow_task_trampoline;
    task->sp = sp;

    // Append to the task list, so that tasks run in creation order.
    halow_task_t **tail = &halow_task_list;
    while (*tail != NULL) {
        tail = &(*tail)->next;
    }
    *tail = task;

    return task;
}

size_t halow_sched_stack_free_words(const halow_task_t *task) {
    if (task == NULL || task->stack == NULL) {
        return 0;
    }
    const uint32_t *stack = (const uint32_t *)task->stack;
    size_t free_words = 0;
    while (free_words < task->stack_words && stack[free_words] == HALOW_STACK_FILL) {
        free_words++;
    }
    return free_words;
}

void halow_sched_task_delete(halow_task_t *task) {
    if (task == NULL) {
        task = halow_task_cur;
        if (task == NULL) {
            return;
        }
        // Retire the calling task.  Its stack must not be touched again, so
        // switch away without saving anything of interest.
        task->state = HALOW_TASK_DEAD;
        void *discard;
        halow_context_switch(&discard, halow_sched_sp);
        // Unreachable: a dead task is never resumed.
        return;
    }
    task->state = HALOW_TASK_DEAD;
}

halow_task_t *halow_sched_task_current(void) {
    return halow_task_cur;
}

// Held while the transceiver is being serviced, and by which context.  See
// halow_sched_claim() in the header.
#define HALOW_OWNER_NONE   (0)
#define HALOW_OWNER_THREAD (1)
#define HALOW_OWNER_IRQ    (2)

static volatile uint8_t halow_service_owner;
static volatile uint8_t halow_service_depth;

// When the running pass has to be over.  Only meaningful inside
// halow_sched_run(); nothing checks it otherwise.
static uint32_t halow_sched_deadline;

// Whether the pass now running is out of time.  Only meaningful inside
// halow_sched_run(): outside one the deadline is whatever the last pass left
// behind, which is always in the past.
static bool halow_pass_expired(void) {
    return halow_sched_running &&
           (int32_t)(mp_hal_ticks_ms() - halow_sched_deadline) >= 0;
}

bool halow_sched_over_budget(void) {
    // Answered for the running task only.  A bus operation outside a task is
    // the boot firmware download, driven straight from MicroPython with nothing
    // waiting on it -- yielding there would leave the transfer half done.
    return halow_task_cur != NULL && halow_pass_expired();
}

bool halow_sched_in_callback;

bool halow_sched_claim(void) {
    uint8_t owner = halow_in_irq() ? HALOW_OWNER_IRQ : HALOW_OWNER_THREAD;
    mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
    bool claimed = halow_service_owner == HALOW_OWNER_NONE || halow_service_owner == owner;
    if (claimed) {
        halow_service_owner = owner;
        halow_service_depth++;
    }
    MICROPY_END_ATOMIC_SECTION(atomic_state);
    return claimed;
}

void halow_sched_release(void) {
    mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
    if (halow_service_depth > 0 && --halow_service_depth == 0) {
        halow_service_owner = HALOW_OWNER_NONE;
    }
    MICROPY_END_ATOMIC_SECTION(atomic_state);
}

void halow_sched_yield(void) {
    halow_task_t *task = halow_task_cur;
    if (task != NULL) {
        halow_context_switch(&task->sp, halow_sched_sp);
    } else {
        // Not task context: run the tasks, holding the poll off for the pass so
        // it cannot cut in on a transfer that is already under way.
        if (halow_sched_claim()) {
            halow_sched_run();
            halow_sched_release();
        }
        if (!halow_in_irq()) {
            // Not mp_event_wait_ms(): it raises, and a KeyboardInterrupt thrown out
            // of morselib abandons its mutexes.  Callbacks only, so a pending
            // exception stays pending and the VM raises it somewhere safe.
            halow_sched_in_callback = true;
            mp_handle_pending(MP_HANDLE_PENDING_CALLBACKS_ONLY);
            halow_sched_in_callback = false;
            MICROPY_INTERNAL_WFE(1);
        }
    }
}

// Counted only outside task context: a task parked on a semaphore is the normal
// steady state and would leave this permanently set.
static uint16_t halow_wait_depth;

bool halow_sched_in_wait(void) {
    return halow_wait_depth > 0;
}

bool halow_sched_teardown;

bool halow_sched_wait(halow_cond_fn_t cond, void *arg, uint32_t timeout_ms) {
    // MMOSAL_WAIT_FOREVER is a promise: morselib's SDIO lock path asserts if it
    // returns false, so in normal operation it is honoured and a genuine wedge
    // is the watchdog's problem.  During teardown the promise is capped -- deinit
    // on a dead bus has to complete.
    bool forever = timeout_ms == UINT32_MAX;
    if (timeout_ms > HALOW_WAIT_MAX_MS && (!forever || halow_sched_teardown)) {
        timeout_ms = HALOW_WAIT_MAX_MS;
    }
    uint32_t start = mp_hal_ticks_ms();
    bool satisfied;
    bool counted = halow_sched_task_current() == NULL;
    if (counted) {
        halow_wait_depth++;
    }
    for (;;) {
        if (cond(arg)) {
            satisfied = true;
            break;
        }
        if (timeout_ms == 0) {
            satisfied = false;
            break;
        }
        if (forever) {
            if (!halow_sched_teardown) {
                halow_sched_yield();
                continue;
            }
            // Teardown began mid-wait: one bounded grace period from here.
            timeout_ms = HALOW_WAIT_MAX_MS;
            start = mp_hal_ticks_ms();
            forever = false;
        }
        if ((uint32_t)(mp_hal_ticks_ms() - start) >= timeout_ms) {
            // Re-check once more, in case the condition was satisfied by an
            // interrupt while the deadline was being evaluated.
            satisfied = cond(arg);
            break;
        }
        halow_sched_yield();
    }
    if (counted) {
        halow_wait_depth--;
    }
    return satisfied;
}

void halow_sched_reap(void) {
    halow_task_t **prev = &halow_task_list;
    for (halow_task_t *task = halow_task_list; task != NULL;) {
        halow_task_t *next = task->next;
        if (task->state == HALOW_TASK_DEAD) {
            *prev = next;
            halow_osal_free(task->stack);
            task->stack = NULL;
            task->next = NULL;
        } else {
            prev = &task->next;
        }
        task = next;
    }
}

void halow_sched_run(void) {
    if (halow_sched_running || halow_task_cur != NULL) {
        // Already inside the scheduler, or called from a task.
        return;
    }
    halow_sched_running = true;
    halow_sched_deadline = mp_hal_ticks_ms() + HALOW_SCHED_BUDGET_MS;

    // Several passes per call rather than one.  Handling a frame takes more than
    // one task hop, and with a single pass each hop waits for the next poll,
    // which puts milliseconds of scheduling latency into every round trip.  A
    // task blocked in halow_sched_wait() stays runnable, so this cannot be a
    // loop-until-idle; the pass count is what keeps it from spinning.
    for (int pass = 0; pass < HALOW_SCHED_PASSES && !halow_pass_expired(); pass++) {
        for (halow_task_t *task = halow_task_list; task != NULL; task = task->next) {
            if (task->state != HALOW_TASK_READY) {
                continue;
            }
            halow_task_cur = task;
            halow_context_switch(&halow_sched_sp, task->sp);
            halow_task_cur = NULL;
        }
    }

    halow_sched_reap();
    halow_sched_running = false;
}

void halow_sched_deinit(void) {
    // Only safe to call from outside the scheduler; tasks are abandoned where
    // they stand, which is why morselib must be shut down first.  The stacks are
    // not freed individually: the whole pool goes back in halow_osal_deinit().
    halow_task_list = NULL;
    halow_task_cur = NULL;
    halow_sched_running = false;
    halow_service_owner = HALOW_OWNER_NONE;
    halow_service_depth = 0;
    memset(halow_tasks, 0, sizeof(halow_tasks));
}

#endif // MICROPY_PY_NETWORK_HALOW
