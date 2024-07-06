// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Patrick Van Oosterwijck
// SPDX-FileCopyrightText: Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/ringbuf.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "supervisor/port.h"
#include "supervisor/usb_serial_jtag.h"

#include "hal/usb_serial_jtag_ll.h"
#include "esp_intr_alloc.h"
#include "soc/periph_defs.h"


#define USB_SERIAL_JTAG_BUF_SIZE (64)

static ringbuf_t ringbuf;
static uint8_t buf[128];
static volatile bool connected;

#if CIRCUITPY_ESP_USB_SERIAL_JTAG && defined(SOC_WIFI_PHY_NEEDS_USB_WORKAROUND) && !defined(CONFIG_ESP_PHY_ENABLE_USB)
#error "CONFIG_ESP_PHY_ENABLE_USB must be enabled in sdkconfig"
#endif

// Make sure the recv interrupt is disabled during this. Otherwise, it could reorder data if it
// interrupts itself.
static void _copy_out_of_fifo(void) {
    size_t req_len = ringbuf_num_empty(&ringbuf);
    if (req_len == 0) {
        // Disable the interrupt so that CircuitPython can run and process the ringbuf. It will
        // re-enable the interrupt once the ringbuf is empty.
        usb_serial_jtag_ll_disable_intr_mask(USB_SERIAL_JTAG_INTR_SERIAL_OUT_RECV_PKT);
    }
    if (req_len > USB_SERIAL_JTAG_BUF_SIZE) {
        req_len = USB_SERIAL_JTAG_BUF_SIZE;
    }
    uint8_t rx_buf[USB_SERIAL_JTAG_BUF_SIZE];

    // Read up to req_len bytes. Does not block.
    size_t len = usb_serial_jtag_ll_read_rxfifo(rx_buf, req_len);

    for (size_t i = 0; i < len; ++i) {
        if (rx_buf[i] == mp_interrupt_char) {
            mp_sched_keyboard_interrupt();
        } else {
            ringbuf_put(&ringbuf, rx_buf[i]);
        }
    }
}

static void usb_serial_jtag_isr_handler(void *arg) {
    uint32_t flags = usb_serial_jtag_ll_get_intsts_mask();

    if (flags & USB_SERIAL_JTAG_INTR_SOF) {
        usb_serial_jtag_ll_clr_intsts_mask(USB_SERIAL_JTAG_INTR_SOF);
    }

    if (flags & USB_SERIAL_JTAG_INTR_TOKEN_REC_IN_EP1) {
        usb_serial_jtag_ll_clr_intsts_mask(USB_SERIAL_JTAG_INTR_TOKEN_REC_IN_EP1);
        connected = true;
    }

    if (flags & USB_SERIAL_JTAG_INTR_SERIAL_OUT_RECV_PKT) {
        // New bytes are in the FIFO. Read them and check for keyboard interrupt.
        usb_serial_jtag_ll_clr_intsts_mask(USB_SERIAL_JTAG_INTR_SERIAL_OUT_RECV_PKT);
        // This is executed at interrupt level, so we don't explicitly need to make it atomic.
        _copy_out_of_fifo();
        port_wake_main_task_from_isr();
    }
}

void usb_serial_jtag_init(void) {
    ringbuf_init(&ringbuf, buf, sizeof(buf));
    usb_serial_jtag_ll_clr_intsts_mask(USB_SERIAL_JTAG_INTR_SOF | USB_SERIAL_JTAG_INTR_SERIAL_OUT_RECV_PKT | USB_SERIAL_JTAG_INTR_TOKEN_REC_IN_EP1);
    usb_serial_jtag_ll_ena_intr_mask(USB_SERIAL_JTAG_INTR_SOF | USB_SERIAL_JTAG_INTR_SERIAL_OUT_RECV_PKT | USB_SERIAL_JTAG_INTR_TOKEN_REC_IN_EP1);
    ESP_ERROR_CHECK(esp_intr_alloc(ETS_USB_SERIAL_JTAG_INTR_SOURCE, ESP_INTR_FLAG_LEVEL1,
        usb_serial_jtag_isr_handler, NULL, NULL));
}

bool usb_serial_jtag_connected(void) {
    return connected;
}

char usb_serial_jtag_read_char(void) {
    uint32_t num_filled = ringbuf_num_filled(&ringbuf);

    if (num_filled == 0 && !usb_serial_jtag_ll_rxfifo_data_available()) {
        return -1;
    }
    char c = -1;

    if (num_filled > 0) {
        common_hal_mcu_disable_interrupts();
        c = ringbuf_get(&ringbuf);
        common_hal_mcu_enable_interrupts();

        num_filled--;
    }

    // Maybe re-enable the recv interrupt if we've emptied the ringbuf.
    if (num_filled == 0) {
        usb_serial_jtag_ll_disable_intr_mask(USB_SERIAL_JTAG_INTR_SERIAL_OUT_RECV_PKT);
        _copy_out_of_fifo();

        // May have only been ctrl-c.
        if (c == -1 && ringbuf_num_filled(&ringbuf) > 0) {
            c = ringbuf_get(&ringbuf);
        }
        usb_serial_jtag_ll_ena_intr_mask(USB_SERIAL_JTAG_INTR_SERIAL_OUT_RECV_PKT);
    }
    return c;
}

uint32_t usb_serial_jtag_bytes_available(void) {
    // Atomically get the number of bytes in the ringbuf plus what is not yet in the ringbuf.
    common_hal_mcu_disable_interrupts();
    const uint32_t count = ringbuf_num_filled(&ringbuf) + usb_serial_jtag_ll_rxfifo_data_available();
    common_hal_mcu_enable_interrupts();
    return count;
}

void usb_serial_jtag_write(const char *text, uint32_t length) {
    if (!usb_serial_jtag_connected()) {
        return;
    }
    size_t total_written = 0;
    while (total_written < length) {
        uint32_t start_time = supervisor_ticks_ms32();
        // Wait until we can write to the FIFO again. If it takes too long, then
        // assume we're disconnected.
        while (!usb_serial_jtag_ll_txfifo_writable()) {
            uint32_t now = supervisor_ticks_ms32();
            if (now - start_time > 200) {
                connected = false;
                return;
            }
        }
        total_written += usb_serial_jtag_ll_write_txfifo((const uint8_t *)(text + total_written), length - total_written);
        RUN_BACKGROUND_TASKS;
    }
    usb_serial_jtag_ll_txfifo_flush();
}
