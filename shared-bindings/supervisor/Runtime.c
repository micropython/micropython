// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Michael Schroeder
//
// SPDX-License-Identifier: MIT

#include <stdbool.h>
#include "py/obj.h"
#include "py/enum.h"
#include "py/runtime.h"
#include "py/objproperty.h"

#include "shared-bindings/supervisor/RunReason.h"
#include "shared-bindings/supervisor/Runtime.h"
#include "shared-bindings/supervisor/SafeModeReason.h"

#include "supervisor/shared/reload.h"
#include "supervisor/shared/serial.h"
#include "supervisor/shared/stack.h"
#include "supervisor/shared/status_leds.h"
#include "supervisor/shared/bluetooth/bluetooth.h"

#if CIRCUITPY_USB_DEVICE
#include "supervisor/usb.h"
#endif

#if CIRCUITPY_DISPLAYIO
#include "shared-bindings/displayio/__init__.h"
#endif

static supervisor_run_reason_t _run_reason;

// TODO: add REPL to description once it is operational

//| class Runtime:
//|     """Current status of runtime objects.
//|
//|     Usage::
//|
//|        import supervisor
//|        if supervisor.runtime.serial_connected:
//|            print("Hello World!")"""
//|

//|     def __init__(self) -> None:
//|         """You cannot create an instance of `supervisor.Runtime`.
//|         Use `supervisor.runtime` to access the sole instance available."""
//|         ...
//|

//|     usb_connected: bool
//|     """Returns the USB enumeration status (read-only)."""
static mp_obj_t supervisor_runtime_get_usb_connected(mp_obj_t self) {
    #if CIRCUITPY_USB_DEVICE
    return mp_obj_new_bool(usb_connected());
    #else
    return mp_const_false;
    #endif
}
MP_DEFINE_CONST_FUN_OBJ_1(supervisor_runtime_get_usb_connected_obj, supervisor_runtime_get_usb_connected);

MP_PROPERTY_GETTER(supervisor_runtime_usb_connected_obj,
    (mp_obj_t)&supervisor_runtime_get_usb_connected_obj);

//|     serial_connected: bool
//|     """Returns the USB serial communication status (read-only)."""
static mp_obj_t supervisor_runtime_get_serial_connected(mp_obj_t self) {
    return mp_obj_new_bool(serial_connected());
}
MP_DEFINE_CONST_FUN_OBJ_1(supervisor_runtime_get_serial_connected_obj, supervisor_runtime_get_serial_connected);

MP_PROPERTY_GETTER(supervisor_runtime_serial_connected_obj,
    (mp_obj_t)&supervisor_runtime_get_serial_connected_obj);

//|     serial_bytes_available: int
//|     """Returns the number of bytes are available to read on the console serial input.
//|     Multiple console serial inputs may be in use at once, including
//|     USB, web workflow, BLE workflow, and/or UART.
//|
//|     Allows for polling to see whether to call the built-in input() or wait. (read-only)
//|
//|     **Limitations**: On STM, UART (not USB) console input can only determine that at least one character
//|     is available, and so if only the UART console is in use, only ``1`` or ``0`` will be returned.
//|
//|     Changed in version 9.1.0: Previously returned only ``True`` or ``False``.
//|     Since ``0`` acts as ``False``, ``if supervisor.runtime.serial_byes_available:``
//|     will still work.
//|     """
static mp_obj_t supervisor_runtime_get_serial_bytes_available(mp_obj_t self) {
    return MP_OBJ_NEW_SMALL_INT(serial_bytes_available());
}
MP_DEFINE_CONST_FUN_OBJ_1(supervisor_runtime_get_serial_bytes_available_obj, supervisor_runtime_get_serial_bytes_available);

MP_PROPERTY_GETTER(supervisor_runtime_serial_bytes_available_obj,
    (mp_obj_t)&supervisor_runtime_get_serial_bytes_available_obj);

supervisor_run_reason_t supervisor_get_run_reason(void) {
    return _run_reason;
}

void supervisor_set_run_reason(supervisor_run_reason_t run_reason) {
    _run_reason = run_reason;
}

//|     run_reason: RunReason
//|     """Why CircuitPython started running this particular time (read-only)."""
static mp_obj_t supervisor_runtime_get_run_reason(mp_obj_t self) {
    return cp_enum_find(&supervisor_run_reason_type, _run_reason);
}
MP_DEFINE_CONST_FUN_OBJ_1(supervisor_runtime_get_run_reason_obj, supervisor_runtime_get_run_reason);

MP_PROPERTY_GETTER(supervisor_runtime_run_reason_obj,
    (mp_obj_t)&supervisor_runtime_get_run_reason_obj);

//|     safe_mode_reason: SafeModeReason
//|     """Why CircuitPython went into safe mode this particular time (read-only).
//|
//|     **Limitations**: Raises ``NotImplementedError`` on builds that do not implement ``safemode.py``.
//|     """
static mp_obj_t supervisor_runtime_get_safe_mode_reason(mp_obj_t self) {
    #if CIRCUITPY_SAFEMODE_PY
    return cp_enum_find(&supervisor_safe_mode_reason_type, get_safe_mode());
    #else
    mp_raise_NotImplementedError(NULL);
    #endif
}
MP_DEFINE_CONST_FUN_OBJ_1(supervisor_runtime_get_safe_mode_reason_obj, supervisor_runtime_get_safe_mode_reason);

MP_PROPERTY_GETTER(supervisor_runtime_safe_mode_reason_obj,
    (mp_obj_t)&supervisor_runtime_get_safe_mode_reason_obj);

//|     autoreload: bool
//|     """Whether CircuitPython may autoreload based on workflow writes to the filesystem."""
//|
static mp_obj_t supervisor_runtime_get_autoreload(mp_obj_t self) {
    return mp_obj_new_bool(autoreload_is_enabled());
}
MP_DEFINE_CONST_FUN_OBJ_1(supervisor_runtime_get_autoreload_obj, supervisor_runtime_get_autoreload);

static mp_obj_t supervisor_runtime_set_autoreload(mp_obj_t self, mp_obj_t state_in) {
    if (mp_obj_is_true(state_in)) {
        autoreload_enable();
    } else {
        autoreload_disable();
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(supervisor_runtime_set_autoreload_obj, supervisor_runtime_set_autoreload);

MP_PROPERTY_GETSET(supervisor_runtime_autoreload_obj,
    (mp_obj_t)&supervisor_runtime_get_autoreload_obj,
    (mp_obj_t)&supervisor_runtime_set_autoreload_obj);

//|     ble_workflow: bool
//|     """Enable/Disable ble workflow until a reset. This prevents BLE advertising outside of the VM and
//|     the services used for it.
//|     If ``CIRCUITPY_BLE_WORKFLOW=false`` is present in ``settings.toml``, setting `ble_workflow`
//|     to ``True`` has no effect.
//|     """
//|
static mp_obj_t supervisor_runtime_get_ble_workflow(mp_obj_t self) {
    #if CIRCUITPY_BLE_FILE_SERVICE && CIRCUITPY_BLE_SERIAL_SERVICE
    return mp_obj_new_bool(supervisor_bluetooth_workflow_is_enabled());
    #else
    return mp_const_false;
    #endif
}
MP_DEFINE_CONST_FUN_OBJ_1(supervisor_runtime_get_ble_workflow_obj, supervisor_runtime_get_ble_workflow);

static mp_obj_t supervisor_runtime_set_ble_workflow(mp_obj_t self, mp_obj_t state_in) {
    #if CIRCUITPY_BLE_FILE_SERVICE && CIRCUITPY_BLE_SERIAL_SERVICE
    if (mp_obj_is_true(state_in)) {
        supervisor_bluetooth_enable_workflow();
    } else {
        supervisor_bluetooth_disable_workflow();
    }
    #else
    mp_raise_NotImplementedError(NULL);
    #endif
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(supervisor_runtime_set_ble_workflow_obj, supervisor_runtime_set_ble_workflow);

MP_PROPERTY_GETSET(supervisor_runtime_ble_workflow_obj,
    (mp_obj_t)&supervisor_runtime_get_ble_workflow_obj,
    (mp_obj_t)&supervisor_runtime_set_ble_workflow_obj);

//|     rgb_status_brightness: int
//|     """Set brightness of status RGB LED from 0-255. This will take effect
//|     after the current code finishes and the status LED is used to show
//|     the finish state."""
//|
static mp_obj_t supervisor_runtime_get_rgb_status_brightness(mp_obj_t self) {
    return MP_OBJ_NEW_SMALL_INT(get_status_brightness());
}
MP_DEFINE_CONST_FUN_OBJ_1(supervisor_runtime_get_rgb_status_brightness_obj, supervisor_runtime_get_rgb_status_brightness);

static mp_obj_t supervisor_runtime_set_rgb_status_brightness(mp_obj_t self, mp_obj_t lvl) {
    #if CIRCUITPY_STATUS_LED
    // This must be int. If cast to uint8_t first, will never raise a ValueError.
    set_status_brightness((uint8_t)mp_arg_validate_int_range(mp_obj_get_int(lvl), 0, 255, MP_QSTR_brightness));
    #else
    mp_raise_NotImplementedError(NULL);
    #endif
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(supervisor_runtime_set_rgb_status_brightness_obj, supervisor_runtime_set_rgb_status_brightness);

MP_PROPERTY_GETSET(supervisor_runtime_rgb_status_brightness_obj,
    (mp_obj_t)&supervisor_runtime_get_rgb_status_brightness_obj,
    (mp_obj_t)&supervisor_runtime_set_rgb_status_brightness_obj);

#if CIRCUITPY_DISPLAYIO
//|     display: displayio.AnyDisplay | None
//|     """The primary configured displayio display, if any.
//|
//|     If the board has a display that is hard coded, or that was explicitly set
//|     in boot.py or code.py (including a previous run of code.py), it is
//|     available here until it is released with ``displayio.release_displays()``.
//|
//|     The display can be of any supported display type, such as `busdisplay.BusDisplay`.
//|
//|     If no display is configured, this property is `None`.
//|
//|     Any display that is not the primary display is automatically released at the end of
//|     running a code file.
//|
//|     On boards without displayio, this property is present but the value is always `None`."""
//|
//|
static mp_obj_t supervisor_runtime_get_display(mp_obj_t self) {
    return common_hal_displayio_get_primary_display();
}
MP_DEFINE_CONST_FUN_OBJ_1(supervisor_runtime_get_display_obj, supervisor_runtime_get_display);
static mp_obj_t supervisor_runtime_set_display(mp_obj_t self, mp_obj_t new_primary_display) {
    common_hal_displayio_set_primary_display(new_primary_display);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(supervisor_runtime_set_display_obj, supervisor_runtime_set_display);

MP_PROPERTY_GETSET(supervisor_runtime_display_obj,
    (mp_obj_t)&supervisor_runtime_get_display_obj,
    (mp_obj_t)&supervisor_runtime_set_display_obj);
#endif

MP_WEAK mp_obj_t common_hal_supervisor_runtime_get_audio(void) {
    return mp_const_none;
}

//|     audio: picodvi.AudioOut | None
//|     """The default audio output for the supervisor and user code.
//|
//|     On boards with a default DVI display this is a `picodvi.AudioOut` that
//|     sends sound over the DVI cable along with the picture. It is `None` when
//|     the display did not start, when audio is turned off with
//|     ``CIRCUITPY_PICODVI_AUDIO = false`` in ``settings.toml``, or after the
//|     display is released with ``displayio.release_displays()``.
//|
//|     On boards without such an output, this property is always `None`."""
//|
static mp_obj_t supervisor_runtime_get_audio(mp_obj_t self) {
    return common_hal_supervisor_runtime_get_audio();
}
MP_DEFINE_CONST_FUN_OBJ_1(supervisor_runtime_get_audio_obj, supervisor_runtime_get_audio);

MP_PROPERTY_GETTER(supervisor_runtime_audio_obj,
    (mp_obj_t)&supervisor_runtime_get_audio_obj);

static const mp_rom_map_elem_t supervisor_runtime_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_usb_connected), MP_ROM_PTR(&supervisor_runtime_usb_connected_obj) },
    { MP_ROM_QSTR(MP_QSTR_serial_connected), MP_ROM_PTR(&supervisor_runtime_serial_connected_obj) },
    { MP_ROM_QSTR(MP_QSTR_serial_bytes_available), MP_ROM_PTR(&supervisor_runtime_serial_bytes_available_obj) },
    { MP_ROM_QSTR(MP_QSTR_run_reason), MP_ROM_PTR(&supervisor_runtime_run_reason_obj) },
    { MP_ROM_QSTR(MP_QSTR_safe_mode_reason), MP_ROM_PTR(&supervisor_runtime_safe_mode_reason_obj) },
    { MP_ROM_QSTR(MP_QSTR_autoreload), MP_ROM_PTR(&supervisor_runtime_autoreload_obj) },
    { MP_ROM_QSTR(MP_QSTR_ble_workflow),  MP_ROM_PTR(&supervisor_runtime_ble_workflow_obj) },
    { MP_ROM_QSTR(MP_QSTR_rgb_status_brightness),  MP_ROM_PTR(&supervisor_runtime_rgb_status_brightness_obj) },
    #if CIRCUITPY_DISPLAYIO
    { MP_ROM_QSTR(MP_QSTR_display),  MP_ROM_PTR(&supervisor_runtime_display_obj) },
    #else
    { MP_ROM_QSTR(MP_QSTR_display),  MP_ROM_NONE },
    #endif
    { MP_ROM_QSTR(MP_QSTR_audio),  MP_ROM_PTR(&supervisor_runtime_audio_obj) },
};

static MP_DEFINE_CONST_DICT(supervisor_runtime_locals_dict, supervisor_runtime_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    supervisor_runtime_type,
    MP_QSTR_Runtime,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    locals_dict, &supervisor_runtime_locals_dict
    );
