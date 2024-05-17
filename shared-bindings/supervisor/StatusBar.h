// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 by Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_STATUS_BAR_STATUS_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_STATUS_BAR_STATUS_H

#include <stdbool.h>
#include "py/obj.h"
#include "shared-module/supervisor/StatusBar.h"

extern const mp_obj_type_t supervisor_status_bar_type;

bool shared_module_supervisor_status_bar_get_console(supervisor_status_bar_obj_t *self);
void shared_module_supervisor_status_bar_set_console(supervisor_status_bar_obj_t *self, bool enabled);

bool shared_module_supervisor_status_bar_get_display(supervisor_status_bar_obj_t *self);
void shared_module_supervisor_status_bar_set_display(supervisor_status_bar_obj_t *self, bool enabled);

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_SUPERVISOR_STATUS_BAR_H
