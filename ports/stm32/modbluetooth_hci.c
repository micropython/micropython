/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018-2020 Damien P. George
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
#include "extmod/modbluetooth_hci.h"
#include "systick.h"
#include "pendsv.h"

#include "py/obj.h"

#if MICROPY_PY_BLUETOOTH

uint8_t mp_bluetooth_hci_cmd_buf[4 + 256];

// Must be provided by the stack bindings.
extern void mp_bluetooth_hci_poll(void);

// Hook for pendsv poller to run this periodically every 128ms
#define BLUETOOTH_HCI_TICK(tick) (((tick) & ~(SYSTICK_DISPATCH_NUM_SLOTS - 1) & 0x7f) == 0)

// Called periodically (systick) or directly (e.g. uart irq).
void mp_bluetooth_hci_poll_wrapper(uint32_t ticks_ms) {
    if (ticks_ms == 0 || BLUETOOTH_HCI_TICK(ticks_ms)) {
        pendsv_schedule_dispatch(PENDSV_DISPATCH_BLUETOOTH_HCI, mp_bluetooth_hci_poll);
    }
}

#if defined(STM32WB)

/******************************************************************************/
// HCI over IPCC

#include <string.h>
#include "rfcore.h"

STATIC uint16_t hci_uart_rx_buf_cur;
STATIC uint16_t hci_uart_rx_buf_len;
STATIC uint8_t hci_uart_rx_buf_data[256];

int mp_bluetooth_hci_controller_deactivate(void) {
    return 0;
}

int mp_bluetooth_hci_controller_sleep_maybe(void) {
    return 0;
}

bool mp_bluetooth_hci_controller_woken(void) {
    return true;
}

int mp_bluetooth_hci_controller_wakeup(void) {
    return 0;
}

int mp_bluetooth_hci_uart_init(uint32_t port) {
    (void)port;
    return 0;
}

int mp_bluetooth_hci_uart_activate(void) {
    rfcore_ble_init();
    hci_uart_rx_buf_cur = 0;
    hci_uart_rx_buf_len = 0;
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

#else

/******************************************************************************/
// HCI over UART

#include "pendsv.h"
#include "uart.h"

pyb_uart_obj_t mp_bluetooth_hci_uart_obj;

static uint8_t hci_uart_rxbuf[512];

mp_obj_t mp_uart_interrupt(mp_obj_t self_in) {
    // New HCI data, schedule mp_bluetooth_hci_poll to make the stack handle it.
    mp_bluetooth_hci_poll_wrapper(0);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(mp_uart_interrupt_obj, mp_uart_interrupt);

int mp_bluetooth_hci_uart_init(uint32_t port) {
    // bits (8), stop (1), parity (none) and flow (rts/cts) are assumed to match MYNEWT_VAL_BLE_HCI_UART_ constants in syscfg.h.
    mp_bluetooth_hci_uart_obj.base.type = &pyb_uart_type;
    mp_bluetooth_hci_uart_obj.uart_id = port;
    mp_bluetooth_hci_uart_obj.is_static = true;
    mp_bluetooth_hci_uart_obj.timeout = 2;
    mp_bluetooth_hci_uart_obj.timeout_char = 2;
    MP_STATE_PORT(pyb_uart_obj_all)[mp_bluetooth_hci_uart_obj.uart_id - 1] = &mp_bluetooth_hci_uart_obj;
    return 0;
}

int mp_bluetooth_hci_uart_set_baudrate(uint32_t baudrate) {
    uart_init(&mp_bluetooth_hci_uart_obj, baudrate, UART_WORDLENGTH_8B, UART_PARITY_NONE, UART_STOPBITS_1, UART_HWCONTROL_RTS | UART_HWCONTROL_CTS);
    uart_set_rxbuf(&mp_bluetooth_hci_uart_obj, sizeof(hci_uart_rxbuf), hci_uart_rxbuf);
    return 0;
}

int mp_bluetooth_hci_uart_activate(void) {
    // Interrupt on RX chunk received (idle)
    // Trigger stack poll when this happens
    mp_obj_t uart_irq_fn = mp_load_attr(MP_OBJ_FROM_PTR(&mp_bluetooth_hci_uart_obj), MP_QSTR_irq);
    mp_obj_t uargs[] = {
        MP_OBJ_FROM_PTR(&mp_uart_interrupt_obj),
        MP_OBJ_NEW_SMALL_INT(UART_FLAG_IDLE),
        mp_const_true,
    };
    mp_call_function_n_kw(uart_irq_fn, 3, 0, uargs);

    mp_bluetooth_hci_controller_init();
    mp_bluetooth_hci_controller_activate();

    return 0;
}

int mp_bluetooth_hci_uart_write(const uint8_t *buf, size_t len) {
    mp_bluetooth_hci_controller_wakeup();
    uart_tx_strn(&mp_bluetooth_hci_uart_obj, (void *)buf, len);
    return 0;
}

// This function expects the controller to be in the wake state via a previous call
// to mp_bluetooth_hci_controller_woken.
int mp_bluetooth_hci_uart_readchar(void) {
    if (uart_rx_any(&mp_bluetooth_hci_uart_obj)) {
        return uart_rx_char(&mp_bluetooth_hci_uart_obj);
    } else {
        return -1;
    }
}

#endif // defined(STM32WB)

#endif // MICROPY_PY_BLUETOOTH
