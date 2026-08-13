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
 *
 * morselib is written against an RTOS: it spawns a small number of long-lived
 * tasks that sit in "wait for work, do work" loops.  MicroPython has no RTOS,
 * so this file provides just enough of one: each morselib task gets its own
 * stack and runs cooperatively, switching back to the scheduler at every point
 * where it would otherwise block.
 *
 * The scheduler is pumped from halow_poll(), which is dispatched via PendSV in
 * exactly the same way as cyw43_poll(), so morselib code never runs concurrently
 * with the MicroPython VM and lwIP needs no additional locking.
 */
#ifndef MICROPY_INCLUDED_DRIVERS_HALOW_HALOW_SCHED_H
#define MICROPY_INCLUDED_DRIVERS_HALOW_HALOW_SCHED_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Task states.
#define HALOW_TASK_READY    (0)     // runnable, will be resumed by the scheduler
#define HALOW_TASK_DEAD     (1)     // returned or deleted, stack is pending free

typedef struct _halow_task_t {
    void *sp;                       // stack pointer, valid while not running
    struct _halow_task_t *next;     // next task in the all-tasks list
    void *stack;                    // base of the allocated stack
    size_t stack_words;             // size of the allocated stack, in words
    const char *name;               // task name, for debugging
    void (*entry)(void *);          // task main function
    void *arg;                      // argument passed to the main function
    volatile uint32_t notify;       // pending task notifications
    volatile uint8_t state;         // one of HALOW_TASK_xxx
} halow_task_t;

// Condition function used by halow_sched_wait(), returns true when the wait is over.
typedef bool (*halow_cond_fn_t)(void *arg);

// Create a task.  It starts out ready and first runs on the next halow_sched_run().
halow_task_t *halow_sched_task_create(void (*entry)(void *), void *arg, size_t stack_words, const char *name);

// Pattern written across a task stack at creation, so that the untouched tail
// of the stack is distinguishable from anything the task has actually used.
#define HALOW_STACK_FILL (0xA5A5A5A5u)

// Words of `task`'s stack never written since creation, i.e. the headroom that
// was left over at its deepest point. Zero means the task used every word and
// may well have run past the end: treat that as an overflow, not as a tight
// fit. Diagnostic only; nothing in the driver depends on it.
size_t halow_sched_stack_free_words(const halow_task_t *task);

// Mark a task as dead.  Passing NULL kills the calling task, which does not return.
void halow_sched_task_delete(halow_task_t *task);

// Return the task the caller is running on, or NULL if this is not task context.
halow_task_t *halow_sched_task_current(void);

// Give up the CPU.  In task context this switches back to the scheduler; in any
// other context it pumps the scheduler and the MicroPython event loop instead.
void halow_sched_yield(void);

// Block until cond(arg) returns true or timeout_ms elapses.  A timeout_ms of 0
// polls once.  Longer waits are capped: nothing here waits forever, because a
// cooperative scheduler that never comes back takes the firmware with it.
// Returns the final value of cond(arg), i.e. true if the wait was satisfied.
bool halow_sched_wait(halow_cond_fn_t cond, void *arg, uint32_t timeout_ms);

// Resume every ready task once, then return.  Safe to call from any context and
// re-entrant against itself (a nested call is a no-op).
void halow_sched_run(void);

// Claim the right to service the transceiver; false if the other context holds
// it.  PendSV preempts MicroPython, which also runs the driver directly, so
// without this the poll can start a bus transaction on top of one in flight.
// Claims nest within a context: refusing a nested one deadlocks, as the nested
// caller is a wait only the scheduler it was refused can satisfy.
// True while scheduled Python callbacks run from inside a morselib wait.
extern bool halow_sched_in_callback;

// True while the driver is tearing down: FOREVER waits become bounded so
// deinit completes even on a dead bus.
extern bool halow_sched_teardown;

bool halow_sched_claim(void);
void halow_sched_release(void);

// True while the driver is inside a wait.  A wait runs scheduled Python
// callbacks, so anything reachable from Python that would free the driver's
// memory has to refuse while this holds.
bool halow_sched_in_wait(void);

// True once the running scheduler pass has used up its time.  Bus operations
// check this so that a task which will not yield of its own accord still gives
// the rest of the system a turn between transactions.
bool halow_sched_over_budget(void);

// Called from halow_sched_run().
void halow_sched_reap(void);

void halow_sched_deinit(void);

#endif // MICROPY_INCLUDED_DRIVERS_HALOW_HALOW_SCHED_H
