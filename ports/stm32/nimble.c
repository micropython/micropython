/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Jim Mussared
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
#include "py/mperrno.h"
#include "py/mphal.h"

#if MICROPY_PY_BLUETOOTH && MICROPY_BLUETOOTH_NIMBLE

#include "systick.h"
#include "pendsv.h"

#include "transport/uart/ble_hci_uart.h"
#include "host/ble_hs.h"

#include "extmod/modbluetooth_nimble.h"

extern void nimble_uart_process(void);
extern void os_eventq_run_all(void);
extern void os_callout_process(void);

// Hook for pendsv poller to run this periodically every 128ms
#define NIMBLE_TICK(tick) (((tick) & ~(SYSTICK_DISPATCH_NUM_SLOTS - 1) & 0x7f) == 0)

void nimble_poll(void) {
    if (mp_bluetooth_nimble_ble_state == MP_BLUETOOTH_NIMBLE_BLE_STATE_OFF) {
        return;
    }

    nimble_uart_process();
    os_callout_process();
    os_eventq_run_all();
}

void mod_bluetooth_nimble_poll_wrapper(uint32_t ticks_ms) {
    if (NIMBLE_TICK(ticks_ms)) {
        pendsv_schedule_dispatch(PENDSV_DISPATCH_NIMBLE, nimble_poll);
    }
}

void mp_bluetooth_nimble_port_preinit(void) {
    MP_STATE_PORT(bluetooth_nimble_memory) = NULL;
    ble_hci_uart_init();
}

void mp_bluetooth_nimble_port_postinit(void) {
}

void mp_bluetooth_nimble_port_deinit(void) {
    #ifdef pyb_pin_BT_REG_ON
    mp_hal_pin_low(pyb_pin_BT_REG_ON);
    #endif
}

void mp_bluetooth_nimble_port_start(void) {
    ble_hs_start();
}

#endif // MICROPY_PY_BLUETOOTH && MICROPY_BLUETOOTH_NIMBLE
