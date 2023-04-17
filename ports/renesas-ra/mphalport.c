/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018,2021 Damien P. George
 * Copyright (c) 2021,2022 Renesas Electronics Corporation
 * Copyright (c) 2023 Arduino SA
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
#include "py/stream.h"
#include "py/mphal.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/ringbuf.h"
#include "extmod/misc.h"
#include "shared/runtime/interrupt_char.h"
#include "tusb.h"
#include "uart.h"

#if MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE
void flash_cache_commit(void);
#endif

#if MICROPY_HW_ENABLE_UART_REPL || MICROPY_HW_USB_CDC

#ifndef MICROPY_HW_STDIN_BUFFER_LEN
#define MICROPY_HW_STDIN_BUFFER_LEN 512
#endif

STATIC uint8_t stdin_ringbuf_array[MICROPY_HW_STDIN_BUFFER_LEN];
ringbuf_t stdin_ringbuf = { stdin_ringbuf_array, sizeof(stdin_ringbuf_array) };

#endif

// this table converts from HAL_StatusTypeDef to POSIX errno
const byte mp_hal_status_to_errno_table[4] = {
    [HAL_OK] = 0,
    [HAL_ERROR] = MP_EIO,
    [HAL_BUSY] = MP_EBUSY,
    [HAL_TIMEOUT] = MP_ETIMEDOUT,
};

NORETURN void mp_hal_raise(HAL_StatusTypeDef status) {
    mp_raise_OSError(mp_hal_status_to_errno_table[status]);
}

#if MICROPY_HW_USB_CDC

uint8_t cdc_itf_pending; // keep track of cdc interfaces which need attention to poll

void poll_cdc_interfaces(void) {
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
}

void tud_cdc_rx_cb(uint8_t itf) {
    // consume pending USB data immediately to free usb buffer and keep the endpoint from stalling.
    // in case the ringbuffer is full, mark the CDC interface that need attention later on for polling
    cdc_itf_pending &= ~(1 << itf);
    for (uint32_t bytes_avail = tud_cdc_n_available(itf); bytes_avail > 0; --bytes_avail) {
        if (ringbuf_free(&stdin_ringbuf)) {
            int data_char = tud_cdc_read_char();
            if (data_char == mp_interrupt_char) {
                mp_sched_keyboard_interrupt();
            } else {
                ringbuf_put(&stdin_ringbuf, data_char);
            }
        } else {
            cdc_itf_pending |= (1 << itf);
            return;
        }
    }
}

#endif

uintptr_t mp_hal_stdio_poll(uintptr_t poll_flags) {
    uintptr_t ret = 0;
    #if MICROPY_HW_USB_CDC
    poll_cdc_interfaces();
    #endif
    #if MICROPY_HW_ENABLE_UART_REPL || MICROPY_HW_USB_CDC
    if ((poll_flags & MP_STREAM_POLL_RD) && ringbuf_peek(&stdin_ringbuf) != -1) {
        ret |= MP_STREAM_POLL_RD;
    }
    if (poll_flags & MP_STREAM_POLL_WR) {
        #if MICROPY_HW_ENABLE_UART_REPL
        ret |= MP_STREAM_POLL_WR;
        #else
        if (tud_cdc_connected() && tud_cdc_write_available() > 0) {
            ret |= MP_STREAM_POLL_WR;
        }
        #endif
    }
    #endif
    #if MICROPY_PY_OS_DUPTERM
    ret |= mp_os_dupterm_poll(poll_flags);
    #endif
    return ret;
}

// Receive single character
int mp_hal_stdin_rx_chr(void) {
    for (;;) {
        #if MICROPY_HW_USB_CDC
        poll_cdc_interfaces();
        #endif

        #if MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE
        flash_cache_commit();
        #endif

        int c = ringbuf_get(&stdin_ringbuf);
        if (c != -1) {
            return c;
        }
        #if MICROPY_PY_OS_DUPTERM
        int dupterm_c = mp_os_dupterm_rx_chr();
        if (dupterm_c >= 0) {
            return dupterm_c;
        }
        #endif
        MICROPY_EVENT_POLL_HOOK
    }
}

// Send string of given length
void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
    #if MICROPY_HW_ENABLE_UART_REPL
    if (MP_STATE_PORT(pyb_stdio_uart) != NULL) {
        uart_tx_strn(MP_STATE_PORT(pyb_stdio_uart), str, len);
    }
    #endif

    #if MICROPY_HW_USB_CDC
    if (tud_cdc_connected()) {
        for (size_t i = 0; i < len;) {
            uint32_t n = len - i;
            if (n > CFG_TUD_CDC_EP_BUFSIZE) {
                n = CFG_TUD_CDC_EP_BUFSIZE;
            }
            int timeout = 0;
            // Wait with a max of USC_CDC_TIMEOUT ms
            while (n > tud_cdc_write_available() && timeout++ < MICROPY_HW_USB_CDC_TX_TIMEOUT) {
                MICROPY_EVENT_POLL_HOOK
            }
            if (timeout >= MICROPY_HW_USB_CDC_TX_TIMEOUT) {
                break;
            }
            uint32_t n2 = tud_cdc_write(str + i, n);
            tud_cdc_write_flush();
            i += n2;
        }
    }
    #endif

    #if MICROPY_PY_OS_DUPTERM
    mp_os_dupterm_tx_strn(str, len);
    #endif
}

void mp_hal_ticks_cpu_enable(void) {
}

void mp_hal_pin_config(mp_hal_pin_obj_t pin_obj, uint32_t mode, uint32_t pull, uint32_t drive, uint32_t alt) {
    ra_gpio_config(pin_obj->pin, mode, pull, drive, alt);
}

/*******************************************************************************/
// MAC address

// Generate a random locally administered MAC address (LAA)
void mp_hal_generate_laa_mac(int idx, uint8_t buf[6]) {
    uint8_t *id = (uint8_t *)MP_HAL_UNIQUE_ID_ADDRESS;
    buf[0] = 0x02; // LAA range
    buf[1] = (id[11] << 4) | (id[10] & 0xf);
    buf[2] = (id[9] << 4) | (id[8] & 0xf);
    buf[3] = (id[7] << 4) | (id[6] & 0xf);
    buf[4] = id[2];
    buf[5] = (id[0] << 2) | idx;
}

// A board can override this if needed
MP_WEAK void mp_hal_get_mac(int idx, uint8_t buf[6]) {
    mp_hal_generate_laa_mac(idx, buf);
}

void mp_hal_get_mac_ascii(int idx, size_t chr_off, size_t chr_len, char *dest) {
    static const char hexchr[16] = "0123456789ABCDEF";
    uint8_t mac[6];
    mp_hal_get_mac(idx, mac);
    for (; chr_len; ++chr_off, --chr_len) {
        *dest++ = hexchr[mac[chr_off >> 1] >> (4 * (1 - (chr_off & 1))) & 0xf];
    }
}

#if MICROPY_HW_ENABLE_USBDEV
void usbfs_interrupt_handler(void) {
    IRQn_Type irq = R_FSP_CurrentIrqGet();
    R_BSP_IrqStatusClear(irq);

    #if CFG_TUSB_RHPORT0_MODE & OPT_MODE_HOST
    tuh_int_handler(0);
    #endif

    #if CFG_TUSB_RHPORT0_MODE & OPT_MODE_DEVICE
    tud_int_handler(0);
    #endif
}

void usbfs_resume_handler(void) {
    IRQn_Type irq = R_FSP_CurrentIrqGet();
    R_BSP_IrqStatusClear(irq);

    #if CFG_TUSB_RHPORT0_MODE & OPT_MODE_HOST
    tuh_int_handler(0);
    #endif

    #if CFG_TUSB_RHPORT0_MODE & OPT_MODE_DEVICE
    tud_int_handler(0);
    #endif
}

void usbfs_d0fifo_handler(void) {
    IRQn_Type irq = R_FSP_CurrentIrqGet();
    R_BSP_IrqStatusClear(irq);

    #if CFG_TUSB_RHPORT0_MODE & OPT_MODE_HOST
    tuh_int_handler(0);
    #endif

    #if CFG_TUSB_RHPORT0_MODE & OPT_MODE_DEVICE
    tud_int_handler(0);
    #endif
}

void usbfs_d1fifo_handler(void) {
    IRQn_Type irq = R_FSP_CurrentIrqGet();
    R_BSP_IrqStatusClear(irq);

    #if CFG_TUSB_RHPORT0_MODE & OPT_MODE_HOST
    tuh_int_handler(0);
    #endif

    #if CFG_TUSB_RHPORT0_MODE & OPT_MODE_DEVICE
    tud_int_handler(0);
    #endif
}

void usbhs_interrupt_handler(void) {
    IRQn_Type irq = R_FSP_CurrentIrqGet();
    R_BSP_IrqStatusClear(irq);

    #if CFG_TUSB_RHPORT1_MODE & OPT_MODE_HOST
    tuh_int_handler(1);
    #endif

    #if CFG_TUSB_RHPORT1_MODE & OPT_MODE_DEVICE
    tud_int_handler(1);
    #endif
}

void usbhs_d0fifo_handler(void) {
    IRQn_Type irq = R_FSP_CurrentIrqGet();
    R_BSP_IrqStatusClear(irq);

    #if CFG_TUSB_RHPORT1_MODE & OPT_MODE_HOST
    tuh_int_handler(1);
    #endif

    #if CFG_TUSB_RHPORT1_MODE & OPT_MODE_DEVICE
    tud_int_handler(1);
    #endif
}

void usbhs_d1fifo_handler(void) {
    IRQn_Type irq = R_FSP_CurrentIrqGet();
    R_BSP_IrqStatusClear(irq);

    #if CFG_TUSB_RHPORT1_MODE & OPT_MODE_HOST
    tuh_int_handler(1);
    #endif

    #if CFG_TUSB_RHPORT1_MODE & OPT_MODE_DEVICE
    tud_int_handler(1);
    #endif
}
#endif

MP_REGISTER_ROOT_POINTER(struct _machine_uart_obj_t *pyb_stdio_uart);
