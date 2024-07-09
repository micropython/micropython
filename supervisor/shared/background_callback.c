// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "py/gc.h"
#include "py/mpconfig.h"
#include "supervisor/background_callback.h"
#include "supervisor/linker.h"
#include "supervisor/port.h"
#include "supervisor/shared/tick.h"
#include "shared-bindings/microcontroller/__init__.h"

static volatile background_callback_t *volatile callback_head, *volatile callback_tail;

#ifndef CALLBACK_CRITICAL_BEGIN
#define CALLBACK_CRITICAL_BEGIN (common_hal_mcu_disable_interrupts())
#endif
#ifndef CALLBACK_CRITICAL_END
#define CALLBACK_CRITICAL_END (common_hal_mcu_enable_interrupts())
#endif

MP_WEAK void PLACE_IN_ITCM(port_wake_main_task)(void) {
}

void PLACE_IN_ITCM(background_callback_add_core)(background_callback_t * cb) {
    CALLBACK_CRITICAL_BEGIN;
    if (cb->prev || callback_head == cb) {
        CALLBACK_CRITICAL_END;
        return;
    }
    cb->next = 0;
    cb->prev = (background_callback_t *)callback_tail;
    if (callback_tail) {
        callback_tail->next = cb;
    }
    if (!callback_head) {
        callback_head = cb;
    }
    callback_tail = cb;
    CALLBACK_CRITICAL_END;

    port_wake_main_task();
}

void PLACE_IN_ITCM(background_callback_add)(background_callback_t * cb, background_callback_fun fun, void *data) {
    cb->fun = fun;
    cb->data = data;
    background_callback_add_core(cb);
}

inline bool background_callback_pending(void) {
    return callback_head != NULL;
}

static int background_prevention_count;

void PLACE_IN_ITCM(background_callback_run_all)() {
    port_background_task();
    if (!background_callback_pending()) {
        return;
    }
    CALLBACK_CRITICAL_BEGIN;
    if (background_prevention_count) {
        CALLBACK_CRITICAL_END;
        return;
    }
    ++background_prevention_count;
    background_callback_t *cb = (background_callback_t *)callback_head;
    callback_head = NULL;
    callback_tail = NULL;
    while (cb) {
        background_callback_t *next = cb->next;
        cb->next = cb->prev = NULL;
        background_callback_fun fun = cb->fun;
        void *data = cb->data;
        CALLBACK_CRITICAL_END;
        // Leave the critical section in order to run the callback function
        if (fun) {
            fun(data);
        }
        CALLBACK_CRITICAL_BEGIN;
        cb = next;
    }
    --background_prevention_count;
    CALLBACK_CRITICAL_END;
}

void background_callback_prevent() {
    CALLBACK_CRITICAL_BEGIN;
    ++background_prevention_count;
    CALLBACK_CRITICAL_END;
}

void background_callback_allow() {
    CALLBACK_CRITICAL_BEGIN;
    --background_prevention_count;
    CALLBACK_CRITICAL_END;
}


// Filter out queued callbacks if they are allocated on the heap.
void background_callback_reset() {
    background_callback_t *new_head = NULL;
    background_callback_t **previous_next = &new_head;
    background_callback_t *new_tail = NULL;
    CALLBACK_CRITICAL_BEGIN;
    background_callback_t *cb = (background_callback_t *)callback_head;
    while (cb) {
        background_callback_t *next = cb->next;
        cb->next = NULL;
        // Unlink any callbacks that are allocated on the python heap or if they
        // reference data on the python heap. The python heap will be disappear
        // soon after this.
        if (gc_ptr_on_heap((void *)cb) || gc_ptr_on_heap(cb->data)) {
            cb->prev = NULL; // Used to indicate a callback isn't queued.
        } else {
            // Set .next of the previous callback.
            *previous_next = cb;
            // Set our .next for the next callback.
            previous_next = &cb->next;
            // Set our prev to the last callback.
            cb->prev = new_tail;
            // Now we're the tail of the list.
            new_tail = cb;
        }
        cb = next;
    }
    callback_head = new_head;
    callback_tail = new_tail;
    background_prevention_count = 0;
    CALLBACK_CRITICAL_END;
}

void background_callback_gc_collect(void) {
    // We don't enter the callback critical section here.  We rely on
    // gc_collect_ptr _NOT_ entering background callbacks, so it is not
    // possible for the list to be cleared.
    //
    // However, it is possible for the list to be extended.  We make the
    // minor assumption that no newly added callback is for a
    // collectable object.  That is, we only plug the hole where an
    // object becomes collectable AFTER it is added but before the
    // callback is run, not the hole where an object was ALREADY
    // collectable but adds a background task for itself.
    //
    // It's necessary to traverse the whole list here, as the callbacks
    // themselves can be in non-gc memory, and some of the cb->data
    // objects themselves might be in non-gc memory.
    background_callback_t *cb = (background_callback_t *)callback_head;
    while (cb) {
        gc_collect_ptr(cb->data);
        cb = cb->next;
    }
}
