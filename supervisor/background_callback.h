/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Jeff Epler for Adafruit Industries
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

#ifndef CIRCUITPY_INCLUDED_SUPERVISOR_BACKGROUND_CALLBACK_H
#define CIRCUITPY_INCLUDED_SUPERVISOR_BACKGROUND_CALLBACK_H

/** Background callbacks are a linked list of tasks to call in the background.
 *
 * Include a member of type `background_callback_t` inside an object
 * which needs to queue up background work, and zero-initialize it.
 *
 * To schedule the work, use background_callback_add, with fun as the
 * function to call and data pointing to the object itself.
 *
 * Next time run_background_tasks_if_tick is called, the callback will
 * be run and removed from the linked list.
 *
 * Queueing a task that is already queued does nothing.  Unconditionally
 * re-queueing it from its own background task will cause it to run during the
 * very next background-tasks invocation, leading to a CircuitPython freeze, so
 * don't do that.
 *
 * background_callback_add can be called from interrupt context.
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

/* During soft reset, remove all pending callbacks and clear the critical section flag */
void background_callback_reset(void);

/* Sometimes background callbacks must be blocked.  Use these functions to
 * bracket the section of code where this is the case.  These calls nest, and
 * begins must be balanced with ends.
 */
void background_callback_begin_critical_section(void);
void background_callback_end_critical_section(void);

/*
 * Background callbacks may stop objects from being collected
 */
void background_callback_gc_collect(void);

#endif
