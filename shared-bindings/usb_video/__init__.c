// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/usb_video/__init__.h"
#include "shared-bindings/usb_video/USBFramebuffer.h"
#include "shared-module/usb_video/__init__.h"

//| """Allows streaming bitmaps to a host computer via USB
//|
//| This makes your CircuitPython device identify to the host computer as a video camera.
//| This mode is also known as "USB UVC".
//|
//| This mode requires 1 IN endpoint. Generally, microcontrollers have a limit on
//| the number of endpoints. If you exceed the number of endpoints, CircuitPython
//| will automatically enter Safe Mode. Even in this case, you may be able to
//| enable USB video by also disabling other USB functions, such as `usb_hid` or
//| `usb_midi`.
//|
//| To enable this mode, you must configure the framebuffer size in ``boot.py`` and then
//| create a display in ``code.py``.
//|
//| .. code-block:: py
//|
//|     # boot.py
//|     import usb_video
//|     usb_video.enable_framebuffer(128, 96)
//|
//| .. code-block:: py
//|
//|     # code.py
//|     import usb_video
//|     import framebufferio
//|     import displayio
//|
//|     displayio.release_displays()
//|     display = framebufferio.FramebufferDisplay(usb_video.USBFramebuffer())
//|
//|     # ... use the display object with displayio Group and TileGrid objects
//|
//| This interface is experimental and may change without notice even in stable
//| versions of CircuitPython."""
//|

//| def enable_framebuffer(width: int, height: int) -> None:
//|     """Enable a USB video framebuffer, setting the given width & height
//|
//|     This function may only be used from ``boot.py``.
//|
//|     Width is rounded up to a multiple of 2.
//|
//|     After boot.py completes, the framebuffer will be allocated. Total storage
//|     of 4×``width``×``height`` bytes is required, reducing the amount available
//|     for Python objects. If the allocation fails, a MemoryError is raised.
//|     This message can be seen in ``boot_out.txt``."""
//|

static mp_obj_t usb_video_enable_framebuffer(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_width, ARG_height };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_width, MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_height, MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = 0 } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // (but note that most devices will not be able to allocate this much memory.
    uint32_t width = mp_arg_validate_int_range(args[ARG_width].u_int, 0, 32767, MP_QSTR_width);
    uint32_t height = mp_arg_validate_int_range(args[ARG_height].u_int, 0, 32767, MP_QSTR_height);
    if (!shared_module_usb_video_enable(width, height)) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Cannot change USB devices now"));
    }

    return mp_const_none;
};
static MP_DEFINE_CONST_FUN_OBJ_KW(usb_video_enable_framebuffer_obj, 0, usb_video_enable_framebuffer);

static const mp_rom_map_elem_t usb_video_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_usb_video) },
    { MP_ROM_QSTR(MP_QSTR_USBFramebuffer), MP_ROM_PTR(&usb_video_USBFramebuffer_type) },
    { MP_ROM_QSTR(MP_QSTR_enable_framebuffer), MP_ROM_PTR(&usb_video_enable_framebuffer_obj) },
};

static MP_DEFINE_CONST_DICT(usb_video_module_globals, usb_video_module_globals_table);

const mp_obj_module_t usb_video_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&usb_video_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_usb_video, usb_video_module);
