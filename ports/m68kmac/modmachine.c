// This file is never compiled standalone, it's included directly from
// extmod/modmachine.c via MICROPY_PY_MACHINE_INCLUDEFILE.

#include "uart_core.h"

static void mp_machine_idle(void) {
    mp_console_idle(true);
}

static mp_obj_t machine_show_console_fn(void) {
    mp_console_showhide(true);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(machine_show_console_obj, machine_show_console_fn);

static mp_obj_t machine_hide_console_fn(void) {
    mp_console_showhide(false);
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_0(machine_hide_console_obj, machine_hide_console_fn);

#define MICROPY_PY_MACHINE_EXTRA_GLOBALS \
    { MP_ROM_QSTR(MP_QSTR_ShowConsole), MP_ROM_PTR(&machine_show_console_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_HideConsole), MP_ROM_PTR(&machine_hide_console_obj) }, \

