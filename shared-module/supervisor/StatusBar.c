// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

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

#if CIRCUITPY_STATUS_BAR && CIRCUITPY_TERMINALIO
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
