/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2017 Scott Shawcroft for Adafruit Industries
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
#include "py/obj.h"
#include "py/runtime.h"
#include "py/reload.h"

#include "lib/utils/interrupt_char.h"
#include "supervisor/shared/autoreload.h"
#include "supervisor/shared/rgb_led_status.h"
#include "supervisor/shared/stack.h"
#include "supervisor/shared/translate.h"
#include "supervisor/shared/workflow.h"

#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/supervisor/__init__.h"
#include "shared-bindings/supervisor/Runtime.h"

//| """Supervisor settings"""
//|

//| runtime: Runtime
//| """Runtime information, such as ``runtime.serial_connected``
//| (USB serial connection status).
//| This object is the sole instance of `supervisor.Runtime`."""
//|

//|     def allow_deep_sleep(*, when_connected: bool = False, on_error: bool = False):
//|         """Configure when CircuitPython can go into deep sleep. Deep sleep can occur
//|         after a program has finished running or when `supervisor.deep_sleep_now()` is called.
//|
//|         :param bool when_connected: If ``True``, CircuitPython will go into deep sleep
//|         when a program finishes, even if it is connected to a host computer over USB or other means.
//|         It will disconnect from the host before sleeping.
//|         If ``False``, deep sleep will not be entered if connected.
//|         :param bool on_error: If ``True``, deep sleep will be entered if even a program
//|         terminated due to an exception or fatal error. If ``False``, an error will cause
//|         CircuitPython to stay awake, flashing error codes on the status RGB LED, if available.
//|         ...
//|
STATIC mp_obj_t supervisor_allow_deep_sleep(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_when_connected, ARG_on_error };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_when_connected, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_on_error, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    supervisor_workflow_set_allow_deep_sleep_when_connected(args[ARG_when_connected].u_bool);
    supervisor_workflow_set_allow_deep_sleep_on_error(args[ARG_on_error].u_bool);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(supervisor_allow_deep_sleep_obj, 0, supervisor_allow_deep_sleep);

//|     def deep_sleep(): -> None
//|     """Go into deep sleep mode immediately, if not connected to a host computer.
//|     But if connected and `supervisor.runtime.allow_deep_sleep(when_connected=true)`
//|     has not been called, simply restart.
//|

STATIC mp_obj_t supervisor_deep_sleep(void) {
    common_hal_mcu_deep_sleep();
}
MP_DEFINE_CONST_FUN_OBJ_0(supervisor_deep_sleep_obj, supervisor_deep_sleep);

//| def enable_autoreload() -> None:
//|     """Enable autoreload based on USB file write activity."""
//|     ...
//|
STATIC mp_obj_t supervisor_enable_autoreload(void) {
    autoreload_enable();
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(supervisor_enable_autoreload_obj, supervisor_enable_autoreload);

//| def disable_autoreload() -> None:
//|     """Disable autoreload based on USB file write activity until
//|     `enable_autoreload` is called."""
//|     ...
//|
STATIC mp_obj_t supervisor_disable_autoreload(void) {
    autoreload_disable();
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(supervisor_disable_autoreload_obj, supervisor_disable_autoreload);

//| def set_rgb_status_brightness(brightness: int) -> None:
//|     """Set brightness of status neopixel from 0-255
//|     `set_rgb_status_brightness` is called."""
//|     ...
//|
STATIC mp_obj_t supervisor_set_rgb_status_brightness(mp_obj_t lvl){
      // This must be int. If cast to uint8_t first, will never raise a ValueError.
      int brightness_int = mp_obj_get_int(lvl);
      if(brightness_int < 0 || brightness_int > 255){
            mp_raise_ValueError(translate("Brightness must be between 0 and 255"));
      }
      set_rgb_status_brightness((uint8_t)brightness_int);
      return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(supervisor_set_rgb_status_brightness_obj, supervisor_set_rgb_status_brightness);

//| def reload() -> None:
//|     """Reload the main Python code and run it (equivalent to hitting Ctrl-D at the REPL)."""
//|     ...
//|
STATIC mp_obj_t supervisor_reload(void) {
    reload_requested = true;
    supervisor_set_run_reason(RUN_REASON_SUPERVISOR_RELOAD);
    mp_raise_reload_exception();
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(supervisor_reload_obj, supervisor_reload);

//| def set_next_stack_limit(size: int) -> None:
//|     """Set the size of the stack for the next vm run. If its too large, the default will be used."""
//|     ...
//|
STATIC mp_obj_t supervisor_set_next_stack_limit(mp_obj_t size_obj) {
    mp_int_t size = mp_obj_get_int(size_obj);

    if (size < 256) {
        mp_raise_ValueError(translate("Stack size must be at least 256"));
    }
    set_next_stack_size(size);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(supervisor_set_next_stack_limit_obj, supervisor_set_next_stack_limit);

STATIC const mp_rom_map_elem_t supervisor_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_supervisor) },
    { MP_ROM_QSTR(MP_QSTR_allow_deep_sleep),  MP_ROM_PTR(&supervisor_allow_deep_sleep_obj) },
    { MP_ROM_QSTR(MP_QSTR_deep_sleep),  MP_ROM_PTR(&supervisor_deep_sleep_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable_autoreload),  MP_ROM_PTR(&supervisor_enable_autoreload_obj) },
    { MP_ROM_QSTR(MP_QSTR_disable_autoreload),  MP_ROM_PTR(&supervisor_disable_autoreload_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_rgb_status_brightness),  MP_ROM_PTR(&supervisor_set_rgb_status_brightness_obj) },
    { MP_ROM_QSTR(MP_QSTR_runtime),  MP_ROM_PTR(&common_hal_supervisor_runtime_obj) },
    { MP_ROM_QSTR(MP_QSTR_reload),  MP_ROM_PTR(&supervisor_reload_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_next_stack_limit),  MP_ROM_PTR(&supervisor_set_next_stack_limit_obj) },

};

STATIC MP_DEFINE_CONST_DICT(supervisor_module_globals, supervisor_module_globals_table);

const mp_obj_module_t supervisor_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&supervisor_module_globals,
};
