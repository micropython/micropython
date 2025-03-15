/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 Angus Gratton
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

#include "py/mpconfig.h"

#if MICROPY_HW_ENABLE_USB_RUNTIME_DEVICE

#include "mp_usbd.h"
#include "py/mperrno.h"
#include "py/objstr.h"

// Implements the singleton runtime USB object
//
// Currently this implementation references TinyUSB directly.

#ifndef NO_QSTR
#include "device/usbd_pvt.h"
#endif

#define HAS_BUILTIN_DRIVERS (MICROPY_HW_USB_CDC || MICROPY_HW_USB_MSC)

const mp_obj_type_t machine_usb_device_type;

static mp_obj_t usb_device_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    (void)type;
    (void)n_args;
    (void)n_kw;
    (void)args;

    if (MP_STATE_VM(usbd) == MP_OBJ_NULL) {
        mp_obj_usb_device_t *o = m_new0(mp_obj_usb_device_t, 1);
        o->base.type = &machine_usb_device_type;
        o->desc_dev = mp_const_none;
        o->desc_cfg = mp_const_none;
        o->desc_strs = mp_const_none;
        o->open_itf_cb = mp_const_none;
        o->reset_cb = mp_const_none;
        o->control_xfer_cb = mp_const_none;
        o->xfer_cb = mp_const_none;
        for (int i = 0; i < CFG_TUD_ENDPPOINT_MAX; i++) {
            o->xfer_data[i][0] = mp_const_none;
            o->xfer_data[i][1] = mp_const_none;
        }
        o->builtin_driver = MP_OBJ_FROM_PTR(&mp_type_usb_device_builtin_none);
        o->active = false; // Builtin USB may be active already, but runtime is inactive
        o->trigger = false;
        o->control_data = MP_OBJ_TO_PTR(mp_obj_new_memoryview('B', 0, NULL));
        o->num_pend_excs = 0;
        for (int i = 0; i < MP_USBD_MAX_PEND_EXCS; i++) {
            o->pend_excs[i] = mp_const_none;
        }

        MP_STATE_VM(usbd) = MP_OBJ_FROM_PTR(o);
    }

    return MP_STATE_VM(usbd);
}

// Utility helper to raise an error if USB device is not active
// (or if a change of active state is triggered but not processed.)
static void usb_device_check_active(mp_obj_usb_device_t *usbd) {
    if (!usbd->active || usbd->trigger) {
        mp_raise_OSError(MP_EINVAL);
    }
}

static mp_obj_t usb_device_submit_xfer(mp_obj_t self, mp_obj_t ep, mp_obj_t buffer) {
    mp_obj_usb_device_t *usbd = (mp_obj_usb_device_t *)MP_OBJ_TO_PTR(self);
    int ep_addr;
    mp_buffer_info_t buf_info = { 0 };
    bool result;

    usb_device_check_active(usbd);

    // Unmarshal arguments, raises TypeError if invalid
    ep_addr = mp_obj_get_int(ep);
    mp_get_buffer_raise(buffer, &buf_info, ep_addr & TUSB_DIR_IN_MASK ? MP_BUFFER_READ : MP_BUFFER_RW);

    uint8_t ep_num = tu_edpt_number(ep_addr);
    uint8_t ep_dir = tu_edpt_dir(ep_addr);

    if (ep_num == 0 || ep_num >= CFG_TUD_ENDPPOINT_MAX) {
        // TinyUSB usbd API doesn't range check arguments, so this check avoids
        // out of bounds array access, or submitting transfers on the control endpoint.
        //
        // This C layer doesn't otherwise keep track of which endpoints the host
        // is aware of (or not).
        mp_raise_ValueError(MP_ERROR_TEXT("ep"));
    }

    if (!usbd_edpt_claim(USBD_RHPORT, ep_addr)) {
        mp_raise_OSError(MP_EBUSY);
    }

    result = usbd_edpt_xfer(USBD_RHPORT, ep_addr, buf_info.buf, buf_info.len);

    if (result) {
        // Store the buffer object until the transfer completes
        usbd->xfer_data[ep_num][ep_dir] = buffer;
    }

    return mp_obj_new_bool(result);
}
static MP_DEFINE_CONST_FUN_OBJ_3(usb_device_submit_xfer_obj, usb_device_submit_xfer);

static mp_obj_t usb_device_active(size_t n_args, const mp_obj_t *args) {
    mp_obj_usb_device_t *usbd = (mp_obj_usb_device_t *)MP_OBJ_TO_PTR(args[0]);

    bool result = usbd->active;
    if (n_args == 2) {
        bool value = mp_obj_is_true(args[1]);

        if (value != result) {
            if (value
                && !mp_usb_device_builtin_enabled(usbd)
                && usbd->desc_dev == mp_const_none) {
                // Only allow activating if config() has already been called to set some descriptors, or a
                // built-in driver is enabled
                mp_raise_OSError(MP_EINVAL);
            }

            // Any change to active state is triggered here and processed
            // from the TinyUSB task.
            usbd->active = value;
            usbd->trigger = true;
            if (value) {
                mp_usbd_init(); // Ensure TinyUSB has initialised by this point
            }
            mp_usbd_schedule_task();
        }
    }

    return mp_obj_new_bool(result);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(usb_device_active_obj, 1, 2, usb_device_active);

static mp_obj_t usb_remote_wakeup(mp_obj_t self) {
    return mp_obj_new_bool(tud_remote_wakeup());
}
static MP_DEFINE_CONST_FUN_OBJ_1(usb_remote_wakeup_obj, usb_remote_wakeup);

static mp_obj_t usb_device_stall(size_t n_args, const mp_obj_t *args) {
    mp_obj_usb_device_t *self = (mp_obj_usb_device_t *)MP_OBJ_TO_PTR(args[0]);
    int epnum = mp_obj_get_int(args[1]);

    usb_device_check_active(self);

    mp_obj_t res = mp_obj_new_bool(usbd_edpt_stalled(USBD_RHPORT, epnum));

    if (n_args == 3) { // Set stall state
        mp_obj_t stall = args[2];
        if (mp_obj_is_true(stall)) {
            usbd_edpt_stall(USBD_RHPORT, epnum);
        } else {
            usbd_edpt_clear_stall(USBD_RHPORT, epnum);
        }
    }

    return res;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(usb_device_stall_obj, 2, 3, usb_device_stall);

// Configure the singleton USB device with all of the relevant transfer and descriptor
// callbacks for dynamic devices.
static mp_obj_t usb_device_config(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    mp_obj_usb_device_t *self = (mp_obj_usb_device_t *)MP_OBJ_TO_PTR(pos_args[0]);

    enum { ARG_desc_dev, ARG_desc_cfg, ARG_desc_strs, ARG_open_itf_cb,
           ARG_reset_cb, ARG_control_xfer_cb, ARG_xfer_cb, ARG_active };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_desc_dev, MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_desc_cfg, MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_desc_strs, MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_open_itf_cb, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_reset_cb, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_control_xfer_cb, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_xfer_cb, MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Check descriptor arguments
    mp_obj_t desc_dev = args[ARG_desc_dev].u_obj;
    mp_obj_t desc_cfg = args[ARG_desc_cfg].u_obj;
    mp_obj_t desc_strs = args[ARG_desc_strs].u_obj;
    if (!MP_OBJ_TYPE_HAS_SLOT(mp_obj_get_type(desc_dev), buffer)) {
        mp_raise_ValueError(MP_ERROR_TEXT("desc_dev"));
    }
    if (!MP_OBJ_TYPE_HAS_SLOT(mp_obj_get_type(desc_cfg), buffer)) {
        mp_raise_ValueError(MP_ERROR_TEXT("desc_cfg"));
    }
    if (desc_strs != mp_const_none
        && !MP_OBJ_TYPE_HAS_SLOT(mp_obj_get_type(desc_strs), subscr)) {
        mp_raise_ValueError(MP_ERROR_TEXT("desc_strs"));
    }

    self->desc_dev = desc_dev;
    self->desc_cfg = desc_cfg;
    self->desc_strs = desc_strs;
    self->open_itf_cb = args[ARG_open_itf_cb].u_obj;
    self->reset_cb = args[ARG_reset_cb].u_obj;
    self->control_xfer_cb = args[ARG_control_xfer_cb].u_obj;
    self->xfer_cb = args[ARG_xfer_cb].u_obj;

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(usb_device_config_obj, 1, usb_device_config);

static const MP_DEFINE_BYTES_OBJ(builtin_default_desc_dev_obj,
    &mp_usbd_builtin_desc_dev, sizeof(tusb_desc_device_t));

#if HAS_BUILTIN_DRIVERS
// BUILTIN_DEFAULT Python object holds properties of the built-in USB configuration
// (i.e. values used by the C implementation of TinyUSB devices.)
static const MP_DEFINE_BYTES_OBJ(builtin_default_desc_cfg_obj,
    mp_usbd_builtin_desc_cfg, MP_USBD_BUILTIN_DESC_CFG_LEN);

static const mp_rom_map_elem_t usb_device_builtin_default_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_itf_max), MP_OBJ_NEW_SMALL_INT(USBD_ITF_BUILTIN_MAX) },
    { MP_ROM_QSTR(MP_QSTR_ep_max), MP_OBJ_NEW_SMALL_INT(USBD_EP_BUILTIN_MAX) },
    { MP_ROM_QSTR(MP_QSTR_str_max), MP_OBJ_NEW_SMALL_INT(USBD_STR_BUILTIN_MAX) },
    { MP_ROM_QSTR(MP_QSTR_desc_dev), MP_ROM_PTR(&builtin_default_desc_dev_obj)  },
    { MP_ROM_QSTR(MP_QSTR_desc_cfg), MP_ROM_PTR(&builtin_default_desc_cfg_obj) },
};
static MP_DEFINE_CONST_DICT(usb_device_builtin_default_dict, usb_device_builtin_default_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_usb_device_builtin_default,
    MP_QSTR_BUILTIN_DEFAULT,
    MP_TYPE_FLAG_NONE,
    locals_dict, &usb_device_builtin_default_dict
    );
#endif // HAS_BUILTIN_DRIVERS

// BUILTIN_NONE holds properties for no enabled built-in USB device support
static const mp_rom_map_elem_t usb_device_builtin_none_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_itf_max), MP_OBJ_NEW_SMALL_INT(0) },
    { MP_ROM_QSTR(MP_QSTR_ep_max), MP_OBJ_NEW_SMALL_INT(0) },
    { MP_ROM_QSTR(MP_QSTR_str_max), MP_OBJ_NEW_SMALL_INT(1) },
    { MP_ROM_QSTR(MP_QSTR_desc_dev), MP_ROM_PTR(&builtin_default_desc_dev_obj)  },
    { MP_ROM_QSTR(MP_QSTR_desc_cfg), mp_const_empty_bytes },
};
static MP_DEFINE_CONST_DICT(usb_device_builtin_none_dict, usb_device_builtin_none_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_usb_device_builtin_none,
    MP_QSTR_BUILTIN_NONE,
    MP_TYPE_FLAG_NONE,
    locals_dict, &usb_device_builtin_none_dict
    );

static const mp_rom_map_elem_t usb_device_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_config), MP_ROM_PTR(&usb_device_config_obj) },
    { MP_ROM_QSTR(MP_QSTR_submit_xfer), MP_ROM_PTR(&usb_device_submit_xfer_obj) },
    { MP_ROM_QSTR(MP_QSTR_active), MP_ROM_PTR(&usb_device_active_obj) },
    { MP_ROM_QSTR(MP_QSTR_stall), MP_ROM_PTR(&usb_device_stall_obj) },
    { MP_ROM_QSTR(MP_QSTR_remote_wakeup), MP_ROM_PTR(&usb_remote_wakeup_obj) },

    // Built-in driver constants
    { MP_ROM_QSTR(MP_QSTR_BUILTIN_NONE), MP_ROM_PTR(&mp_type_usb_device_builtin_none) },

    #if !HAS_BUILTIN_DRIVERS
    // No builtin-in drivers, so BUILTIN_DEFAULT is BUILTIN_NONE
    { MP_ROM_QSTR(MP_QSTR_BUILTIN_DEFAULT), MP_ROM_PTR(&mp_type_usb_device_builtin_none) },
    #else
    { MP_ROM_QSTR(MP_QSTR_BUILTIN_DEFAULT), MP_ROM_PTR(&mp_type_usb_device_builtin_default) },

    // Specific driver constant names are to support future switching of built-in drivers,
    // but currently only one is present and it maps directly to BUILTIN_DEFAULT
    #if MICROPY_HW_USB_CDC && !MICROPY_HW_USB_MSC
    { MP_ROM_QSTR(MP_QSTR_BUILTIN_CDC), MP_ROM_PTR(&mp_type_usb_device_builtin_default) },
    #endif
    #if MICROPY_HW_USB_MSC && !MICROPY_HW_USB_CDC
    { MP_ROM_QSTR(MP_QSTR_BUILTIN_MSC), MP_ROM_PTR(&mp_type_usb_device_builtin_default) },
    #endif
    #if MICROPY_HW_USB_CDC && MICROPY_HW_USB_MSC
    { MP_ROM_QSTR(MP_QSTR_BUILTIN_CDC_MSC), MP_ROM_PTR(&mp_type_usb_device_builtin_default) },
    #endif
    #endif // !HAS_BUILTIN_DRIVERS
};
static MP_DEFINE_CONST_DICT(usb_device_locals_dict, usb_device_locals_dict_table);

static void usb_device_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    mp_obj_usb_device_t *self = MP_OBJ_TO_PTR(self_in);
    if (dest[0] == MP_OBJ_NULL) {
        // Load attribute.
        if (attr == MP_QSTR_builtin_driver) {
            dest[0] = self->builtin_driver;
        } else {
            // Continue lookup in locals_dict.
            dest[1] = MP_OBJ_SENTINEL;
        }
    } else if (dest[1] != MP_OBJ_NULL) {
        // Store attribute.
        if (attr == MP_QSTR_builtin_driver) {
            if (self->active) {
                mp_raise_OSError(MP_EINVAL); // Need to deactivate first
            }
            // Note: this value should be one of the BUILTIN_nnn constants,
            // but not checked here to save code size in a low level API
            self->builtin_driver = dest[1];
            dest[0] = MP_OBJ_NULL;
        }
    }
}

MP_DEFINE_CONST_OBJ_TYPE(
    machine_usb_device_type,
    MP_QSTR_USBDevice,
    MP_TYPE_FLAG_NONE,
    make_new, usb_device_make_new,
    locals_dict, &usb_device_locals_dict,
    attr, &usb_device_attr
    );

MP_REGISTER_ROOT_POINTER(mp_obj_t usbd);

#endif
