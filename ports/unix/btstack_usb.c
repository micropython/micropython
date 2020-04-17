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

#include <pthread.h>
#include <unistd.h>

#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mphal.h"

#if MICROPY_PY_BLUETOOTH && MICROPY_BLUETOOTH_BTSTACK

#include "lib/btstack/src/btstack.h"
#include "lib/btstack/platform/embedded/btstack_run_loop_embedded.h"
#include "lib/btstack/platform/embedded/hal_cpu.h"
#include "lib/btstack/platform/embedded/hal_time_ms.h"

#include "extmod/btstack/modbluetooth_btstack.h"

#if !MICROPY_PY_THREAD
#error Unix btstack requires MICROPY_PY_THREAD
#endif

STATIC const useconds_t USB_POLL_INTERVAL_US = 1000;

STATIC const uint8_t read_static_address_command_complete_prefix[] = { 0x0e, 0x1b, 0x01, 0x09, 0xfc };

STATIC uint8_t local_addr[6] = {0};
STATIC uint8_t static_address[6] = {0};
STATIC volatile bool have_addr = false;
STATIC bool using_static_address = false;

STATIC btstack_packet_callback_registration_t hci_event_callback_registration;

STATIC void packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size) {
    (void)channel;
    (void)size;
    if (packet_type != HCI_EVENT_PACKET) {
        return;
    }
    switch (hci_event_packet_get_type(packet)) {
        case BTSTACK_EVENT_STATE:
            if (btstack_event_state_get_state(packet) != HCI_STATE_WORKING) {
                return;
            }
            gap_local_bd_addr(local_addr);
            if (using_static_address) {
                memcpy(local_addr, static_address, sizeof(local_addr));
            }
            have_addr = true;
            break;
        case HCI_EVENT_COMMAND_COMPLETE:
            if (memcmp(packet, read_static_address_command_complete_prefix, sizeof(read_static_address_command_complete_prefix)) == 0) {
                reverse_48(&packet[7], static_address);
                gap_random_address_set(static_address);
                using_static_address = true;
                have_addr = true;
            }
            break;
        default:
            break;
    }
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
    static bool run_loop_init = false;
    if (!run_loop_init) {
        run_loop_init = true;
        btstack_run_loop_init(btstack_run_loop_embedded_get_instance());
    } else {
        btstack_run_loop_embedded_get_instance()->init();
    }

    // TODO: allow setting USB device path via cmdline/env var.

    // hci_dump_open(NULL, HCI_DUMP_STDOUT);
    hci_init(hci_transport_usb_instance(), NULL);

    hci_event_callback_registration.callback = &packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);
}

STATIC pthread_t bstack_thread_id;

void mp_bluetooth_btstack_port_deinit(void) {
    hci_power_control(HCI_POWER_OFF);

    // Wait for the poll loop to terminate when the state is set to OFF.
    pthread_join(bstack_thread_id, NULL);
    have_addr = false;
}

STATIC void *btstack_thread(void *arg) {
    (void)arg;
    hci_power_control(HCI_POWER_ON);

    // modbluetooth_btstack.c will have set the state to STARTING before
    // calling mp_bluetooth_btstack_port_start.
    // This loop will terminate when the HCI_POWER_OFF above results
    // in modbluetooth_btstack.c setting the state back to OFF.
    // Or, if a timeout results in it being set to TIMEOUT.

    while (mp_bluetooth_btstack_state == MP_BLUETOOTH_BTSTACK_STATE_STARTING || mp_bluetooth_btstack_state == MP_BLUETOOTH_BTSTACK_STATE_ACTIVE) {
        btstack_run_loop_embedded_execute_once();

        // The USB transport schedules events to the run loop at 1ms intervals,
        // and the implementation currently polls rather than selects.
        usleep(USB_POLL_INTERVAL_US);
    }

    hci_close();

    return NULL;
}

void mp_bluetooth_btstack_port_start(void) {
    // Create a thread to run the btstack loop.
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&bstack_thread_id, &attr, &btstack_thread, NULL);
}

void mp_hal_get_mac(int idx, uint8_t buf[6]) {
    if (idx == MP_HAL_MAC_BDADDR) {
        if (!have_addr) {
            mp_raise_OSError(MP_ENODEV);
        }
        memcpy(buf, local_addr, sizeof(local_addr));
    }
}

#endif // MICROPY_PY_BLUETOOTH && MICROPY_BLUETOOTH_BTSTACK
