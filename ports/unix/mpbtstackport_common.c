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
#include "lib/btstack/src/ble/le_device_db_tlv.h"
#include "lib/btstack/src/classic/btstack_link_key_db_tlv.h"
#include "lib/btstack/platform/embedded/btstack_run_loop_embedded.h"
#include "lib/btstack/platform/embedded/hal_cpu.h"
#include "lib/btstack/platform/embedded/hal_time_ms.h"

#include "extmod/btstack/modbluetooth_btstack.h"
#include "extmod/btstack/btstack_tlv_mpy.h"

#include "mpbtstackport.h"

STATIC uint8_t local_addr[6] = {0};
STATIC uint8_t static_address[6] = {0};
STATIC volatile bool have_addr = false;
STATIC bool using_static_address = false;

#define TLV_DB_PATH_PREFIX "/tmp/btstack_"
#define TLV_DB_PATH_POSTFIX ".tlv"
static char tlv_db_path[100];
static const btstack_tlv_t * tlv_impl;
static btstack_tlv_mpy_t   tlv_context;

STATIC btstack_packet_callback_registration_t hci_event_callback_registration;

// Called by the UART polling thread in mphciport.c, or by the USB polling thread in mpbtstackport_usb.c.
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

            printf("BTstack up and running on %s.\n", bd_addr_to_str(local_addr));
            strcpy(tlv_db_path, TLV_DB_PATH_PREFIX);
            strcat(tlv_db_path, bd_addr_to_str(local_addr));
            strcat(tlv_db_path, TLV_DB_PATH_POSTFIX);
            tlv_impl = btstack_tlv_mpy_init_instance(&tlv_context, tlv_db_path);
            btstack_tlv_set_instance(tlv_impl, &tlv_context);
#ifdef ENABLE_CLASSIC
            hci_set_link_key_db(btstack_link_key_db_tlv_get_instance(tlv_impl, &tlv_context));
#endif
#ifdef ENABLE_BLE
            le_device_db_tlv_configure(tlv_impl, &tlv_context);
#endif

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

    // hci_dump_open(NULL, HCI_DUMP_STDOUT);

    #if MICROPY_BLUETOOTH_BTSTACK_H4
    mp_bluetooth_btstack_port_init_h4();
    #endif

    #if MICROPY_BLUETOOTH_BTSTACK_USB
    mp_bluetooth_btstack_port_init_usb();
    #endif
}

#endif // MICROPY_PY_BLUETOOTH && MICROPY_BLUETOOTH_BTSTACK
