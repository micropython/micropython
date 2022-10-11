/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Dan Halbert for Adafruit Industries
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

#include <stdbool.h>
#include "py/obj.h"
#include "shared-bindings/supervisor/StatusBar.h"
#include "shared-bindings/supervisor/__init__.h"
#include "supervisor/shared/display.h"
#include "supervisor/shared/status_bar.h"

#if CIRCUITPY_TERMINALIO
#include "shared-module/terminalio/Terminal.h"
#endif

bool shared_module_supervisor_status_bar_get_console(supervisor_status_bar_obj_t *self) {
    return self->console;
}

void shared_module_supervisor_status_bar_set_console(supervisor_status_bar_obj_t *self, bool enabled) {
    if (self->console == enabled) {
        // Do nothing if not changing the state.
        return;
    }

    if (self->updated) {
        // Clear before changing state. If disabling, will remain cleared.
        supervisor_status_bar_clear();
    }

    self->console = enabled;

    // Update may be ignored.
    supervisor_status_bar_update();
}

bool shared_module_supervisor_status_bar_get_display(supervisor_status_bar_obj_t *self) {
    return self->display;
}

#if CIRCUITPY_TERMINALIO
void shared_module_supervisor_status_bar_set_display(supervisor_status_bar_obj_t *self, bool enabled) {
    if (self->display == enabled) {
        // Do nothing if not changing the state.
        return;
    }

    if (self->updated) {
        // Clear before changing state. If disabling, will remain cleared.
        terminalio_terminal_clear_status_bar(&supervisor_terminal);
    }

    self->display = enabled;

    // Update may be ignored.
    supervisor_status_bar_update();
}
#endif

void shared_module_supervisor_status_bar_init(supervisor_status_bar_obj_t *self) {
    self->console = true;
    self->display = true;
    self->updated = false;
}

void shared_module_supervisor_status_bar_updated(supervisor_status_bar_obj_t *self) {
    self->updated = true;
}
