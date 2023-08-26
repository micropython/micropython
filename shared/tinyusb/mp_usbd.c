/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Blake W. Felt & Angus Gratton
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

#include <stdlib.h>

#include "py/mpconfig.h"

#if MICROPY_HW_ENABLE_USBDEV

#ifndef NO_QSTR
#include "tusb.h" // TinyUSB is not available when running the string preprocessor
#include "device/usbd.h"
#include "device/usbd_pvt.h"
#include "mp_usbd_internal.h"
#endif

#include "py/mphal.h"
#include "py/runtime.h"
#include "py/objstr.h"

#if MICROPY_HW_ENABLE_USB_RUNTIME_DEVICE

// Top-level singleton object

typedef struct {
    mp_obj_base_t base;

    // Global callbacks set by USBD.init()
    mp_obj_t descriptor_device_cb;
    mp_obj_t descriptor_config_cb;
    mp_obj_t descriptor_string_cb;
    mp_obj_t open_cb;
    mp_obj_t reset_cb;
    mp_obj_t control_xfer_cb;
    mp_obj_t xfer_cb;

    mp_obj_t reenumerate_cb;

    // Temporary pointers for xfer data in progress.
    // Ensuring they aren't garbage collected until the xfer completes
    mp_obj_t ctrl_xfer_data;
    mp_obj_t xfer_data;
} mp_obj_usbd_t;

const mp_obj_type_t mp_type_usbd;
const mp_obj_type_t mp_type_usbd_static;

STATIC mp_obj_t usbd_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    (void)type;
    (void)n_args;
    (void)n_kw;
    (void)args;

    if (MP_STATE_VM(usbd) == MP_OBJ_NULL) {
        mp_obj_usbd_t *o = m_new0(mp_obj_usbd_t, 1);
        o->base.type = &mp_type_usbd;
        o->descriptor_device_cb = mp_const_none;
        o->descriptor_config_cb = mp_const_none;
        o->descriptor_string_cb = mp_const_none;
        o->open_cb = mp_const_none;
        o->reset_cb = mp_const_none;
        o->control_xfer_cb = mp_const_none;
        o->xfer_cb = mp_const_none;
        o->ctrl_xfer_data = mp_const_none;
        o->xfer_data = mp_const_none;
        o->reenumerate_cb = mp_const_none;

        MP_STATE_VM(usbd) = MP_OBJ_FROM_PTR(o);
    }

    return MP_STATE_VM(usbd);
}

STATIC mp_obj_t usbd_submit_xfer(mp_obj_t self, mp_obj_t ep, mp_obj_t buffer) {
    int epnum = mp_obj_get_int(ep);
    mp_obj_usbd_t *usbd = (mp_obj_usbd_t *)MP_OBJ_TO_PTR(self);
    mp_buffer_info_t buf_info = { 0 };
    bool result;

    if (!usbd_edpt_claim(USBD_RHPORT, epnum)) {
        assert(0); // TODO: raise a runtime error here or something
    }

    // TODO: arg checking

    mp_get_buffer_raise(buffer, &buf_info, epnum & TUSB_DIR_IN_MASK ? MP_BUFFER_READ : MP_BUFFER_RW);

    result = usbd_edpt_xfer(USBD_RHPORT, epnum, buf_info.buf, buf_info.len);

    if (result) {
        // Store the buffer object until the transfer completes
        if (usbd->xfer_data == mp_const_none) {
            usbd->xfer_data = mp_obj_new_dict(1);
        }
        mp_obj_dict_store(usbd->xfer_data, ep, buffer);
    }

    return mp_obj_new_bool(result);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(usbd_submit_xfer_obj, usbd_submit_xfer);

STATIC mp_obj_t usbd_control_xfer(mp_obj_t self, mp_obj_t request_obj, mp_obj_t buffer) {
    mp_obj_usbd_t *usbd = (mp_obj_usbd_t *)MP_OBJ_TO_PTR(self);
    mp_buffer_info_t buf_info = { 0 };
    tusb_control_request_t request = { 0 };
    size_t request_len;
    mp_obj_t *request_items;
    bool result;

    // TODO: arg checking

    mp_obj_tuple_get(request_obj, &request_len, &request_items);
    assert(request_len == 5); // TODO: raise here, once we know that's OK to do

    // TODO: marshalling the request struct to and from a tuple seems like one of the most
    // painful bits of this. Passing raw bytes to Python and letting Python handle it might be smarter.
    request.bmRequestType = mp_obj_get_int(request_items[0]);
    request.bRequest = mp_obj_get_int(request_items[1]);
    request.wValue = mp_obj_get_int(request_items[2]);
    request.wIndex = mp_obj_get_int(request_items[3]);
    request.wLength = mp_obj_get_int(request_items[4]);

    // TODO: What happens if we raise exceptions in TinyUSB callbacks?
    if (mp_obj_is_true(buffer)) {
        mp_get_buffer_raise(buffer, &buf_info,
            request.bmRequestType_bit.direction == TUSB_DIR_IN ? MP_BUFFER_READ : MP_BUFFER_RW);
    } else {
        buf_info.buf = NULL;
        buf_info.len = 0;
    }

    result = tud_control_xfer(USBD_RHPORT,
        &request,
        buf_info.buf,
        buf_info.len);

    if (result) {
        // Keep alive until the transfer can complete
        usbd->ctrl_xfer_data = buffer;
    }

    return mp_obj_new_bool(result);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(usbd_control_xfer_obj, usbd_control_xfer);

STATIC mp_obj_t usbd_reenumerate(size_t n_args, const mp_obj_t *args) {
    mp_obj_usbd_t *usbd = (mp_obj_usbd_t *)MP_OBJ_TO_PTR(args[0]);
    mp_obj_t done_cb = n_args > 1 ? args[1] : mp_const_true;

    /* We may be in a USB-CDC REPL (i.e. inside tud_task()), so it's not safe to
       disconnect here.

       Need to wait until tud_task() exits and do it then. See usbd_task() for
       implementation.
    */

    usbd->reenumerate_cb = mp_obj_is_true(done_cb) ? done_cb : mp_const_true;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(usbd_reenumerate_obj, 1, usbd_reenumerate);

STATIC mp_obj_t usbd_set_ep_stall(mp_obj_t self, mp_obj_t ep, mp_obj_t stall) {
    int epnum = mp_obj_get_int(ep);

    if (mp_obj_is_true(stall)) {
        usbd_edpt_stall(USBD_RHPORT, epnum);
    } else {
        usbd_edpt_clear_stall(USBD_RHPORT, epnum);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(usbd_set_ep_stall_obj, usbd_set_ep_stall);

STATIC mp_obj_t usbd_get_ep_stall(mp_obj_t self, mp_obj_t ep) {
    int epnum = mp_obj_get_int(ep);
    return mp_obj_new_bool(usbd_edpt_stalled(USBD_RHPORT, epnum));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(usbd_get_ep_stall_obj, usbd_get_ep_stall);

/* Initialize the singleton USB device with all of the relevant transfer and descriptor
   callbacks.
*/
STATIC mp_obj_t usbd_init(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    mp_obj_usbd_t *self = (mp_obj_usbd_t *)MP_OBJ_TO_PTR(pos_args[0]);

    enum { ARG_descriptor_device_cb, ARG_descriptor_config_cb, ARG_descriptor_string_cb, ARG_open_cb,
           ARG_reset_cb, ARG_control_xfer_cb, ARG_xfer_cb };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_descriptor_device_cb, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_descriptor_config_cb, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_descriptor_string_cb, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_open_cb, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_reset_cb, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_control_xfer_cb, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_xfer_cb, MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    self->descriptor_device_cb = args[ARG_descriptor_device_cb].u_obj;
    self->descriptor_config_cb = args[ARG_descriptor_config_cb].u_obj;
    self->descriptor_string_cb = args[ARG_descriptor_string_cb].u_obj;
    self->open_cb = args[ARG_open_cb].u_obj;
    self->reset_cb = args[ARG_reset_cb].u_obj;
    self->control_xfer_cb = args[ARG_control_xfer_cb].u_obj;
    self->xfer_cb = args[ARG_xfer_cb].u_obj;

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(usbd_init_obj, 1, usbd_init);

/* usbd_static Python object is a wrapper for the static properties of the USB device */
STATIC const MP_DEFINE_BYTES_OBJ(desc_device_obj,
    &mp_usbd_desc_device_static, sizeof(tusb_desc_device_t));
STATIC const MP_DEFINE_BYTES_OBJ(desc_cfg_obj,
    mp_usbd_desc_cfg_static, USBD_STATIC_DESC_LEN);

STATIC const mp_rom_map_elem_t usbd_static_properties_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_itf_max), MP_OBJ_NEW_SMALL_INT(USBD_ITF_STATIC_MAX) },
    { MP_ROM_QSTR(MP_QSTR_ep_max), MP_OBJ_NEW_SMALL_INT(USBD_EP_STATIC_MAX) },
    { MP_ROM_QSTR(MP_QSTR_str_max), MP_OBJ_NEW_SMALL_INT(USBD_STR_STATIC_MAX) },
    { MP_ROM_QSTR(MP_QSTR_desc_device), MP_ROM_PTR(&desc_device_obj)  },
    { MP_ROM_QSTR(MP_QSTR_desc_cfg), MP_ROM_PTR(&desc_cfg_obj) },
};
STATIC MP_DEFINE_CONST_DICT(usbd_static_properties_dict, usbd_static_properties_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_usbd_static,
    MP_QSTR_usbd_static,
    MP_TYPE_FLAG_NONE,
    locals_dict, &usbd_static_properties_dict
    );


STATIC const mp_rom_map_elem_t usbd_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_static), MP_ROM_PTR(&mp_type_usbd_static) },
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&usbd_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_submit_xfer), MP_ROM_PTR(&usbd_submit_xfer_obj) },
    { MP_ROM_QSTR(MP_QSTR_control_xfer), MP_ROM_PTR(&usbd_control_xfer_obj) },
    { MP_ROM_QSTR(MP_QSTR_reenumerate), MP_ROM_PTR(&usbd_reenumerate_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_ep_stall), MP_ROM_PTR(&usbd_set_ep_stall_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_ep_stall), MP_ROM_PTR(&usbd_get_ep_stall_obj) },
};
STATIC MP_DEFINE_CONST_DICT(usbd_locals_dict, usbd_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_usbd,
    MP_QSTR_USBD,
    MP_TYPE_FLAG_NONE,
    make_new, usbd_make_new,
    locals_dict, &usbd_locals_dict
    );

MP_REGISTER_ROOT_POINTER(mp_obj_t usbd);

/* Common code path for descriptor callback functions that read descriptor
   values back from the relevant Python callback, or fall back to the static
   result if no callback is set.
*/
static const uint8_t *_usbd_handle_descriptor_cb(mp_obj_t callback, const void *static_result) {
    mp_obj_usbd_t *usbd = MP_OBJ_TO_PTR(MP_STATE_VM(usbd));
    mp_buffer_info_t buf_info;

    if (usbd == NULL || callback == mp_const_none) {
        // The USBD object isn't initialized, or this callback is unset,
        // so return the static descriptor
        return static_result;
    }

    usbd->ctrl_xfer_data = mp_call_function_0(callback);
    mp_get_buffer_raise(usbd->ctrl_xfer_data, &buf_info, MP_BUFFER_READ);
    return buf_info.buf;
}

const uint8_t *tud_descriptor_device_cb(void) {
    mp_obj_usbd_t *usbd = MP_OBJ_TO_PTR(MP_STATE_VM(usbd));
    return _usbd_handle_descriptor_cb(usbd ? usbd->descriptor_device_cb : mp_const_none,
        &mp_usbd_desc_device_static);
}

const uint8_t *tud_descriptor_configuration_cb(uint8_t index) {
    (void)index;
    mp_obj_usbd_t *usbd = MP_OBJ_TO_PTR(MP_STATE_VM(usbd));
    return _usbd_handle_descriptor_cb(usbd ? usbd->descriptor_config_cb : mp_const_none,
        mp_usbd_desc_cfg_static);
}

const char *mp_usbd_internal_dynamic_descriptor_string_cb(uint8_t index) {
    mp_obj_usbd_t *usbd = MP_OBJ_TO_PTR(MP_STATE_VM(usbd));
    mp_buffer_info_t buf_info;

    if (usbd == NULL || usbd->descriptor_string_cb == mp_const_none) {
        return NULL;
    }

    mp_obj_t callback_res = mp_call_function_1(usbd->descriptor_string_cb, mp_obj_new_int(index));
    if (callback_res == mp_const_none) {
        return NULL;
    }
    mp_get_buffer_raise(callback_res, &buf_info, MP_BUFFER_READ);
    return buf_info.buf;
}

bool tud_vendor_control_xfer_cb(uint8_t rhport, uint8_t stage, tusb_control_request_t const *request) {
    // TODO: Implement Vendor control xfers on the Python side
    return false;
}

void runtime_dev_init(void) {
}

void runtime_dev_reset(uint8_t rhport) {
    mp_obj_usbd_t *usbd = MP_OBJ_TO_PTR(MP_STATE_VM(usbd));
    if (!usbd) {
        return; // Unsure if this will ever happen?
    }
    usbd->xfer_data = mp_const_none;
    usbd->ctrl_xfer_data = mp_const_none;

    if (mp_obj_is_callable(usbd->reset_cb)) {
        mp_call_function_0(usbd->reset_cb);
    }
}

uint16_t runtime_dev_open(uint8_t rhport, tusb_desc_interface_t const *itf_desc, uint16_t max_len) {
    mp_obj_usbd_t *usbd = MP_OBJ_TO_PTR(MP_STATE_VM(usbd));
    const uint8_t *p_desc = (const void *)itf_desc;
    uint16_t claim_len = 0;

    if (!usbd) {
        return 0;
    }

    // Claim any interfaces (and associated descriptor data) that aren't in the interface number range reserved for
    // static drivers.
    while (claim_len < max_len && (tu_desc_type(p_desc) != TUSB_DESC_INTERFACE ||
                                   ((tusb_desc_interface_t *)p_desc)->bInterfaceNumber >= USBD_ITF_STATIC_MAX)) {
        if (tu_desc_type(p_desc) == TUSB_DESC_ENDPOINT) {
            // Open all the relevant endpoints

            // TODO: it might make more sense to move this to the Python side, so usbd.open_cb handler has to explicitly open
            // the endpoints it intends to use? Will wait and see what the usage patterns are like.
            bool r __attribute__((unused)) = usbd_edpt_open(USBD_RHPORT, (const void *)p_desc);
            assert(r); // TODO: work out what to do if this fails
        }

        claim_len += tu_desc_len(p_desc);
        p_desc += tu_desc_len(p_desc);
    }

    if (claim_len && mp_obj_is_callable(usbd->open_cb)) {
        // Make a read-only memoryview into the itf_desc_view.
        // Important the callback doesn't stash this data anywhere.
        mp_obj_t itf_desc_view = mp_obj_new_memoryview('B', claim_len, (void *)itf_desc);
        mp_call_function_1(usbd->open_cb, itf_desc_view);
    }

    return claim_len;
}

bool runtime_dev_control_xfer_cb(uint8_t rhport, uint8_t stage, tusb_control_request_t const *request) {
    mp_obj_t cb_res = mp_const_false;
    mp_obj_usbd_t *usbd = MP_OBJ_TO_PTR(MP_STATE_VM(usbd));
    assert(usbd);

    if (mp_obj_is_callable(usbd->control_xfer_cb)) {
        // TODO: it might more efficient to pass request as a memoryview, and make the caller extract fields of interest?
        mp_obj_t request_tuple[] = {
            mp_obj_new_int(request->bmRequestType),
            mp_obj_new_int(request->bRequest),
            mp_obj_new_int(request->wValue),
            mp_obj_new_int(request->wIndex),
            mp_obj_new_int(request->wLength),
        };
        cb_res = mp_call_function_2(usbd->control_xfer_cb,
            mp_obj_new_int(stage),
            mp_obj_new_tuple(MP_ARRAY_SIZE(request_tuple), request_tuple));
    }

    if (stage == CONTROL_STAGE_ACK) {
        // Allow data to be GCed once it's no longer in use
        usbd->ctrl_xfer_data = mp_const_none;
    }

    return mp_obj_is_true(cb_res);
}

bool runtime_dev_xfer_cb(uint8_t rhport, uint8_t ep_addr, xfer_result_t result, uint32_t xferred_bytes) {
    mp_obj_t ep = mp_obj_new_int(ep_addr);
    mp_obj_t cb_res = mp_const_false;
    mp_obj_usbd_t *usbd = MP_OBJ_TO_PTR(MP_STATE_VM(usbd));
    assert(usbd);

    if (mp_obj_is_callable(usbd->xfer_cb)) {
        mp_obj_t args[] = {
            ep,
            mp_obj_new_int(result),
            mp_obj_new_int(xferred_bytes)
        };
        cb_res = mp_call_function_n_kw(usbd->xfer_cb, MP_ARRAY_SIZE(args), 0, args);
    }

    if (usbd->xfer_data != mp_const_none) {
        // Storing None instead of deleting here means we don't need to handle KeyError,
        // and most likely this device EP number will be used again
        mp_obj_dict_store(usbd->xfer_data, ep, mp_const_none);
    }

    return mp_obj_is_true(cb_res);
}

static usbd_class_driver_t const _runtime_dev_driver =
{
    #if CFG_TUSB_DEBUG >= 2
    .name = "runtime_dev",
    #endif
    .init = runtime_dev_init,
    .reset = runtime_dev_reset,
    .open = runtime_dev_open,
    .control_xfer_cb = runtime_dev_control_xfer_cb,
    .xfer_cb = runtime_dev_xfer_cb,
    .sof = NULL
};

usbd_class_driver_t const *usbd_app_driver_get_cb(uint8_t *driver_count) {
    *driver_count = 1;
    return &_runtime_dev_driver;
}

#endif // MICROPY_HW_ENABLE_USB_RUNTIME_DEVICE

void usbd_task(void) {
    #ifdef MICROPY_HW_USBDEV_TASK_HOOK
    // If we call here from a "polling" task hook (rather than an ISR as on some
    // ports), then need to prevent recursive calls to tud_task_ext() if/when
    // the task hook runs Python code in a callback
    static bool in_task;
    if (in_task) {
        return;
    }
    in_task = true;
    #endif

    tud_task_ext(0, false);

    #if MICROPY_HW_ENABLE_USB_RUNTIME_DEVICE
    mp_obj_usbd_t *usbd = MP_OBJ_TO_PTR(MP_STATE_VM(usbd));
    if (usbd && mp_obj_is_true(usbd->reenumerate_cb)) {
        tud_disconnect();

        /* Wait for the TinyUSB state machine to know it's been disconnected */
        while (tud_connected()) {
            tud_task_ext(0, false);
        }
        tud_task_ext(0, false);

        tud_connect();

        /* Note: nothing is actually connected at this point, what happens next
           is the host will reenumerate the device. Arguably, the descriptor
           callbacks are the real signal we've reenumerated. */
        if (mp_obj_is_callable(usbd->reenumerate_cb)) {
            mp_call_function_0(usbd->reenumerate_cb);
        }
        usbd->reenumerate_cb = mp_const_none;
    }
    #endif // MICROPY_HW_ENABLE_USB_RUNTIME_DEVICE

    #ifdef MICROPY_HW_USBDEV_TASK_HOOK
    in_task = false;
    #endif
}

#endif
