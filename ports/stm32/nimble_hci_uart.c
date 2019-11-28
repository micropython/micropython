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
#include "extmod/nimble/nimble/hci_uart.h"

#if MICROPY_BLUETOOTH_NIMBLE

#if defined(STM32WB)

/******************************************************************************/
// HCI over IPCC

#include "rfcore.h"

int nimble_hci_uart_configure(uint32_t port) {
    (void)port;
    return 0;
}

int nimble_hci_uart_set_baudrate(uint32_t baudrate) {
    (void)baudrate;
    return 0;
}

int nimble_hci_uart_activate(void) {
    rfcore_ble_init();
    return 0;
}

void nimble_hci_uart_rx(hal_uart_rx_cb_t rx_cb, void *rx_arg) {
    // Protect in case it's called from ble_npl_sem_pend at thread-level
    MICROPY_PY_LWIP_ENTER
    rfcore_ble_check_msg(rx_cb, rx_arg);
    MICROPY_PY_LWIP_EXIT
}

void nimble_hci_uart_tx_strn(const char *str, uint len) {
    MICROPY_PY_LWIP_ENTER
    rfcore_ble_hci_cmd(len, (const uint8_t*)str);
    MICROPY_PY_LWIP_EXIT
}

#else

/******************************************************************************/
// HCI over UART

#include "pendsv.h"
#include "uart.h"
#include "drivers/cyw43/cywbt.h"

pyb_uart_obj_t bt_hci_uart_obj;
static uint8_t hci_uart_rxbuf[512];

#ifdef pyb_pin_BT_DEV_WAKE
static uint32_t bt_sleep_ticks;
#endif

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

void nimble_hci_uart_rx(hal_uart_rx_cb_t rx_cb, void *rx_arg) {
    #ifdef pyb_pin_BT_HOST_WAKE
    int host_wake = 0;
    host_wake = mp_hal_pin_read(pyb_pin_BT_HOST_WAKE);
    /*
    // this is just for info/tracing purposes
    static int last_host_wake = 0;
    if (host_wake != last_host_wake) {
        printf("HOST_WAKE change %d -> %d\n", last_host_wake, host_wake);
        last_host_wake = host_wake;
    }
    */
    #endif

    while (uart_rx_any(&bt_hci_uart_obj)) {
        uint8_t data = uart_rx_char(&bt_hci_uart_obj);
        //printf("UART RX: %02x\n", data);
        rx_cb(rx_arg, data);
    }

    #ifdef pyb_pin_BT_DEV_WAKE
    if (host_wake == 1 && mp_hal_pin_read(pyb_pin_BT_DEV_WAKE) == 0) {
        if (mp_hal_ticks_ms() - bt_sleep_ticks > 500) {
            //printf("BT SLEEP\n");
            mp_hal_pin_high(pyb_pin_BT_DEV_WAKE); // let sleep
        }
    }
    #endif
}

void nimble_hci_uart_tx_strn(const char *str, uint len) {
    #ifdef pyb_pin_BT_DEV_WAKE
    bt_sleep_ticks = mp_hal_ticks_ms();

    if (mp_hal_pin_read(pyb_pin_BT_DEV_WAKE) == 1) {
        //printf("BT WAKE for TX\n");
        mp_hal_pin_low(pyb_pin_BT_DEV_WAKE); // wake up
        // Use delay_us rather than delay_ms to prevent running the scheduler (which
        // might result in more BLE operations).
        mp_hal_delay_us(5000); // can't go lower than this
    }
    #endif

    uart_tx_strn(&bt_hci_uart_obj, str, len);
}

#endif // defined(STM32WB)

#endif // MICROPY_BLUETOOTH_NIMBLE
