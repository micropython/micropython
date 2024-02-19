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

#include "py/mpconfig.h"

#if MICROPY_PY_BLUETOOTH && MICROPY_BLUETOOTH_BTSTACK && MICROPY_BLUETOOTH_BTSTACK_USB

#include <pthread.h>
#include <unistd.h>

#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mphal.h"

#include "lib/btstack/src/btstack.h"
#include "lib/btstack/src/hci_transport_usb.h"
#include "lib/btstack/platform/embedded/btstack_run_loop_embedded.h"
#include "lib/btstack/platform/embedded/hal_cpu.h"
#include "lib/btstack/platform/embedded/hal_time_ms.h"

#include "extmod/btstack/modbluetooth_btstack.h"

#include "mpbtstackport.h"

#if !MICROPY_PY_THREAD
#error Unix btstack requires MICROPY_PY_THREAD
#endif

STATIC const useconds_t USB_POLL_INTERVAL_US = 1000;

void mp_bluetooth_btstack_port_init_usb(void) {
    // MICROPYBTUSB can be a ':'' or '-' separated port list.
    char *path = getenv("MICROPYBTUSB");
    if (path != NULL) {
        uint8_t usb_path[7] = {0};
        size_t usb_path_len = 0;

        while (usb_path_len < MP_ARRAY_SIZE(usb_path)) {
            char *delimiter;
            usb_path[usb_path_len++] = strtol(path, &delimiter, 16);
            if (!delimiter || (*delimiter != ':' && *delimiter != '-')) {
                break;
            }
            path = delimiter + 1;
        }

        hci_transport_usb_set_path(usb_path_len, usb_path);
    }

    hci_init(hci_transport_usb_instance(), NULL);
}

STATIC pthread_t bstack_thread_id;

void mp_bluetooth_btstack_port_deinit(void) {
    hci_power_control(HCI_POWER_OFF);

    // Wait for the poll loop to terminate when the state is set to OFF.
    pthread_join(bstack_thread_id, NULL);
}


// Provided by mpbstackport_common.c.
extern bool mp_bluetooth_hci_poll(void);

STATIC void *btstack_thread(void *arg) {
    (void)arg;
    hci_power_control(HCI_POWER_ON);

    // modbluetooth_btstack.c will have set the state to STARTING before
    // calling mp_bluetooth_btstack_port_start.
    // This loop will terminate when the HCI_POWER_OFF above results
    // in modbluetooth_btstack.c setting the state back to OFF.
    // Or, if a timeout results in it being set to TIMEOUT.

    while (true) {
        if (!mp_bluetooth_hci_poll()) {
            break;
        }

        // The USB transport schedules events to the run loop at 1ms intervals,
        // and the implementation currently polls rather than selects.
        usleep(USB_POLL_INTERVAL_US);
    }
    return NULL;
}

void mp_bluetooth_btstack_port_start(void) {
    // Create a thread to run the btstack loop.
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&bstack_thread_id, &attr, &btstack_thread, NULL);
}

#endif // MICROPY_PY_BLUETOOTH && MICROPY_BLUETOOTH_BTSTACK && MICROPY_BLUETOOTH_BTSTACK_USB
