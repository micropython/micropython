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

#include <string.h>

#include "py/gc.h"
#include "py/mpconfig.h"
#include "supervisor/background_callback.h"
#include "supervisor/linker.h"
#include "supervisor/shared/tick.h"
#include "shared-bindings/microcontroller/__init__.h"

STATIC volatile background_callback_t *volatile callback_head, *volatile callback_tail;

#define CALLBACK_CRITICAL_BEGIN (common_hal_mcu_disable_interrupts())
#define CALLBACK_CRITICAL_END (common_hal_mcu_enable_interrupts())

MP_WEAK void port_wake_main_task(void) {
}

void background_callback_add_core(background_callback_t *cb) {
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

void background_callback_add(background_callback_t *cb, background_callback_fun fun, void *data) {
    cb->fun = fun;
    cb->data = data;
    background_callback_add_core(cb);
}

static bool in_background_callback;
void PLACE_IN_ITCM(background_callback_run_all)() {
    if (!callback_head) {
        return;
    }
    CALLBACK_CRITICAL_BEGIN;
    if (in_background_callback) {
        CALLBACK_CRITICAL_END;
        return;
    }
    in_background_callback = true;
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
    in_background_callback = false;
    CALLBACK_CRITICAL_END;
}

void background_callback_begin_critical_section() {
    CALLBACK_CRITICAL_BEGIN;
}

void background_callback_end_critical_section() {
    CALLBACK_CRITICAL_END;
}

void background_callback_reset() {
    CALLBACK_CRITICAL_BEGIN;
    background_callback_t *cb = (background_callback_t *)callback_head;
    while (cb) {
        background_callback_t *next = cb->next;
        memset(cb, 0, sizeof(*cb));
        cb = next;
    }
    callback_head = NULL;
    callback_tail = NULL;
    in_background_callback = false;
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
