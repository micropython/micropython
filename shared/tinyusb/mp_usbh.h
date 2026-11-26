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
#ifndef MICROPY_INCLUDED_SHARED_TINYUSB_MP_USBH_H
#define MICROPY_INCLUDED_SHARED_TINYUSB_MP_USBH_H

#include "py/obj.h"

#if MICROPY_HW_USB_HOST

// Support for USB Video Class cameras (machine.USBHost.UVC).
#ifndef MICROPY_HW_USB_HOST_UVC
#define MICROPY_HW_USB_HOST_UVC (1)
#endif

#ifndef NO_QSTR
#include "tusb.h"
#include "host/usbh_pvt.h"
#endif

// Ports provide these to power up the host controller (clocks, pins, PHY, VBUS) before
// TinyUSB initialises it, and to power it down again.
#ifndef MICROPY_HW_USB_HOST_LL_INIT
#define MICROPY_HW_USB_HOST_LL_INIT()
#endif
#ifndef MICROPY_HW_USB_HOST_LL_DEINIT
#define MICROPY_HW_USB_HOST_LL_DEINIT()
#endif

// Start and stop the TinyUSB host stack on TUH_OPT_RHPORT.  mp_usbh_deinit() must also
// be called on soft reset.
bool mp_usbh_init(void);
void mp_usbh_deinit(void);
bool mp_usbh_is_active(void);

// Run the TinyUSB host task.  Must be called from thread context.
void mp_usbh_task(void);

// Host controller IRQ entry point.  Ports call this rather than tuh_int_handler().
void mp_usbh_int_handler(uint8_t rhport);

// Run a control transfer and wait for it to finish, for class drivers.  Returns the
// number of bytes transferred, or a negative MP_Exxx error code.  For IN transfers the
// data is written to buf, otherwise it's read from buf.  May raise, eg on Ctrl-C.
int mp_usbh_control_xfer(uint8_t daddr, uint8_t bmRequestType, uint8_t bRequest,
    uint16_t wValue, uint16_t wIndex, void *buf, uint16_t wLength, uint32_t timeout_ms);

// An isochronous IN stream, serviced straight from the host controller IRQ because
// TinyUSB's task queue can't keep up with a transfer every 125us microframe.  The
// callback is called from the IRQ once per (micro)frame: with ok true, data and len
// hold the payload of that (micro)frame (len may be zero), and with ok false the
// (micro)frame was lost to a bus error.  Only one stream can be active.  Controllers
// without support return -MP_EOPNOTSUPP.
typedef void (*mp_usbh_iso_in_cb_t)(void *arg, const uint8_t *data, size_t len, bool ok);
int mp_usbh_iso_in_start(uint8_t daddr, const tusb_desc_endpoint_t *desc_ep, mp_usbh_iso_in_cb_t cb, void *cb_arg);
void mp_usbh_iso_in_stop(void);

// Controller specific hooks, around TinyUSB's initialisation of the controller.
void mp_usbh_hcd_pre_init(uint8_t rhport);
void mp_usbh_hcd_post_init(uint8_t rhport);

#if MICROPY_HW_USB_HOST_UVC
extern const usbh_class_driver_t mp_usbh_uvc_driver;
void mp_usbh_uvc_deinit(void);
#endif

#endif // MICROPY_HW_USB_HOST

#endif // MICROPY_INCLUDED_SHARED_TINYUSB_MP_USBH_H
