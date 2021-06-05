/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Damien P. George
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

#if MICROPY_PY_BLUETOOTH && MICROPY_BLUETOOTH_BTSTACK

#include "lib/btstack/src/btstack.h"
#include "lib/btstack/platform/embedded/btstack_run_loop_embedded.h"
#include "lib/btstack/platform/embedded/hal_cpu.h"
#include "lib/btstack/platform/embedded/hal_time_ms.h"

#include "extmod/mpbthci.h"
#include "extmod/btstack/btstack_hci_uart.h"
#include "extmod/btstack/modbluetooth_btstack.h"

// The IRQ functionality in btstack_run_loop_embedded.c is not used, so the
// following three functions are empty.

void hal_cpu_disable_irqs(void) {
}

void hal_cpu_enable_irqs(void) {
}

void hal_cpu_enable_irqs_and_sleep(void) {
}

uint32_t hal_time_ms(void) {
    return mp_hal_ticks_ms();
}

STATIC const hci_transport_config_uart_t hci_transport_config_uart = {
    HCI_TRANSPORT_CONFIG_UART,
    MICROPY_HW_BLE_UART_BAUDRATE,
    MICROPY_HW_BLE_UART_BAUDRATE_SECONDARY,
    0,
    NULL,
};

void mp_bluetooth_hci_poll(void) {
    if (mp_bluetooth_btstack_state == MP_BLUETOOTH_BTSTACK_STATE_OFF) {
        return;
    }

    // Process uart data.
    if (mp_bluetooth_btstack_state != MP_BLUETOOTH_BTSTACK_STATE_HALTING) {
        mp_bluetooth_btstack_hci_uart_process();
    }

    // Call the BTstack run loop.
    btstack_run_loop_embedded_execute_once();
}

void mp_bluetooth_btstack_port_init(void) {
    static bool run_loop_init = false;
    if (!run_loop_init) {
        run_loop_init = true;
        btstack_run_loop_init(btstack_run_loop_embedded_get_instance());
    } else {
        btstack_run_loop_embedded_get_instance()->init();
    }

    // hci_dump_open(NULL, HCI_DUMP_STDOUT);
    const hci_transport_t *transport = hci_transport_h4_instance(&mp_bluetooth_btstack_hci_uart_block);
    hci_init(transport, &hci_transport_config_uart);

    #ifdef MICROPY_HW_BLE_BTSTACK_CHIPSET_INSTANCE
    hci_set_chipset(MICROPY_HW_BLE_BTSTACK_CHIPSET_INSTANCE);
    #endif
}

void mp_bluetooth_btstack_port_deinit(void) {
    hci_power_control(HCI_POWER_OFF);
    hci_close();
}

void mp_bluetooth_btstack_port_start(void) {
    hci_power_control(HCI_POWER_ON);
}

#endif // MICROPY_PY_BLUETOOTH && MICROPY_BLUETOOTH_BTSTACK
