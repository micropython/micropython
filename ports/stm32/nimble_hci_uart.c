/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018-2019 Damien P. George
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
#include "uart.h"
#include "pendsv.h"
#include "drivers/cyw43/cywbt.h"

#if MICROPY_BLUETOOTH_NIMBLE

/******************************************************************************/
// UART
pyb_uart_obj_t bt_hci_uart_obj;
static uint8_t hci_uart_rxbuf[512];

extern void nimble_poll(void);

mp_obj_t mp_uart_interrupt(mp_obj_t self_in) {
    pendsv_schedule_dispatch(PENDSV_DISPATCH_NIMBLE, nimble_poll);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(mp_uart_interrupt_obj, mp_uart_interrupt);

int nimble_hci_uart_set_baudrate(uint32_t baudrate) {
    uart_init(&bt_hci_uart_obj, baudrate, UART_WORDLENGTH_8B, UART_PARITY_NONE, UART_STOPBITS_1, UART_HWCONTROL_RTS | UART_HWCONTROL_CTS);
    uart_set_rxbuf(&bt_hci_uart_obj, sizeof(hci_uart_rxbuf), hci_uart_rxbuf);
    return 0;
}

int nimble_hci_uart_configure(uint32_t port) {
    // bits (8), stop (1), parity (none) and flow (rts/cts) are assumed to match MYNEWT_VAL_BLE_HCI_UART_ constants in syscfg.h.
    bt_hci_uart_obj.base.type = &pyb_uart_type;
    bt_hci_uart_obj.uart_id = port;
    bt_hci_uart_obj.is_static = true;
    bt_hci_uart_obj.timeout = 2;
    bt_hci_uart_obj.timeout_char = 2;
    MP_STATE_PORT(pyb_uart_obj_all)[bt_hci_uart_obj.uart_id - 1] = &bt_hci_uart_obj;
    return 0;
}

int nimble_hci_uart_activate(void) {
    // Interrupt on RX chunk received (idle)
    // Trigger nimble poll when this happens
    mp_obj_t uart_irq_fn = mp_load_attr(&bt_hci_uart_obj, MP_QSTR_irq);
    mp_obj_t uargs[] = {
        MP_OBJ_FROM_PTR(&mp_uart_interrupt_obj),
        MP_OBJ_NEW_SMALL_INT(UART_FLAG_IDLE),
        mp_const_true,
    };
    mp_call_function_n_kw(uart_irq_fn, 3, 0, uargs);

    #if MICROPY_PY_NETWORK_CYW43
    cywbt_init();
    cywbt_activate();
    #endif

    return 0;
}

mp_uint_t nimble_hci_uart_rx_any() {
    return uart_rx_any(&bt_hci_uart_obj);
}

int nimble_hci_uart_rx_char() {
    return uart_rx_char(&bt_hci_uart_obj);
}

void nimble_hci_uart_tx_strn(const char *str, uint len) {
    uart_tx_strn(&bt_hci_uart_obj, str, len);
}

#endif // MICROPY_BLUETOOTH_NIMBLE
