// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 by Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdbool.h>
#include "py/obj.h"
#include "py/enum.h"
#include "py/runtime.h"
#include "py/objproperty.h"
#include "shared-bindings/supervisor/StatusBar.h"

//| class StatusBar:
//|     """Current status of runtime objects.
//|
//|     Usage::
//|
//|        import supervisor
//|
//|        supervisor.status_bar.console = False
//|     """
//|

//|     def __init__(self) -> None:
//|         """You cannot create an instance of `supervisor.StatusBar`.
//|         Use `supervisor.status_bar` to access the sole instance available."""
//|         ...

//|     console: bool
//|     """Whether status bar information is sent over the console (REPL) serial connection,
//|     using OSC terminal escape codes that change the terminal's title. Default is ``True``.
//|     If set to ``False``, status bar will be cleared and then disabled.
//|     May be set in ``boot.py`` or later. Persists across soft restarts.
//|     """
static mp_obj_t supervisor_status_bar_get_console(mp_obj_t self_in) {
    #if CIRCUITPY_STATUS_BAR
    supervisor_status_bar_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(shared_module_supervisor_status_bar_get_console(self));
    #else
    mp_raise_NotImplementedError(NULL);
    #endif
}
MP_DEFINE_CONST_FUN_OBJ_1(supervisor_status_bar_get_console_obj, supervisor_status_bar_get_console);

static mp_obj_t supervisor_status_bar_set_console(mp_obj_t self_in, mp_obj_t state_in) {
    #if CIRCUITPY_STATUS_BAR
    supervisor_status_bar_obj_t *self = MP_OBJ_TO_PTR(self_in);
    shared_module_supervisor_status_bar_set_console(self, mp_obj_is_true(state_in));
    return mp_const_none;
    #else
    mp_raise_NotImplementedError(NULL);
    #endif
}
MP_DEFINE_CONST_FUN_OBJ_2(supervisor_status_bar_set_console_obj, supervisor_status_bar_set_console);

MP_PROPERTY_GETSET(supervisor_status_bar_console_obj,
    (mp_obj_t)&supervisor_status_bar_get_console_obj,
    (mp_obj_t)&supervisor_status_bar_set_console_obj);

//|     display: bool
//|     """Whether status bar information is displayed on the top line of the display.
//|     Default is ``True``. If set to ``False``, status bar will be cleared and then disabled.
//|     May be set in ``boot.py`` or later.  Persists across soft restarts.
//|     Not available if `terminalio` is not available.
//|     """
//|
static mp_obj_t supervisor_status_bar_get_display(mp_obj_t self_in) {
    #if CIRCUITPY_STATUS_BAR && CIRCUITPY_TERMINALIO
    supervisor_status_bar_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(shared_module_supervisor_status_bar_get_display(self));
    #else
    mp_raise_NotImplementedError(NULL);
    #endif
}
MP_DEFINE_CONST_FUN_OBJ_1(supervisor_status_bar_get_display_obj, supervisor_status_bar_get_display);

static mp_obj_t supervisor_status_bar_set_display(mp_obj_t self_in, mp_obj_t state_in) {
    #if CIRCUITPY_STATUS_BAR && CIRCUITPY_TERMINALIO
    supervisor_status_bar_obj_t *self = MP_OBJ_TO_PTR(self_in);
    shared_module_supervisor_status_bar_set_display(self, mp_obj_is_true(state_in));
    return mp_const_none;
    #else
    mp_raise_NotImplementedError(NULL);
    #endif
}
MP_DEFINE_CONST_FUN_OBJ_2(supervisor_status_bar_set_display_obj, supervisor_status_bar_set_display);

MP_PROPERTY_GETSET(supervisor_status_bar_display_obj,
    (mp_obj_t)&supervisor_status_bar_get_display_obj,
    (mp_obj_t)&supervisor_status_bar_set_display_obj);


static const mp_rom_map_elem_t supervisor_status_bar_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_console), MP_ROM_PTR(&supervisor_status_bar_console_obj) },
    { MP_ROM_QSTR(MP_QSTR_display), MP_ROM_PTR(&supervisor_status_bar_display_obj) },
};

static MP_DEFINE_CONST_DICT(supervisor_status_bar_locals_dict, supervisor_status_bar_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    supervisor_status_bar_type,
    MP_QSTR_Status_Bar,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    locals_dict, &supervisor_status_bar_locals_dict
    );
