// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>

/** Background callbacks are a linked list of tasks to call in the background.
 *
 * Include a member of type `background_callback_t` inside an object
 * which needs to queue up background work, and zero-initialize it.
 *
 * To schedule the work, use background_callback_add, with fun as the
 * function to call and data pointing to the object itself.
 *
 * Next time background_callback_run_all() is called, the callback will
 * be run and removed from the linked list. Use `RUN_BACKGROUND_TASKS;` instead
 * of calling background_callback_run_all() directly.
 *
 * Queueing a task that is already queued does nothing.  Unconditionally
 * re-queueing it from its own background task will cause it to run during the
 * very next background-tasks invocation, leading to a CircuitPython freeze, so
 * don't do that.
 *
 * background_callback_add can be called from interrupt context.
 *
 * If your work isn't triggered by an event, then it may be better implemented
 * using ticks, which runs tasks every millisecond or so. Ticks are enabled with
 * supervisor_enable_tick() and disabled with supervisor_disable_tick(). When
 * enabled, a timer will schedule a callback to supervisor_background_tick(),
 * which includes port_background_tick(), every millisecond.
 */
typedef void (*background_callback_fun)(void *data);
typedef struct background_callback {
    background_callback_fun fun;
    void *data;
    struct background_callback *next;
    struct background_callback *prev;
} background_callback_t;

/* Add a background callback for which 'fun' and 'data' were previously set */
void background_callback_add_core(background_callback_t *cb);

/* Add a background callback to the given function with the given data.  When
 * the callback involves an object on the GC heap, the 'data' must be a pointer
 * to that object itself, not an internal pointer.  Otherwise, it can be the
 * case that no other references to the object itself survive, and the object
 * becomes garbage collected while an outstanding background callback still
 * exists.
 */
void background_callback_add(background_callback_t *cb, background_callback_fun fun, void *data);

/* Run all background callbacks.  Normally, this is done by the supervisor
 * whenever the list is non-empty */
void background_callback_run_all(void);

/* True when a background callback is pending. Helpful for checking background state when
 * interrupts are disabled. */
bool background_callback_pending(void);

/* During soft reset, remove all pending callbacks and clear the critical section flag */
void background_callback_reset(void);

/* Sometimes background callbacks must be blocked.  Use these functions to
 * bracket the section of code where this is the case.  These calls nest, and
 * begins must be balanced with ends.
 */
void background_callback_prevent(void);
void background_callback_allow(void);

/*
 * Background callbacks may stop objects from being collected
 */
void background_callback_gc_collect(void);
