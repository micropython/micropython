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
#include "py/stackctrl.h"

#include "lib/btstack/src/btstack.h"
#include "lib/btstack/src/hci_transport_usb.h"
#include "lib/btstack/platform/posix/btstack_run_loop_posix.h"
#include "lib/btstack/platform/posix/hci_dump_posix_fs.h"

#include "extmod/btstack/modbluetooth_btstack.h"

#include "mpbtstackport.h"

#if !MICROPY_PY_THREAD
#error Unix btstack requires MICROPY_PY_THREAD
#endif

#define DEBUG_printf(...) // printf("mpbtstackport_usb.c: " __VA_ARGS__)

void mp_bluetooth_btstack_port_init_usb(void) {

    #if MICROPY_BLUETOOTH_BTSTACK_HCI_DUMP
    // log into file using HCI_DUMP_PACKETLOGGER format
    const char *pklg_path = "/tmp/hci_dump.pklg";
    hci_dump_posix_fs_open(pklg_path, HCI_DUMP_PACKETLOGGER);
    const hci_dump_t *hci_dump_impl = hci_dump_posix_fs_get_instance();
    hci_dump_init(hci_dump_impl);
    printf("Packet Log: %s\n", pklg_path);
    #endif

    btstack_run_loop_init(btstack_run_loop_posix_get_instance());

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

static pthread_t bstack_thread_id;

void mp_bluetooth_btstack_port_deinit(void) {
    DEBUG_printf("mp_bluetooth_btstack_port_deinit: begin: mp_bluetooth_btstack_state=%d\n", mp_bluetooth_btstack_state);

    // Workaround:
    //
    // Possibly hci_power_control(HCI_POWER_OFF);
    // crashes with memory fault if state is already MP_BLUETOOTH_BTSTACK_STATE_OFF.
    if (mp_bluetooth_btstack_state != MP_BLUETOOTH_BTSTACK_STATE_OFF &&
        mp_bluetooth_btstack_state != MP_BLUETOOTH_BTSTACK_STATE_TIMEOUT) {

        hci_power_control(HCI_POWER_OFF);

        DEBUG_printf("mp_bluetooth_btstack_port_deinit: after HCI_POWER_OFF: mp_bluetooth_btstack_state=%d\n", mp_bluetooth_btstack_state);
    }

    // btstack_run_loop_trigger_exit() will call btstack_run_loop_t::trigger_exit()
    // i.e. btstack_run_loop_posix_trigger_exit().
    // And that will trigger the exit of btstack_run_loop_execute()
    // i.e. btstack_run_loop_posix_execute().
    btstack_run_loop_trigger_exit();

    // A call of btstack_run_loop_poll_data_sources_from_irq() is needed
    // additional to btstack_run_loop_trigger_exit() to exit the posix run loop.
    //
    // Otherwise the posix run loop will wait forever for ready File Descriptors.
    //
    // Hint:
    // btstack_run_loop_poll_data_sources_from_irq()
    // calls btstack_run_loop_posix_poll_data_sources_from_irq()
    btstack_run_loop_poll_data_sources_from_irq();

    // MicroPython threads are created with PTHREAD_CREATE_DETACHED.
    //
    // Nonetheless the thread is created with PTHREAD_CREATE_JOINABLE and
    // pthread_join() is used.

    DEBUG_printf("mp_bluetooth_btstack_port_deinit: pthread_join()\n");
    pthread_join(bstack_thread_id, NULL);

    #if MICROPY_BLUETOOTH_BTSTACK_HCI_DUMP
    DEBUG_printf("mp_bluetooth_btstack_port_deinit: hci_dump_posix_fs_close()\n");
    hci_dump_posix_fs_close();
    #endif

    DEBUG_printf("mp_bluetooth_btstack_port_deinit: end\n");
}

static void *btstack_thread(void *arg) {
    (void)arg;

    // This code runs on an non-MicroPython thread.
    // But some of the code e.g. in "extmod/btstack/modbluetooth_btstack.c"
    // make calls that needs the state of a MicroPython thread e. g. "m_del()".
    //
    // So set up relevant MicroPython state and obtain the GIL,
    // to synchronised with the rest of the runtime.

    mp_state_thread_t ts;
    mp_thread_init_state(&ts, 40000 * (sizeof(void *) / 4) - 1024, NULL, NULL);
    MP_THREAD_GIL_ENTER();

    log_info("btstack_thread: HCI_POWER_ON");
    hci_power_control(HCI_POWER_ON);

    btstack_run_loop_execute();

    log_info("btstack_thread: end");

    // Give back the GIL and reset the MicroPython state.
    MP_THREAD_GIL_EXIT();
    mp_thread_set_state(NULL);

    return NULL;
}

void mp_bluetooth_btstack_port_start(void) {
    // Create a thread to run the btstack loop.
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    // We need a joinable thread for pthread_join() what is the default.
    pthread_create(&bstack_thread_id, &attr, &btstack_thread, NULL);
}

#endif // MICROPY_PY_BLUETOOTH && MICROPY_BLUETOOTH_BTSTACK && MICROPY_BLUETOOTH_BTSTACK_USB
