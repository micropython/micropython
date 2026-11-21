/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Dryw Wade Clere at SparkFun Electronics
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

#include <string.h>

#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mphal.h"

#if MICROPY_HW_USB_HOST

#include "shared/runtime/softtimer.h"
#include "mp_usbh.h"
#include "host/hcd.h"

// TinyUSB waits out some enumeration delays without raising an event, so the host task
// also runs periodically while the host is active.
#define MP_USBH_TASK_PERIOD_MS (10)

// Control transfer data goes through this buffer, which is suitable for DMA.
#define MP_USBH_CTRL_BUF_SIZE (64)

static bool mp_usbh_active;
static soft_timer_entry_t mp_usbh_timer;

static volatile xfer_result_t mp_usbh_ctrl_result;
static volatile uint32_t mp_usbh_ctrl_len;
CFG_TUH_MEM_SECTION static uint8_t mp_usbh_ctrl_buf[MP_USBH_CTRL_BUF_SIZE] TU_ATTR_ALIGNED(CFG_TUH_MEM_DCACHE_LINE_SIZE);

#if CFG_TUSB_OS == OPT_OS_NONE
// TinyUSB's host stack times enumeration delays with this.
uint32_t tusb_time_millis_api(void) {
    return mp_hal_ticks_ms();
}
#endif

void mp_usbh_task(void) {
    tuh_task_ext(0, false);
}

static void mp_usbh_task_callback(mp_sched_node_t *node) {
    (void)node;
    mp_usbh_task();
}

static void mp_usbh_schedule_task(void) {
    static mp_sched_node_t node;
    mp_sched_schedule_node(&node, mp_usbh_task_callback);
}

TU_ATTR_FAST_FUNC void tuh_event_hook_cb(uint8_t rhport, uint32_t eventid, bool in_isr) {
    (void)rhport;
    (void)eventid;
    (void)in_isr;
    mp_usbh_schedule_task();
    mp_hal_wake_main_task_from_isr();
}

static void mp_usbh_timer_callback(soft_timer_entry_t *self) {
    (void)self;
    mp_usbh_schedule_task();
}

// Class drivers implemented by MicroPython, in addition to TinyUSB's own.
usbh_class_driver_t const *usbh_app_driver_get_cb(uint8_t *driver_count) {
    #if MICROPY_HW_USB_HOST_UVC
    *driver_count = 1;
    return &mp_usbh_uvc_driver;
    #else
    *driver_count = 0;
    return NULL;
    #endif
}

bool mp_usbh_init(void) {
    if (mp_usbh_active) {
        return true;
    }

    MICROPY_HW_USB_HOST_LL_INIT();
    mp_usbh_hcd_pre_init(TUH_OPT_RHPORT);
    const tusb_rhport_init_t host_init = {
        .role = TUSB_ROLE_HOST,
        .speed = TUH_OPT_HIGH_SPEED ? TUSB_SPEED_HIGH : TUSB_SPEED_FULL,
    };
    if (!tusb_rhport_init(TUH_OPT_RHPORT, &host_init)) {
        MICROPY_HW_USB_HOST_LL_DEINIT();
        return false;
    }
    mp_usbh_hcd_post_init(TUH_OPT_RHPORT);

    mp_usbh_active = true;
    soft_timer_static_init(&mp_usbh_timer, SOFT_TIMER_MODE_PERIODIC, MP_USBH_TASK_PERIOD_MS, mp_usbh_timer_callback);
    soft_timer_insert(&mp_usbh_timer, MP_USBH_TASK_PERIOD_MS);
    return true;
}

void mp_usbh_deinit(void) {
    #if MICROPY_HW_USB_HOST_UVC
    // Stops any video stream, so must come before the controller is stopped.
    mp_usbh_uvc_deinit();
    #endif

    if (!mp_usbh_active) {
        return;
    }
    soft_timer_remove(&mp_usbh_timer);
    mp_usbh_iso_in_stop();
    tuh_deinit(TUH_OPT_RHPORT);
    MICROPY_HW_USB_HOST_LL_DEINIT();
    mp_usbh_active = false;
}

bool mp_usbh_is_active(void) {
    return mp_usbh_active;
}

static void mp_usbh_control_xfer_cb(tuh_xfer_t *xfer) {
    mp_usbh_ctrl_len = xfer->actual_len;
    mp_usbh_ctrl_result = xfer->result;
}

int mp_usbh_control_xfer(uint8_t daddr, uint8_t bmRequestType, uint8_t bRequest,
    uint16_t wValue, uint16_t wIndex, void *buf, uint16_t wLength, uint32_t timeout_ms) {
    if (wLength > MP_USBH_CTRL_BUF_SIZE) {
        return -MP_EINVAL;
    }

    const bool dir_in = bmRequestType & TUSB_DIR_IN_MASK;
    if (!dir_in && wLength > 0) {
        memcpy(mp_usbh_ctrl_buf, buf, wLength);
    }
    hcd_dcache_clean_invalidate(mp_usbh_ctrl_buf, MP_USBH_CTRL_BUF_SIZE);

    const tusb_control_request_t request = {
        .bmRequestType = bmRequestType,
        .bRequest = bRequest,
        .wValue = tu_htole16(wValue),
        .wIndex = tu_htole16(wIndex),
        .wLength = tu_htole16(wLength),
    };
    tuh_xfer_t xfer = {
        .daddr = daddr,
        .ep_addr = 0,
        .setup = &request,
        .buffer = wLength > 0 ? mp_usbh_ctrl_buf : NULL,
        .complete_cb = mp_usbh_control_xfer_cb,
    };
    mp_usbh_ctrl_result = XFER_RESULT_INVALID;
    mp_usbh_ctrl_len = 0;
    if (!tuh_control_xfer(&xfer)) {
        return -MP_EIO;
    }

    // TinyUSB's blocking mode has no timeout, so wait here instead.
    mp_uint_t t0 = mp_hal_ticks_ms();
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        while (mp_usbh_ctrl_result == XFER_RESULT_INVALID && mp_hal_ticks_ms() - t0 < timeout_ms) {
            mp_usbh_task();
            mp_event_wait_ms(1);
        }
        nlr_pop();
    } else {
        tuh_edpt_abort_xfer(daddr, 0);
        nlr_jump(nlr.ret_val);
    }

    switch (mp_usbh_ctrl_result) {
        case XFER_RESULT_SUCCESS:
            break;
        case XFER_RESULT_INVALID:
            tuh_edpt_abort_xfer(daddr, 0);
            return -MP_ETIMEDOUT;
        case XFER_RESULT_STALLED:
            return -MP_EPIPE;
        default:
            return -MP_EIO;
    }

    uint32_t len = MIN(mp_usbh_ctrl_len, wLength);
    if (dir_in && len > 0) {
        memcpy(buf, mp_usbh_ctrl_buf, len);
    }
    return len;
}

#endif // MICROPY_HW_USB_HOST
