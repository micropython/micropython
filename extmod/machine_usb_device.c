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

#if MICROPY_HW_ENABLE_USBDEV

#include "mp_usbd.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/objstr.h"

// Implements the singleton runtime USB object
// Full runtime functionality when MICROPY_HW_ENABLE_USB_RUNTIME_DEVICE=1
// Minimal functionality (builtin_driver control only) when =0
//
// Currently this implementation references TinyUSB directly.

#ifndef NO_QSTR
#include "device/usbd_pvt.h"
#endif

#define HAS_BUILTIN_DRIVERS (MICROPY_HW_USB_CDC || MICROPY_HW_USB_MSC || MICROPY_HW_USB_NCM)

// USB class flags are defined in mp_usbd.h

// Structure for combinable built-in USB driver configurations
typedef struct {
    mp_obj_base_t base;
    uint8_t flags;  // Combination of USB_BUILTIN_FLAG_* values
} mp_obj_usb_builtin_t;

const mp_obj_type_t machine_usb_device_type;
const mp_obj_type_t mp_type_usb_builtin;

// Forward declarations for builtin objects
static const mp_obj_usb_builtin_t builtin_none_obj;

// Forward declaration for builtin config creation
static mp_obj_t mp_usbd_create_builtin_config(uint8_t flags);

static mp_obj_t usb_device_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    (void)type;
    (void)n_args;
    (void)n_kw;
    (void)args;

    if (MP_STATE_VM(usbd) == MP_OBJ_NULL) {
        mp_obj_usb_device_t *o = m_new0(mp_obj_usb_device_t, 1);
        o->base.type = &machine_usb_device_type;

        // Initialize fields common to both minimal and full modes
        #if MICROPY_HW_ENABLE_USB_RUNTIME_DEVICE
        o->builtin_driver = MP_OBJ_FROM_PTR(&builtin_none_obj);
        o->active = false; // Runtime mode starts inactive
        #else
        // In static mode, detect if USB is already initialized at boot
        // Check if TinyUSB is running by testing if the stack is configured
        o->active = tud_inited();

        // Set builtin_driver based on current class state if active
        if (o->active) {
            // Convert current class state to appropriate builtin driver
            extern mp_usbd_class_state_t mp_usbd_class_state;
            uint8_t flags = 0;
            if (mp_usbd_class_state.cdc_enabled) {
                flags |= USB_BUILTIN_FLAG_CDC;
            }
            if (mp_usbd_class_state.msc_enabled) {
                flags |= USB_BUILTIN_FLAG_MSC;
            }
            #if MICROPY_HW_NETWORK_USBNET
            if (mp_usbd_class_state.ncm_enabled) {
                flags |= USB_BUILTIN_FLAG_NCM;
            }
            #endif

            // Create appropriate builtin object based on current state
            if (flags == USB_BUILTIN_FLAG_NONE) {
                o->builtin_driver = MP_OBJ_FROM_PTR(&builtin_none_obj);
            } else {
                // Use the dynamic builtin system
                o->builtin_driver = mp_usbd_create_builtin_config(flags);
            }
        } else {
            o->builtin_driver = MP_OBJ_FROM_PTR(&builtin_none_obj);
        }
        #endif
        o->custom_vid = 0; // 0 = use builtin default
        o->custom_pid = 0; // 0 = use builtin default

        #if MICROPY_HW_ENABLE_USB_RUNTIME_DEVICE
        // Initialize runtime-only fields
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
        o->trigger = false;
        o->control_data = MP_OBJ_TO_PTR(mp_obj_new_memoryview('B', 0, NULL));
        o->num_pend_excs = 0;
        for (int i = 0; i < MP_USBD_MAX_PEND_EXCS; i++) {
            o->pend_excs[i] = mp_const_none;
        }
        #endif

        MP_STATE_VM(usbd) = MP_OBJ_FROM_PTR(o);
    }

    return MP_STATE_VM(usbd);
}
#if MICROPY_HW_ENABLE_USB_RUNTIME_DEVICE
// Utility helper to raise an error if USB device is not active
// (or if a change of active state is triggered but not processed.)
static void usb_device_check_active(mp_obj_usb_device_t *usbd) {
    if (!usbd->active || usbd->trigger) {
        mp_raise_OSError(MP_EINVAL);
    }
}
#endif

#if MICROPY_HW_ENABLE_USB_RUNTIME_DEVICE
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
#endif // MICROPY_HW_ENABLE_USB_RUNTIME_DEVICE

static mp_obj_t usb_device_active(size_t n_args, const mp_obj_t *args) {
    mp_obj_usb_device_t *usbd = (mp_obj_usb_device_t *)MP_OBJ_TO_PTR(args[0]);

    bool result = usbd->active;
    if (n_args == 2) {
        bool value = mp_obj_is_true(args[1]);

        if (value != result) {
            #if MICROPY_HW_ENABLE_USB_RUNTIME_DEVICE
            // Runtime mode: check descriptors and handle trigger mechanism
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
            #else
            // Static mode: directly control USB classes based on builtin_driver
            if (value && !mp_usb_device_builtin_enabled(usbd)) {
                // Only allow activating if a built-in driver is enabled
                mp_raise_OSError(MP_EINVAL);
            }

            usbd->active = value;
            if (value) {
                mp_usbd_init(); // Ensure TinyUSB has initialised by this point

                // Update class state based on current builtin_driver
                if (mp_obj_is_type(usbd->builtin_driver, &mp_type_usb_builtin)) {
                    mp_obj_usb_builtin_t *builtin = MP_OBJ_TO_PTR(usbd->builtin_driver);
                    mp_usbd_update_class_state(builtin->flags);
                }

                // Connect to USB host
                tud_connect();
            } else {
                // Disconnect from USB host first
                tud_disconnect();

                // Disable all classes when deactivating
                mp_usbd_update_class_state(USB_BUILTIN_FLAG_NONE);
            }
            #endif
        }
    }

    return mp_obj_new_bool(result);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(usb_device_active_obj, 1, 2, usb_device_active);

#if MICROPY_HW_ENABLE_USB_RUNTIME_DEVICE
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
#endif // MICROPY_HW_ENABLE_USB_RUNTIME_DEVICE

#if MICROPY_HW_ENABLE_USB_RUNTIME_DEVICE
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
#endif // MICROPY_HW_ENABLE_USB_RUNTIME_DEVICE

// Device descriptor object (needed by bitfield builtin functions)
static const MP_DEFINE_BYTES_OBJ(builtin_default_desc_dev_obj,
    &mp_usbd_builtin_desc_dev, sizeof(tusb_desc_device_t));

// Forward declarations for bitfield builtin functions
static uint8_t mp_usbd_get_itf_max(uint8_t flags);
static uint8_t mp_usbd_get_ep_max(uint8_t flags);
static uint8_t mp_usbd_get_str_max(uint8_t flags);
static const uint8_t *mp_usbd_get_builtin_desc_cfg(uint8_t flags);
static size_t mp_usbd_get_desc_cfg_len(uint8_t flags);

// Function from mp_usbd_descriptor.c to update class state based on flags
extern void mp_usbd_update_class_state(uint8_t flags);

// Create a new builtin configuration object with specified flags
static mp_obj_t mp_usbd_create_builtin_config(uint8_t flags) {
    mp_obj_usb_builtin_t *builtin = mp_obj_malloc(mp_obj_usb_builtin_t, &mp_type_usb_builtin);
    builtin->flags = flags;
    return MP_OBJ_FROM_PTR(builtin);
}

// Binary operator for combining builtin configs (implements __or__)
static mp_obj_t builtin_binary_op(mp_binary_op_t op, mp_obj_t lhs_in, mp_obj_t rhs_in) {
    if (op == MP_BINARY_OP_OR) {
        mp_obj_usb_builtin_t *lhs = MP_OBJ_TO_PTR(lhs_in);
        mp_obj_usb_builtin_t *rhs = MP_OBJ_TO_PTR(rhs_in);
        return mp_usbd_create_builtin_config(lhs->flags | rhs->flags);
    }
    return MP_OBJ_NULL; // Operation not supported
}

// Dynamic attribute access for builtin config objects
static void builtin_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    mp_obj_usb_builtin_t *self = MP_OBJ_TO_PTR(self_in);

    if (dest[0] == MP_OBJ_NULL) {
        // Load attribute
        if (attr == MP_QSTR_desc_cfg) {
            size_t len = mp_usbd_get_desc_cfg_len(self->flags);
            dest[0] = mp_obj_new_bytes(mp_usbd_get_builtin_desc_cfg(self->flags), len);
        } else if (attr == MP_QSTR_itf_max) {
            dest[0] = MP_OBJ_NEW_SMALL_INT(mp_usbd_get_itf_max(self->flags));
        } else if (attr == MP_QSTR_ep_max) {
            dest[0] = MP_OBJ_NEW_SMALL_INT(mp_usbd_get_ep_max(self->flags));
        } else if (attr == MP_QSTR_str_max) {
            dest[0] = MP_OBJ_NEW_SMALL_INT(mp_usbd_get_str_max(self->flags));
        } else if (attr == MP_QSTR_desc_dev) {
            dest[0] = MP_OBJ_FROM_PTR(&builtin_default_desc_dev_obj);
        }
    }
}

// Helper functions for dynamic property calculation
static uint8_t mp_usbd_get_itf_max(uint8_t flags) {
    uint8_t count = 0;
    if ((flags & USB_BUILTIN_FLAG_CDC) && MICROPY_HW_USB_CDC) {
        count += 2;  // CDC uses 2 interfaces
    }
    if ((flags & USB_BUILTIN_FLAG_MSC) && MICROPY_HW_USB_MSC) {
        count += 1;
    }
    #if MICROPY_HW_NETWORK_USBNET
    if ((flags & USB_BUILTIN_FLAG_NCM) && MICROPY_HW_NETWORK_USBNET) {
        count += 2;  // NCM uses 2 interfaces (control + data)
    }
    #endif
    return count;
}

static uint8_t mp_usbd_get_ep_max(uint8_t flags) {
    uint8_t ep_max = 1;  // Endpoint 0 is always used
    if ((flags & USB_BUILTIN_FLAG_CDC) && MICROPY_HW_USB_CDC) {
        ep_max = 3;  // CDC uses endpoints 1, 2, 3
    }
    if ((flags & USB_BUILTIN_FLAG_MSC) && MICROPY_HW_USB_MSC) {
        ep_max = (ep_max > 2) ? ep_max : 2;  // MSC uses endpoints 1, 2
    }
    #if MICROPY_HW_NETWORK_USBNET
    if ((flags & USB_BUILTIN_FLAG_NCM) && MICROPY_HW_NETWORK_USBNET) {
        ep_max = (ep_max > 3) ? ep_max : 3;  // NCM uses endpoints 1, 2, 3
    }
    #endif
    return ep_max;
}

static uint8_t mp_usbd_get_str_max(uint8_t flags) {
    uint8_t str_max = 1;  // String 0 is always used (language descriptor)
    if ((flags & USB_BUILTIN_FLAG_CDC) && MICROPY_HW_USB_CDC) {
        str_max = 4;  // CDC uses strings 1, 2, 3, 4
    }
    if ((flags & USB_BUILTIN_FLAG_MSC) && MICROPY_HW_USB_MSC) {
        str_max = (str_max > 2) ? str_max : 2;  // MSC uses strings 1, 2
    }
    #if MICROPY_HW_NETWORK_USBNET
    if ((flags & USB_BUILTIN_FLAG_NCM) && MICROPY_HW_NETWORK_USBNET) {
        str_max = (str_max > 3) ? str_max : 3;  // NCM uses strings 1, 2, 3
    }
    #endif
    return str_max;
}

static const uint8_t *mp_usbd_get_builtin_desc_cfg(uint8_t flags) {
    // For now, return the generated descriptor from mp_usbd_descriptor.c
    // This will be implemented when we modify that file
    extern const uint8_t *mp_usbd_generate_desc_cfg_from_flags(uint8_t flags);
    return mp_usbd_generate_desc_cfg_from_flags(flags);
}

static size_t mp_usbd_get_desc_cfg_len(uint8_t flags) {
    // Calculate descriptor length based on enabled classes
    extern size_t mp_usbd_get_descriptor_cfg_len_from_flags(uint8_t flags);
    return mp_usbd_get_descriptor_cfg_len_from_flags(flags);
}

// Type definition for combinable builtin config objects
MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_usb_builtin,
    MP_QSTR_USBBuiltin,
    MP_TYPE_FLAG_NONE,
    binary_op, builtin_binary_op,
    attr, builtin_attr
    );

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

// Create the new bitfield-based constant objects
static const mp_obj_usb_builtin_t builtin_none_obj = {
    {&mp_type_usb_builtin}, USB_BUILTIN_FLAG_NONE
};

static const mp_obj_usb_builtin_t builtin_cdc_obj = {
    {&mp_type_usb_builtin}, USB_BUILTIN_FLAG_CDC
};

static const mp_obj_usb_builtin_t builtin_msc_obj = {
    {&mp_type_usb_builtin}, USB_BUILTIN_FLAG_MSC
};

#if MICROPY_HW_NETWORK_USBNET
static const mp_obj_usb_builtin_t builtin_ncm_obj = {
    {&mp_type_usb_builtin}, USB_BUILTIN_FLAG_NCM
};
#endif

// Create default combination based on compile-time configuration
#if MICROPY_HW_USB_CDC && MICROPY_HW_USB_MSC
static const mp_obj_usb_builtin_t builtin_default_obj = {
    {&mp_type_usb_builtin}, USB_BUILTIN_FLAG_CDC | USB_BUILTIN_FLAG_MSC
};
#elif MICROPY_HW_USB_CDC
static const mp_obj_usb_builtin_t builtin_default_obj = {
    {&mp_type_usb_builtin}, USB_BUILTIN_FLAG_CDC
};
#elif MICROPY_HW_USB_MSC
static const mp_obj_usb_builtin_t builtin_default_obj = {
    {&mp_type_usb_builtin}, USB_BUILTIN_FLAG_MSC
};
#else
static const mp_obj_usb_builtin_t builtin_default_obj = {
    {&mp_type_usb_builtin}, USB_BUILTIN_FLAG_NONE
};
#endif

// Legacy combination for backward compatibility
#if MICROPY_HW_USB_CDC && MICROPY_HW_USB_MSC
static const mp_obj_usb_builtin_t builtin_cdc_msc_obj = {
    {&mp_type_usb_builtin}, USB_BUILTIN_FLAG_CDC | USB_BUILTIN_FLAG_MSC
};
#endif

#if MICROPY_HW_ENABLE_USB_RUNTIME_DEVICE
// Full runtime USB device methods and constants
static const mp_rom_map_elem_t usb_device_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_config), MP_ROM_PTR(&usb_device_config_obj) },
    { MP_ROM_QSTR(MP_QSTR_submit_xfer), MP_ROM_PTR(&usb_device_submit_xfer_obj) },
    { MP_ROM_QSTR(MP_QSTR_active), MP_ROM_PTR(&usb_device_active_obj) },
    { MP_ROM_QSTR(MP_QSTR_stall), MP_ROM_PTR(&usb_device_stall_obj) },
    { MP_ROM_QSTR(MP_QSTR_remote_wakeup), MP_ROM_PTR(&usb_remote_wakeup_obj) },

    // Built-in driver constants (new bitfield-based system)
    { MP_ROM_QSTR(MP_QSTR_BUILTIN_NONE), MP_ROM_PTR(&builtin_none_obj) },
    { MP_ROM_QSTR(MP_QSTR_BUILTIN_DEFAULT), MP_ROM_PTR(&builtin_default_obj) },

    // Individual class constants
    { MP_ROM_QSTR(MP_QSTR_BUILTIN_CDC), MP_ROM_PTR(&builtin_cdc_obj) },
    { MP_ROM_QSTR(MP_QSTR_BUILTIN_MSC), MP_ROM_PTR(&builtin_msc_obj) },

    #if MICROPY_HW_NETWORK_USBNET
    { MP_ROM_QSTR(MP_QSTR_BUILTIN_NCM), MP_ROM_PTR(&builtin_ncm_obj) },
    #endif

    // Legacy combination constant for backward compatibility
    #if MICROPY_HW_USB_CDC && MICROPY_HW_USB_MSC
    { MP_ROM_QSTR(MP_QSTR_BUILTIN_CDC_MSC), MP_ROM_PTR(&builtin_cdc_msc_obj) },
    #endif
};
#else
// Minimal static USB device methods and constants
static const mp_rom_map_elem_t usb_device_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_active), MP_ROM_PTR(&usb_device_active_obj) },

    // Built-in driver constants (new bitfield-based system)
    { MP_ROM_QSTR(MP_QSTR_BUILTIN_NONE), MP_ROM_PTR(&builtin_none_obj) },
    { MP_ROM_QSTR(MP_QSTR_BUILTIN_DEFAULT), MP_ROM_PTR(&builtin_default_obj) },

    // Individual class constants
    { MP_ROM_QSTR(MP_QSTR_BUILTIN_CDC), MP_ROM_PTR(&builtin_cdc_obj) },
    { MP_ROM_QSTR(MP_QSTR_BUILTIN_MSC), MP_ROM_PTR(&builtin_msc_obj) },

    #if MICROPY_HW_NETWORK_USBNET
    { MP_ROM_QSTR(MP_QSTR_BUILTIN_NCM), MP_ROM_PTR(&builtin_ncm_obj) },
    #endif

    // Legacy combination constant for backward compatibility
    #if MICROPY_HW_USB_CDC && MICROPY_HW_USB_MSC
    { MP_ROM_QSTR(MP_QSTR_BUILTIN_CDC_MSC), MP_ROM_PTR(&builtin_cdc_msc_obj) },
    #endif
};
#endif
static MP_DEFINE_CONST_DICT(usb_device_locals_dict, usb_device_locals_dict_table);

static void usb_device_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    mp_obj_usb_device_t *self = MP_OBJ_TO_PTR(self_in);
    if (dest[0] == MP_OBJ_NULL) {
        // Load attribute.
        if (attr == MP_QSTR_builtin_driver) {
            dest[0] = self->builtin_driver;
        } else if (attr == MP_QSTR_vid) {
            uint16_t vid = self->custom_vid;
            if (vid == 0) {
                vid = MICROPY_HW_USB_RUNTIME_VID;
            }
            dest[0] = MP_OBJ_NEW_SMALL_INT(vid);
        } else if (attr == MP_QSTR_pid) {
            uint16_t pid = self->custom_pid;
            if (pid == 0) {
                pid = MICROPY_HW_USB_RUNTIME_PID;
            }
            dest[0] = MP_OBJ_NEW_SMALL_INT(pid);
        } else {
            // Continue lookup in locals_dict.
            dest[1] = MP_OBJ_SENTINEL;
        }
    } else if (dest[1] != MP_OBJ_NULL) {
        // Store attribute.
        if (attr == MP_QSTR_builtin_driver) {
            // Require deactivation before changing builtin_driver
            if (self->active) {
                mp_raise_OSError(MP_EINVAL);
            }

            // Handle new bitfield builtin types and legacy constants
            uint8_t flags = 0;
            if (mp_obj_is_type(dest[1], &mp_type_usb_builtin)) {
                // New bitfield builtin object
                mp_obj_usb_builtin_t *builtin = MP_OBJ_TO_PTR(dest[1]);
                flags = builtin->flags;
            } else if (dest[1] == MP_OBJ_FROM_PTR(&builtin_none_obj)) {
                flags = USB_BUILTIN_FLAG_NONE;
            } else if (dest[1] == MP_OBJ_FROM_PTR(&builtin_default_obj)) {
                flags = builtin_default_obj.flags;
            } else if (dest[1] == MP_OBJ_FROM_PTR(&builtin_cdc_obj)) {
                flags = USB_BUILTIN_FLAG_CDC;
            } else if (dest[1] == MP_OBJ_FROM_PTR(&builtin_msc_obj)) {
                flags = USB_BUILTIN_FLAG_MSC;
            #if MICROPY_HW_NETWORK_USBNET
            } else if (dest[1] == MP_OBJ_FROM_PTR(&builtin_ncm_obj)) {
                flags = USB_BUILTIN_FLAG_NCM;
            #endif
            #if MICROPY_HW_USB_CDC && MICROPY_HW_USB_MSC
            } else if (dest[1] == MP_OBJ_FROM_PTR(&builtin_cdc_msc_obj)) {
                flags = USB_BUILTIN_FLAG_CDC | USB_BUILTIN_FLAG_MSC;
            #endif
            // Also handle legacy constant types for backward compatibility
            } else if (dest[1] == MP_OBJ_FROM_PTR(&mp_type_usb_device_builtin_none)) {
                flags = USB_BUILTIN_FLAG_NONE;
            #if HAS_BUILTIN_DRIVERS
            } else if (dest[1] == MP_OBJ_FROM_PTR(&mp_type_usb_device_builtin_default)) {
                // Map to default combination based on compile configuration
                #if MICROPY_HW_USB_CDC && MICROPY_HW_USB_MSC
                flags = USB_BUILTIN_FLAG_CDC | USB_BUILTIN_FLAG_MSC;
                #elif MICROPY_HW_USB_CDC
                flags = USB_BUILTIN_FLAG_CDC;
                #elif MICROPY_HW_USB_MSC
                flags = USB_BUILTIN_FLAG_MSC;
                #endif
            #endif
            }

            // Update the internal class state based on flags
            mp_usbd_update_class_state(flags);

            // Store the builtin_driver (could be legacy constant or new bitfield object)
            self->builtin_driver = dest[1];
            dest[0] = MP_OBJ_NULL;
        } else if (attr == MP_QSTR_vid) {
            // Require deactivation before changing VID
            if (self->active) {
                mp_raise_OSError(MP_EINVAL);
            }
            mp_int_t vid = mp_obj_get_int(dest[1]);
            if (vid < 0 || vid > 0xFFFF) {
                mp_raise_ValueError(MP_ERROR_TEXT("VID must be 0-65535"));
            }
            self->custom_vid = (uint16_t)vid;
            dest[0] = MP_OBJ_NULL;
        } else if (attr == MP_QSTR_pid) {
            // Require deactivation before changing PID
            if (self->active) {
                mp_raise_OSError(MP_EINVAL);
            }
            mp_int_t pid = mp_obj_get_int(dest[1]);
            if (pid < 0 || pid > 0xFFFF) {
                mp_raise_ValueError(MP_ERROR_TEXT("PID must be 0-65535"));
            }
            self->custom_pid = (uint16_t)pid;
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
