/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Ibrahim Abdelkader <iabdalkader@openmv.io>
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

#include "py/runtime.h"
#include "py/mphal.h"
#include "py/stream.h"
#include "extmod/modmachine.h"

#include "mp_usbd.h"
#include "mp_usbd_cdc.h"

#if MICROPY_HW_USB_CDC && MICROPY_HW_ENABLE_USBDEV && !MICROPY_EXCLUDE_SHARED_TINYUSB_USBD_CDC

static uint8_t cdc_itf_pending; // keep track of cdc interfaces which need attention to poll
static int8_t cdc_connected_flush_delay = 0;

uintptr_t mp_usbd_cdc_poll_interfaces(uintptr_t poll_flags) {
    uintptr_t ret = 0;
    if (!cdc_itf_pending) {
        // Explicitly run the USB stack as the scheduler may be locked (eg we are in
        // an interrupt handler) while there is data pending.
        mp_usbd_task();
    }

    // any CDC interfaces left to poll?
    if (cdc_itf_pending && ringbuf_free(&stdin_ringbuf)) {
        for (uint8_t itf = 0; itf < 8; ++itf) {
            if (cdc_itf_pending & (1 << itf)) {
                tud_cdc_rx_cb(itf);
                if (!cdc_itf_pending) {
                    break;
                }
            }
        }
    }
    if ((poll_flags & MP_STREAM_POLL_RD) && ringbuf_peek(&stdin_ringbuf) != -1) {
        ret |= MP_STREAM_POLL_RD;
    }
    if ((poll_flags & MP_STREAM_POLL_WR) &&
        (!tud_cdc_connected() || (tud_cdc_connected() && tud_cdc_write_available() > 0))) {
        // Always allow write when not connected, fifo will retain latest.
        // When connected operate as blocking, only allow if space is available.
        ret |= MP_STREAM_POLL_WR;
    }
    return ret;
}

void tud_cdc_rx_cb(uint8_t itf) {
    // consume pending USB data immediately to free usb buffer and keep the endpoint from stalling.
    // in case the ringbuffer is full, mark the CDC interface that need attention later on for polling
    cdc_itf_pending &= ~(1 << itf);
    for (uint32_t bytes_avail = tud_cdc_n_available(itf); bytes_avail > 0; --bytes_avail) {
        if (ringbuf_free(&stdin_ringbuf)) {
            int data_char = tud_cdc_read_char();
            #if MICROPY_KBD_EXCEPTION
            if (data_char == mp_interrupt_char) {
                // Clear the ring buffer
                stdin_ringbuf.iget = stdin_ringbuf.iput = 0;
                // and stop
                mp_sched_keyboard_interrupt();
            } else {
                ringbuf_put(&stdin_ringbuf, data_char);
            }
            #else
            ringbuf_put(&stdin_ringbuf, data_char);
            #endif
        } else {
            cdc_itf_pending |= (1 << itf);
            return;
        }
    }
}

mp_uint_t mp_usbd_cdc_tx_strn(const char *str, mp_uint_t len) {
    size_t i = 0;
    while (i < len) {
        uint32_t n = len - i;
        if (n > CFG_TUD_CDC_EP_BUFSIZE) {
            n = CFG_TUD_CDC_EP_BUFSIZE;
        }
        if (tud_cdc_connected()) {
            int timeout = 0;
            // If CDC port is connected but the buffer is full, wait for up to USC_CDC_TIMEOUT ms.
            while (n > tud_cdc_write_available() && timeout++ < MICROPY_HW_USB_CDC_TX_TIMEOUT) {
                mp_event_wait_ms(1);

                // Explicitly run the USB stack as the scheduler may be locked (eg we
                // are in an interrupt handler), while there is data pending.
                mp_usbd_task();
            }
            // Limit write to available space in tx buffer when connected.
            n = MIN(n, tud_cdc_write_available());
            if (n == 0) {
                break;
            }
        }
        // When not connected we always write to usb fifo, ensuring it has latest data.
        uint32_t n2 = tud_cdc_write(str + i, n);
        tud_cdc_write_flush();
        i += n2;
    }
    return i;
}

void tud_sof_cb(uint32_t frame_count) {
    if (--cdc_connected_flush_delay < 0) {
        // Finished on-connection delay, disable SOF interrupt again.
        tud_sof_cb_enable(false);
        tud_cdc_write_flush();
    }
}

#endif

#if MICROPY_HW_ENABLE_USBDEV && (MICROPY_HW_USB_CDC_1200BPS_TOUCH || MICROPY_HW_USB_CDC)

#if MICROPY_HW_USB_CDC_1200BPS_TOUCH
static mp_sched_node_t mp_bootloader_sched_node;

static void usbd_cdc_run_bootloader_task(mp_sched_node_t *node) {
    mp_hal_delay_ms(250);
    machine_bootloader(0, NULL);
}
#endif

void
#if MICROPY_HW_USB_EXTERNAL_TINYUSB
mp_usbd_line_state_cb
#else
tud_cdc_line_state_cb
#endif
    (uint8_t itf, bool dtr, bool rts) {
    #if MICROPY_HW_USB_CDC && !MICROPY_EXCLUDE_SHARED_TINYUSB_USBD_CDC
    if (dtr) {
        // A host application has started to open the cdc serial port.
        // Wait a few ms for host to be ready then send tx buffer.
        // High speed connection SOF fires at 125us, full speed at 1ms.
        cdc_connected_flush_delay = (tud_speed_get() == TUSB_SPEED_HIGH) ? 128 : 16;
        tud_sof_cb_enable(true);
    }
    #endif
    #if MICROPY_HW_USB_CDC_1200BPS_TOUCH
    if (dtr == false && rts == false) {
        // Device is disconnected.
        cdc_line_coding_t line_coding;
        tud_cdc_n_get_line_coding(itf, &line_coding);
        if (line_coding.bit_rate == 1200) {
            // Delay bootloader jump to allow the USB stack to service endpoints.
            mp_sched_schedule_node(&mp_bootloader_sched_node, usbd_cdc_run_bootloader_task);
        }
    }
    #endif
}

#endif
