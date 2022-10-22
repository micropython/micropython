/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Jim Mussared
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

#include "extmod/btstack/modbluetooth_btstack.h"

#include "mpbtstackport.h"

// Called by the UART polling thread in mpbthciport.c, or by the USB polling thread in mpbtstackport_usb.c.
bool mp_bluetooth_hci_poll(void) {
    if (mp_bluetooth_btstack_state == MP_BLUETOOTH_BTSTACK_STATE_STARTING || mp_bluetooth_btstack_state == MP_BLUETOOTH_BTSTACK_STATE_ACTIVE || mp_bluetooth_btstack_state == MP_BLUETOOTH_BTSTACK_STATE_HALTING) {
        // Pretend like we're running in IRQ context (i.e. other things can't be running at the same time).
        mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
        #if MICROPY_BLUETOOTH_BTSTACK_H4
        mp_bluetooth_hci_poll_h4();
        #endif
        btstack_run_loop_embedded_execute_once();
        MICROPY_END_ATOMIC_SECTION(atomic_state);

        return true;
    }

    return false;
}

bool mp_bluetooth_hci_active(void) {
    return mp_bluetooth_btstack_state != MP_BLUETOOTH_BTSTACK_STATE_OFF
           && mp_bluetooth_btstack_state != MP_BLUETOOTH_BTSTACK_STATE_TIMEOUT;
}

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

void mp_bluetooth_btstack_port_init(void) {
    btstack_run_loop_init(btstack_run_loop_embedded_get_instance());

    // hci_dump_open(NULL, HCI_DUMP_STDOUT);

    #if MICROPY_BLUETOOTH_BTSTACK_H4
    mp_bluetooth_btstack_port_init_h4();
    #endif

    #if MICROPY_BLUETOOTH_BTSTACK_USB
    mp_bluetooth_btstack_port_init_usb();
    #endif
}

#endif // MICROPY_PY_BLUETOOTH && MICROPY_BLUETOOTH_BTSTACK
