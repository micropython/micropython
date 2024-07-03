/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Blake W. Felt
 * Copyright (c) 2022-2023 Angus Gratton
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

#include "mp_usbd.h"
#include "py/mpconfig.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/obj.h"
#include "py/objarray.h"
#include "py/objstr.h"
#include "py/runtime.h"

#if MICROPY_HW_ENABLE_USB_RUNTIME_DEVICE

#ifndef NO_QSTR
#include "tusb.h" // TinyUSB is not available when running the string preprocessor
#include "device/dcd.h"
#include "device/usbd.h"
#include "device/usbd_pvt.h"
#endif

static bool in_usbd_task; // Flags if mp_usbd_task() is currently running

// Some top-level functions that manage global TinyUSB USBD state, not the
// singleton object visible to Python
static void mp_usbd_disconnect(mp_obj_usb_device_t *usbd);
static void mp_usbd_task_inner(void);

// Pend an exception raise in a USBD callback to print when safe.
//
// We can't raise any exceptions out of the TinyUSB task, as it may still need
// to do some state cleanup.
//
// The requirement for this becomes very similar to
// mp_call_function_x_protected() for interrupts, but it's more restrictive: if
// the C-based USB-CDC serial port is in use, we can't print from inside a
// TinyUSB callback as it might try to recursively call into TinyUSB to flush
// the CDC port and make room. Therefore, we have to store the exception and
// print it as we exit the TinyUSB task.
//
// (Worse, a single TinyUSB task can process multiple callbacks and therefore generate
// multiple exceptions...)
static void usbd_pend_exception(mp_obj_t exception) {
    mp_obj_usb_device_t *usbd = MP_OBJ_TO_PTR(MP_STATE_VM(usbd));
    assert(usbd != NULL);
    if (usbd->num_pend_excs < MP_USBD_MAX_PEND_EXCS) {
        usbd->pend_excs[usbd->num_pend_excs] = exception;
    }
    usbd->num_pend_excs++;
}

// Call a Python function from inside a TinyUSB callback.
//
// Handles any exception using usbd_pend_exception()
static mp_obj_t usbd_callback_function_n(mp_obj_t fun, size_t n_args, const mp_obj_t *args) {
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_obj_t ret = mp_call_function_n_kw(fun, n_args, 0, args);
        nlr_pop();
        return ret;
    } else {
        usbd_pend_exception(MP_OBJ_FROM_PTR(nlr.ret_val));
        return MP_OBJ_NULL;
    }
}

// Return a pointer to the data inside a Python buffer provided in a callback
static void *usbd_get_buffer_in_cb(mp_obj_t obj, mp_uint_t flags) {
    mp_buffer_info_t buf_info;
    if (obj == mp_const_none) {
        // This is only if the user somehow
        return NULL;
    } else if (mp_get_buffer(obj, &buf_info, flags)) {
        return buf_info.buf;
    } else {
        mp_obj_t exc = mp_obj_new_exception_msg(&mp_type_TypeError,
            MP_ERROR_TEXT("object with buffer protocol required"));
        usbd_pend_exception(exc);
        return NULL;
    }
}

const uint8_t *tud_descriptor_device_cb(void) {
    mp_obj_usb_device_t *usbd = MP_OBJ_TO_PTR(MP_STATE_VM(usbd));
    const void *result = NULL;
    if (usbd) {
        result = usbd_get_buffer_in_cb(usbd->desc_dev, MP_BUFFER_READ);
    }
    return result ? result : &mp_usbd_builtin_desc_dev;
}

const uint8_t *tud_descriptor_configuration_cb(uint8_t index) {
    (void)index;
    mp_obj_usb_device_t *usbd = MP_OBJ_TO_PTR(MP_STATE_VM(usbd));
    const void *result = NULL;
    if (usbd) {
        result = usbd_get_buffer_in_cb(usbd->desc_cfg, MP_BUFFER_READ);
    }
    return result ? result : &mp_usbd_builtin_desc_cfg;
}

const char *mp_usbd_runtime_string_cb(uint8_t index) {
    mp_obj_usb_device_t *usbd = MP_OBJ_TO_PTR(MP_STATE_VM(usbd));
    nlr_buf_t nlr;

    if (usbd == NULL || usbd->desc_strs == mp_const_none) {
        return NULL;
    }

    if (nlr_push(&nlr) == 0) {
        mp_obj_t res = mp_obj_subscr(usbd->desc_strs, mp_obj_new_int(index), MP_OBJ_SENTINEL);
        nlr_pop();
        if (res != mp_const_none) {
            return usbd_get_buffer_in_cb(res, MP_BUFFER_READ);
        }
    } else {
        mp_obj_t exception = MP_OBJ_FROM_PTR(nlr.ret_val);
        if (!(mp_obj_is_type(exception, &mp_type_KeyError) || mp_obj_is_type(exception, &mp_type_IndexError))) {
            // Don't print KeyError or IndexError, allowing dicts or lists to have missing entries.
            // but log any more exotic errors that pop up
            usbd_pend_exception(exception);
        }
    }

    return NULL;
}

bool tud_vendor_control_xfer_cb(uint8_t rhport, uint8_t stage, tusb_control_request_t const *request) {
    return false; // Currently no support for Vendor control transfers on the Python side
}

// Generic "runtime device" TinyUSB class driver, delegates everything to Python callbacks

static void runtime_dev_init(void) {
}

static void runtime_dev_reset(uint8_t rhport) {
    mp_obj_usb_device_t *usbd = MP_OBJ_TO_PTR(MP_STATE_VM(usbd));
    if (!usbd) {
        return;
    }

    for (int epnum = 0; epnum < CFG_TUD_ENDPPOINT_MAX; epnum++) {
        for (int dir = 0; dir < 2; dir++) {
            usbd->xfer_data[epnum][dir] = mp_const_none;
        }
    }

    if (mp_obj_is_callable(usbd->reset_cb)) {
        usbd_callback_function_n(usbd->reset_cb, 0, NULL);
    }
}

// Calculate how many interfaces TinyUSB expects us to claim from
// driver open().
//
// Annoyingly, the calling function (process_set_config() in TinyUSB) knows
// this but doesn't pass the information to us.
//
// The answer is:
// - If an Interface Association Descriptor (IAD) is immediately before itf_desc
//   in the configuration descriptor, then claim all of the associated interfaces.
// - Otherwise, claim exactly one interface
//
// Relying on the implementation detail that itf_desc is a pointer inside the
// tud_descriptor_configuration_cb() result. Therefore, we can iterate through
// from the beginning to check for an IAD immediately preceding it.
//
// Returns the number of associated interfaces to claim.
static uint8_t _runtime_dev_count_itfs(tusb_desc_interface_t const *itf_desc) {
    const tusb_desc_configuration_t *cfg_desc = (const void *)tud_descriptor_configuration_cb(0);
    const uint8_t *p_desc = (const void *)cfg_desc;
    const uint8_t *p_end = p_desc + cfg_desc->wTotalLength;
    assert(p_desc <= itf_desc && itf_desc < p_end);
    while (p_desc != (const void *)itf_desc && p_desc < p_end) {
        const uint8_t *next = tu_desc_next(p_desc);

        if (tu_desc_type(p_desc) == TUSB_DESC_INTERFACE_ASSOCIATION
            && next == (const void *)itf_desc) {
            const tusb_desc_interface_assoc_t *desc_iad = (const void *)p_desc;
            return desc_iad->bInterfaceCount;
        }
        p_desc = next;
    }
    return 1; // No IAD found
}

// Scan the other descriptors after these interface(s) to find the total associated length to claim
// from driver open().
//
// Total number of interfaces to scan for is assoc_itf_count.
//
// Opens any associated endpoints so they can have transfers submitted against them.
//
// Returns the total number of descriptor bytes to claim.
static uint16_t _runtime_dev_claim_itfs(tusb_desc_interface_t const *itf_desc, uint8_t assoc_itf_count, uint16_t max_len) {
    const uint8_t *p_desc = (const void *)itf_desc;
    const uint8_t *p_end = p_desc + max_len;
    while (p_desc < p_end) {
        if (tu_desc_type(p_desc) == TUSB_DESC_INTERFACE) {
            if (assoc_itf_count > 0) {
                // Claim this interface descriptor
                assoc_itf_count--;
            } else {
                // This is the end of the previous interface's associated descriptors
                break;
            }
        } else if (tu_desc_type(p_desc) == TUSB_DESC_ENDPOINT) {
            // Open any endpoints that we come across
            if (tu_desc_type(p_desc) == TUSB_DESC_ENDPOINT) {
                bool r = usbd_edpt_open(USBD_RHPORT, (const void *)p_desc);
                if (!r) {
                    mp_obj_t exc = mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(MP_ENODEV));
                    usbd_pend_exception(exc);
                    break;
                }
            }
        }
        p_desc = tu_desc_next(p_desc);
    }
    return p_desc - (const uint8_t *)itf_desc;
}

// TinyUSB "Application driver" open callback. Called when the USB host sets
// configuration. Returns number of bytes to claim from descriptors pointed to
// by itf_desc.
//
// This is a little fiddly as it's called before any built-in TinyUSB drivers,
// but we don't want to override those.
//
// Also, TinyUSB expects us to know how many interfaces to claim for each time
// this function is called, and will behave unexpectedly if we claim the wrong
// number of interfaces. However, unlike a "normal" USB driver we don't know at
// compile time how many interfaces we've implemented. Instead, we have to look
// back through the configuration descriptor to figure this out.
static uint16_t runtime_dev_open(uint8_t rhport, tusb_desc_interface_t const *itf_desc, uint16_t max_len) {
    mp_obj_usb_device_t *usbd = MP_OBJ_TO_PTR(MP_STATE_VM(usbd));

    // Runtime USB isn't initialised
    if (!usbd) {
        return 0;
    }

    // If TinyUSB built-in drivers are enabled, don't claim any interface in the built-in range
    if (mp_usb_device_builtin_enabled(usbd) && itf_desc->bInterfaceNumber < USBD_ITF_BUILTIN_MAX) {
        return 0;
    }

    // Determine the total descriptor length of the interface(s) we are going to claim
    uint8_t assoc_itf_count = _runtime_dev_count_itfs(itf_desc);
    uint16_t claim_len = _runtime_dev_claim_itfs(itf_desc, assoc_itf_count, max_len);

    // Call the Python callback to allow the driver to start working with these interface(s)

    if (mp_obj_is_callable(usbd->open_itf_cb)) {
        // Repurpose the control_data memoryview to point into itf_desc for this one call
        usbd->control_data->items = (void *)itf_desc;
        usbd->control_data->len = claim_len;
        mp_obj_t args[] = { MP_OBJ_FROM_PTR(usbd->control_data) };
        usbd_callback_function_n(usbd->open_itf_cb, 1, args);
        usbd->control_data->len = 0;
        usbd->control_data->items = NULL;
    }

    return claim_len;
}

static bool runtime_dev_control_xfer_cb(uint8_t rhport, uint8_t stage, tusb_control_request_t const *request) {
    mp_obj_t cb_res = mp_const_false;
    mp_obj_usb_device_t *usbd = MP_OBJ_TO_PTR(MP_STATE_VM(usbd));
    tusb_dir_t dir = request->bmRequestType_bit.direction;
    mp_buffer_info_t buf_info;
    bool result;

    if (!usbd) {
        return false;
    }

    if (mp_obj_is_callable(usbd->control_xfer_cb)) {
        usbd->control_data->items = (void *)request;
        usbd->control_data->len = sizeof(tusb_control_request_t);
        mp_obj_t args[] = {
            mp_obj_new_int(stage),
            MP_OBJ_FROM_PTR(usbd->control_data),
        };
        cb_res = usbd_callback_function_n(usbd->control_xfer_cb, MP_ARRAY_SIZE(args), args);
        usbd->control_data->items = NULL;
        usbd->control_data->len = 0;

        if (cb_res == MP_OBJ_NULL) {
            // Exception occurred in the callback handler, stall this transfer
            cb_res = mp_const_false;
        }
    }

    // Check if callback returned any data to submit
    if (mp_get_buffer(cb_res, &buf_info, dir == TUSB_DIR_IN ? MP_BUFFER_READ : MP_BUFFER_RW)) {
        result = tud_control_xfer(USBD_RHPORT,
            request,
            buf_info.buf,
            buf_info.len);

        if (result) {
            // Keep buffer object alive until the transfer completes
            usbd->xfer_data[0][dir] = cb_res;
        }
    } else {
        // Expect True or False to stall or continue
        result = mp_obj_is_true(cb_res);

        if (stage == CONTROL_STAGE_SETUP && result) {
            // If no additional data but callback says to continue transfer then
            // queue a status response.
            tud_control_status(rhport, request);
        } else if (stage == CONTROL_STAGE_ACK) {
            // Allow data to be GCed once it's no longer in use
            usbd->xfer_data[0][dir] = mp_const_none;
        }
    }

    return result;
}

static bool runtime_dev_xfer_cb(uint8_t rhport, uint8_t ep_addr, xfer_result_t result, uint32_t xferred_bytes) {
    mp_obj_t ep = mp_obj_new_int(ep_addr);
    mp_obj_t cb_res = mp_const_false;
    mp_obj_usb_device_t *usbd = MP_OBJ_TO_PTR(MP_STATE_VM(usbd));
    if (!usbd) {
        return false;
    }

    if (mp_obj_is_callable(usbd->xfer_cb)) {
        mp_obj_t args[] = {
            ep,
            MP_OBJ_NEW_SMALL_INT(result),
            MP_OBJ_NEW_SMALL_INT(xferred_bytes),
        };
        cb_res = usbd_callback_function_n(usbd->xfer_cb, MP_ARRAY_SIZE(args), args);
    }

    // Clear any xfer_data for this endpoint
    usbd->xfer_data[tu_edpt_number(ep_addr)][tu_edpt_dir(ep_addr)] = mp_const_none;

    return cb_res != MP_OBJ_NULL && mp_obj_is_true(cb_res);
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


// Functions below here (named mp_usbd_xyz) apply to the whole TinyUSB C-based subsystem
// and not necessarily the USBD singleton object (named usbd_xyz).

// To support soft reset clearing USB runtime state, we manage three TinyUSB states:
//
// - "Not initialised" - tusb_inited() returns false, no USB at all). Only way
//   back to this state is hard reset.
//
// - "Activated" - tusb_inited() returns true, USB device "connected" at device
//   end and available to host.
//
// - "Deactivated" - tusb_inited() returns true, but USB device "disconnected"
//   at device end and host can't see it.


// Top-level USB device subsystem init.
//
// Initialises TinyUSB and/or re-activates it, provided USB is needed.
//
// This is called on any soft reset after boot.py runs, or on demand if the
// user activates USB and it hasn't activated yet.
void mp_usbd_init(void) {
    mp_obj_usb_device_t *usbd = MP_OBJ_TO_PTR(MP_STATE_VM(usbd));
    bool need_usb;

    if (usbd == NULL) {
        // No runtime USB device
        #if CFG_TUD_CDC || CFG_TUD_MSC
        // Builtin  drivers are available, so initialise as defaults
        need_usb = true;
        #else
        // No builtin drivers, nothing to initialise
        need_usb = false;
        #endif
    } else {
        // Otherwise, initialise based on whether runtime USB is active
        need_usb = usbd->active;
    }

    if (need_usb) {
        tusb_init(); // Safe to call redundantly
        tud_connect(); // Reconnect if mp_usbd_deinit() has disconnected
    }
}

// Top-level USB device deinit.
//
// This variant is called from soft reset, NULLs out the USB device
// singleton instance from MP_STATE_VM, and disconnects the port if a
// runtime device was active.
void mp_usbd_deinit(void) {
    mp_obj_usb_device_t *usbd = MP_OBJ_TO_PTR(MP_STATE_VM(usbd));
    MP_STATE_VM(usbd) = MP_OBJ_NULL;
    if (usbd && usbd->active) {
        // Disconnect if a runtime USB device was active
        mp_usbd_disconnect(usbd);
    }
}

// Thin wrapper around tud_disconnect() that tells TinyUSB all endpoints
// have stalled, to prevent it getting confused if a transfer is in progress.
static void mp_usbd_disconnect(mp_obj_usb_device_t *usbd) {
    if (!tusb_inited()) {
        return; // TinyUSB hasn't initialised
    }

    if (usbd) {
        // There might be USB transfers in progress right now, so need to stall any live
        // endpoints
        //
        // TODO: figure out if we really need this
        for (int epnum = 0; epnum < CFG_TUD_ENDPPOINT_MAX; epnum++) {
            for (int dir = 0; dir < 2; dir++) {
                if (usbd->xfer_data[epnum][dir] != mp_const_none) {
                    usbd_edpt_stall(USBD_RHPORT, tu_edpt_addr(epnum, dir));
                    usbd->xfer_data[epnum][dir] = mp_const_none;
                }
            }
        }
    }

    #if MICROPY_HW_USB_CDC
    // Ensure no pending static CDC writes, as these can cause TinyUSB to crash
    tud_cdc_write_clear();
    // Prevent cdc write flush from initiating any new transfers while disconnecting
    usbd_edpt_stall(USBD_RHPORT, USBD_CDC_EP_IN);
    #endif

    bool was_connected = tud_connected();
    tud_disconnect();
    if (was_connected) {
        // Need to ensure a long enough delay before TinyUSB re-connects that
        // the host triggers a bus reset. This may happen anyway, but delaying here
        // lets us be "belt and braces" sure.
        mp_hal_delay_ms(50);
    }
}

// Thjs callback is queued by mp_usbd_schedule_task() to process USB later.
void mp_usbd_task_callback(mp_sched_node_t *node) {
    if (tud_inited() && !in_usbd_task) {
        mp_usbd_task_inner();
    }
    // If in_usbd_task is set, it means something else has already manually called
    // mp_usbd_task() (most likely: C-based USB-CDC serial port). Now the MP
    // scheduler is running inside there and triggering this callback. It's OK
    // to skip, the already-running outer TinyUSB task will process all pending
    // events before it returns.
}

// Task function can be called manually to force processing of USB events
// (mostly from USB-CDC serial port when blocking.)
void mp_usbd_task(void) {
    if (in_usbd_task) {
        // If this exception triggers, it means a USB callback tried to do
        // something that itself became blocked on TinyUSB (most likely: read or
        // write from a C-based USB-CDC serial port.)
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("TinyUSB callback can't recurse"));
    }

    mp_usbd_task_inner();
}

static void mp_usbd_task_inner(void) {
    in_usbd_task = true;

    tud_task_ext(0, false);

    mp_obj_usb_device_t *usbd = MP_OBJ_TO_PTR(MP_STATE_VM(usbd));

    // Check for a triggered change to/from active state
    if (usbd && usbd->trigger) {
        if (usbd->active) {
            if (tud_connected()) {
                // If a SETUP packet has been received, first disconnect
                // and wait for the host to recognise this and trigger a bus reset.
                //
                // Effectively this forces it to re-enumerate the device.
                mp_usbd_disconnect(usbd);
            }
            tud_connect();
        } else {
            mp_usbd_disconnect(usbd);
        }
        usbd->trigger = false;
    }

    in_usbd_task = false;

    if (usbd) {
        // Print any exceptions that were raised by Python callbacks
        // inside tud_task_ext(). See usbd_callback_function_n.

        // As printing exceptions to USB-CDC may recursively call mp_usbd_task(),
        // first copy out the pending data to the local stack
        mp_uint_t num_pend_excs = usbd->num_pend_excs;
        mp_obj_t pend_excs[MP_USBD_MAX_PEND_EXCS];
        for (mp_uint_t i = 0; i < MIN(MP_USBD_MAX_PEND_EXCS, num_pend_excs); i++) {
            pend_excs[i] = usbd->pend_excs[i];
            usbd->pend_excs[i] = mp_const_none;
        }
        usbd->num_pend_excs = 0;

        // Now print the exceptions stored from this mp_usbd_task() call
        for (mp_uint_t i = 0; i < MIN(MP_USBD_MAX_PEND_EXCS, num_pend_excs); i++) {
            mp_obj_print_exception(&mp_plat_print, pend_excs[i]);
        }
        if (num_pend_excs > MP_USBD_MAX_PEND_EXCS) {
            mp_printf(&mp_plat_print, "%u additional exceptions in USB callbacks\n",
                num_pend_excs - MP_USBD_MAX_PEND_EXCS);
        }
    }
}

#endif // MICROPY_HW_ENABLE_USB_RUNTIME_DEVICE
