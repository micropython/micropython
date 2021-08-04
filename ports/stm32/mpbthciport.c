/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018-2021 Damien P. George
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
#include "extmod/mpbthci.h"
#include "extmod/modbluetooth.h"
#include "mpbthciport.h"
#include "softtimer.h"
#include "pendsv.h"
#include "shared/runtime/mpirq.h"

#if MICROPY_PY_BLUETOOTH

#define DEBUG_printf(...) // printf("mpbthciport.c: " __VA_ARGS__)

uint8_t mp_bluetooth_hci_cmd_buf[4 + 256];

// Soft timer for scheduling a HCI poll.
STATIC soft_timer_entry_t mp_bluetooth_hci_soft_timer;

#if MICROPY_PY_BLUETOOTH_USE_SYNC_EVENTS
// Prevent double-enqueuing of the scheduled task.
STATIC volatile bool events_task_is_scheduled;
#endif

// This is called by soft_timer and executes at IRQ_PRI_PENDSV.
STATIC void mp_bluetooth_hci_soft_timer_callback(soft_timer_entry_t *self) {
    mp_bluetooth_hci_poll_now();
}

void mp_bluetooth_hci_init(void) {
    #if MICROPY_PY_BLUETOOTH_USE_SYNC_EVENTS
    events_task_is_scheduled = false;
    #endif
    soft_timer_static_init(
        &mp_bluetooth_hci_soft_timer,
        SOFT_TIMER_MODE_ONE_SHOT,
        0,
        mp_bluetooth_hci_soft_timer_callback
        );
}

STATIC void mp_bluetooth_hci_start_polling(void) {
    #if MICROPY_PY_BLUETOOTH_USE_SYNC_EVENTS
    events_task_is_scheduled = false;
    #endif
    mp_bluetooth_hci_poll_now();
}

void mp_bluetooth_hci_poll_in_ms(uint32_t ms) {
    soft_timer_reinsert(&mp_bluetooth_hci_soft_timer, ms);
}

#if MICROPY_PY_BLUETOOTH_USE_SYNC_EVENTS

// For synchronous mode, we run all BLE stack code inside a scheduled task.
// This task is scheduled periodically via a soft timer, or
// immediately on HCI UART RXIDLE.

STATIC mp_obj_t run_events_scheduled_task(mp_obj_t none_in) {
    (void)none_in;
    events_task_is_scheduled = false;
    // This will process all buffered HCI UART data, and run any callouts or events.
    mp_bluetooth_hci_poll();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(run_events_scheduled_task_obj, run_events_scheduled_task);

// Called periodically (systick) or directly (e.g. UART RX IRQ) in order to
// request that processing happens ASAP in the scheduler.
void mp_bluetooth_hci_poll_now(void) {
    if (!events_task_is_scheduled) {
        events_task_is_scheduled = mp_sched_schedule(MP_OBJ_FROM_PTR(&run_events_scheduled_task_obj), mp_const_none);
        if (!events_task_is_scheduled) {
            // The schedule queue is full, set callback to try again soon.
            mp_bluetooth_hci_poll_in_ms(5);
        }
    }
}

#else // !MICROPY_PY_BLUETOOTH_USE_SYNC_EVENTS

void mp_bluetooth_hci_poll_now(void) {
    pendsv_schedule_dispatch(PENDSV_DISPATCH_BLUETOOTH_HCI, mp_bluetooth_hci_poll);
}

#endif

#if defined(STM32WB)

/******************************************************************************/
// HCI over IPCC

#include <string.h>
#include "rfcore.h"

STATIC uint16_t hci_uart_rx_buf_cur;
STATIC uint16_t hci_uart_rx_buf_len;
STATIC uint8_t hci_uart_rx_buf_data[256];

int mp_bluetooth_hci_uart_init(uint32_t port, uint32_t baudrate) {
    (void)port;
    (void)baudrate;

    DEBUG_printf("mp_bluetooth_hci_uart_init (stm32 rfcore)\n");

    rfcore_ble_init();
    hci_uart_rx_buf_cur = 0;
    hci_uart_rx_buf_len = 0;

    // Start the HCI polling to process any initial events/packets.
    mp_bluetooth_hci_start_polling();

    return 0;
}

int mp_bluetooth_hci_uart_deinit(void) {
    DEBUG_printf("mp_bluetooth_hci_uart_deinit (stm32 rfcore)\n");

    return 0;
}

int mp_bluetooth_hci_uart_set_baudrate(uint32_t baudrate) {
    (void)baudrate;
    return 0;
}

int mp_bluetooth_hci_uart_write(const uint8_t *buf, size_t len) {
    MICROPY_PY_BLUETOOTH_ENTER
    rfcore_ble_hci_cmd(len, (const uint8_t *)buf);
    MICROPY_PY_BLUETOOTH_EXIT
    return 0;
}

// Callback to copy data into local hci_uart_rx_buf_data buffer for subsequent use.
STATIC int mp_bluetooth_hci_uart_msg_cb(void *env, const uint8_t *buf, size_t len) {
    (void)env;
    if (hci_uart_rx_buf_len + len > MP_ARRAY_SIZE(hci_uart_rx_buf_data)) {
        len = MP_ARRAY_SIZE(hci_uart_rx_buf_data) - hci_uart_rx_buf_len;
    }
    memcpy(hci_uart_rx_buf_data + hci_uart_rx_buf_len, buf, len);
    hci_uart_rx_buf_len += len;
    return 0;
}

int mp_bluetooth_hci_uart_readchar(void) {
    if (hci_uart_rx_buf_cur >= hci_uart_rx_buf_len) {
        hci_uart_rx_buf_cur = 0;
        hci_uart_rx_buf_len = 0;
        rfcore_ble_check_msg(mp_bluetooth_hci_uart_msg_cb, NULL);
    }

    if (hci_uart_rx_buf_cur < hci_uart_rx_buf_len) {
        return hci_uart_rx_buf_data[hci_uart_rx_buf_cur++];
    } else {
        return -1;
    }
}

#if MICROPY_HW_STM32WB_TRANSPARENT_MODE
#include "py/stream.h"

STATIC int rfcore_transparent_msg_cb(void *env, const uint8_t *buf, size_t len) {
    mp_hal_stdout_tx_strn((const char*)buf, len);
    return 0;
}

#define STATE_IDLE 0
#define STATE_NEED_LEN 1
#define STATE_IN_PAYLOAD 2

#define HCI_KIND_BT_CMD (0x01) // <kind=1><?><?><len>
#define HCI_KIND_BT_ACL (0x02) // <kind=2><?><?><len LSB><len MSB>
#define HCI_KIND_BT_EVENT (0x04) // <kind=4><op><len><data...>
#define HCI_KIND_VENDOR_RESPONSE (0x11)
#define HCI_KIND_VENDOR_EVENT (0x12)

STATIC mp_obj_t rfcore_transparent(void) {
    rfcore_ble_init();

    mp_hal_set_interrupt_char(-1);

    uint8_t buf[1024];
    size_t rx = 0;
    size_t len = 0;
    int state = 0;
    int cmd_type = 0;

    while (true) {
        if (state == STATE_IN_PAYLOAD && len == 0) {
            rfcore_ble_hci_cmd(rx, buf);
            // mp_hal_stdout_tx_strn((const char*)buf, rx);
            rx = 0;
            len = 0;
            state = STATE_IDLE;
        }

        if (mp_hal_stdio_poll(MP_STREAM_POLL_RD) & MP_STREAM_POLL_RD) {
            uint8_t c = mp_hal_stdin_rx_chr();

            if (state == STATE_IDLE && (c == HCI_KIND_BT_CMD || c == HCI_KIND_BT_ACL || c == HCI_KIND_BT_EVENT || c == HCI_KIND_VENDOR_RESPONSE || c == HCI_KIND_VENDOR_EVENT)) {
                cmd_type = c;
                state = STATE_NEED_LEN;
                buf[rx++] = c;
                len = 0;
            } else if (state == STATE_NEED_LEN) {
                buf[rx++] = c;
                if (cmd_type == HCI_KIND_BT_ACL && rx == 4) {
                    len = c;
                }
                if (cmd_type == HCI_KIND_BT_ACL && rx == 5) {
                    len += ((size_t)c) << 8;
                    state = STATE_IN_PAYLOAD;
                }
                if (cmd_type == HCI_KIND_BT_EVENT && rx == 3) {
                    len = c;
                    state = STATE_IN_PAYLOAD;
                }
                if (cmd_type == HCI_KIND_BT_CMD && rx == 4) {
                    len = c;
                    state = STATE_IN_PAYLOAD;
                }
            } else if (state == STATE_IN_PAYLOAD) {
                buf[rx++] = c;
                --len;
            }
        }

        rfcore_ble_check_msg(rfcore_transparent_msg_cb, NULL);
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(rfcore_transparent_obj, rfcore_transparent);
#endif // MICROPY_HW_STM32WB_TRANSPARENT_MODE


#else

/******************************************************************************/
// HCI over UART

#include "uart.h"

pyb_uart_obj_t mp_bluetooth_hci_uart_obj;
mp_irq_obj_t mp_bluetooth_hci_uart_irq_obj;

static uint8_t hci_uart_rxbuf[768];

mp_obj_t mp_uart_interrupt(mp_obj_t self_in) {
    // Queue up the scheduler to run the HCI UART and event processing ASAP.
    mp_bluetooth_hci_poll_now();

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(mp_uart_interrupt_obj, mp_uart_interrupt);

int mp_bluetooth_hci_uart_init(uint32_t port, uint32_t baudrate) {
    DEBUG_printf("mp_bluetooth_hci_uart_init (stm32)\n");

    // bits (8), stop (1), parity (none) and flow (rts/cts) are assumed to match MYNEWT_VAL_BLE_HCI_UART_ constants in syscfg.h.
    mp_bluetooth_hci_uart_obj.base.type = &pyb_uart_type;
    mp_bluetooth_hci_uart_obj.uart_id = port;
    mp_bluetooth_hci_uart_obj.is_static = true;
    // We don't want to block indefinitely, but expect flow control is doing its job.
    mp_bluetooth_hci_uart_obj.timeout = 200;
    mp_bluetooth_hci_uart_obj.timeout_char = 200;
    MP_STATE_PORT(pyb_uart_obj_all)[mp_bluetooth_hci_uart_obj.uart_id - 1] = &mp_bluetooth_hci_uart_obj;

    // Initialise the UART.
    uart_init(&mp_bluetooth_hci_uart_obj, baudrate, UART_WORDLENGTH_8B, UART_PARITY_NONE, UART_STOPBITS_1, UART_HWCONTROL_RTS | UART_HWCONTROL_CTS);
    uart_set_rxbuf(&mp_bluetooth_hci_uart_obj, sizeof(hci_uart_rxbuf), hci_uart_rxbuf);

    // Add IRQ handler for IDLE (i.e. packet finished).
    uart_irq_config(&mp_bluetooth_hci_uart_obj, false);
    mp_irq_init(&mp_bluetooth_hci_uart_irq_obj, &uart_irq_methods, MP_OBJ_FROM_PTR(&mp_bluetooth_hci_uart_obj));
    mp_bluetooth_hci_uart_obj.mp_irq_obj = &mp_bluetooth_hci_uart_irq_obj;
    mp_bluetooth_hci_uart_obj.mp_irq_trigger = UART_FLAG_IDLE;
    mp_bluetooth_hci_uart_irq_obj.handler = MP_OBJ_FROM_PTR(&mp_uart_interrupt_obj);
    mp_bluetooth_hci_uart_irq_obj.ishard = true;
    uart_irq_config(&mp_bluetooth_hci_uart_obj, true);

    // Start the HCI polling to process any initial events/packets.
    mp_bluetooth_hci_start_polling();

    return 0;
}

int mp_bluetooth_hci_uart_deinit(void) {
    DEBUG_printf("mp_bluetooth_hci_uart_deinit (stm32)\n");

    // TODO: deinit mp_bluetooth_hci_uart_obj

    return 0;
}

int mp_bluetooth_hci_uart_set_baudrate(uint32_t baudrate) {
    DEBUG_printf("mp_bluetooth_hci_uart_set_baudrate(%lu) (stm32)\n", baudrate);
    uart_set_baudrate(&mp_bluetooth_hci_uart_obj, baudrate);
    return 0;
}

int mp_bluetooth_hci_uart_write(const uint8_t *buf, size_t len) {
    // DEBUG_printf("mp_bluetooth_hci_uart_write (stm32)\n");

    mp_bluetooth_hci_controller_wakeup();
    int errcode;
    uart_tx_data(&mp_bluetooth_hci_uart_obj, (void *)buf, len, &errcode);
    if (errcode != 0) {
        mp_printf(&mp_plat_print, "\nmp_bluetooth_hci_uart_write: failed to write to UART %d\n", errcode);
    }
    return 0;
}

// This function expects the controller to be in the wake state via a previous call
// to mp_bluetooth_hci_controller_woken.
int mp_bluetooth_hci_uart_readchar(void) {
    // DEBUG_printf("mp_bluetooth_hci_uart_readchar (stm32)\n");

    if (uart_rx_any(&mp_bluetooth_hci_uart_obj)) {
        // DEBUG_printf("... available\n");
        return uart_rx_char(&mp_bluetooth_hci_uart_obj);
    } else {
        return -1;
    }
}

#endif // defined(STM32WB)

// Default (weak) implementation of the HCI controller interface.
// A driver (e.g. cywbt43.c) can override these for controller-specific
// functionality (i.e. power management).

MP_WEAK int mp_bluetooth_hci_controller_init(void) {
    DEBUG_printf("mp_bluetooth_hci_controller_init (default)\n");
    return 0;
}

MP_WEAK int mp_bluetooth_hci_controller_deinit(void) {
    DEBUG_printf("mp_bluetooth_hci_controller_deinit (default)\n");
    return 0;
}

MP_WEAK int mp_bluetooth_hci_controller_sleep_maybe(void) {
    DEBUG_printf("mp_bluetooth_hci_controller_sleep_maybe (default)\n");
    return 0;
}

MP_WEAK bool mp_bluetooth_hci_controller_woken(void) {
    DEBUG_printf("mp_bluetooth_hci_controller_woken (default)\n");
    return true;
}

MP_WEAK int mp_bluetooth_hci_controller_wakeup(void) {
    DEBUG_printf("mp_bluetooth_hci_controller_wakeup (default)\n");
    return 0;
}

#endif // MICROPY_PY_BLUETOOTH
