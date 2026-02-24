/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Jim Mussared
 * Copyright (c) 2020 Damien P. George
 * Copyright (c) 2022 Andrew Leech
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
#include "led.h"

#if MICROPY_PY_BLUETOOTH && MICROPY_BLUETOOTH_NIMBLE

#define DEBUG_printf(...) // mp_printf(&mp_plat_print, "mpnimbleport.c: " __VA_ARGS__)

#include "mpbthciport.h"
#include "drivers/rng.h"
#include "nimble/nimble_npl.h"
#include "controller/ble_ll.h"
#include "controller/ble_ll_hci.h"
#include "nimble/controller/src/ble_ll_conn_priv.h"
#include "controller/ble_phy.h"
#include "nimble/ble_hci_trans.h"

#include "extmod/nimble/modbluetooth_nimble.h"

void mp_bluetooth_hci_controller_init(void) {
    DEBUG_printf("mp_bluetooth_hci_controller_init\n");

    // Init ble phy
    // ref: nimble/controller/src/ble_ll.c:ble_ll_task()
    ble_phy_init();

    // Set output power to 1mW (0 dBm)
    ble_phy_txpwr_set(MYNEWT_VAL(BLE_LL_TX_PWR_DBM));

    // Register callback for transport
    ble_hci_trans_cfg_ll(ble_ll_hci_cmd_rx, NULL, ble_ll_hci_acl_rx, NULL);

    // Tell the host that we are ready to receive packets
    ble_ll_hci_send_noop();

    // Start polling for date.
    mp_bluetooth_hci_poll_now();
}

void mp_bluetooth_hci_controller_deinit(void) {
    mp_bluetooth_hci_deinit();

}

// The global BLE controller LL data object
extern struct ble_ll_obj g_ble_ll_data;

static void run_controller_events() {
    struct ble_npl_event *ev = ble_npl_eventq_get(&g_ble_ll_data.ll_evq, 0);
    if (ev != NULL) {
        ble_npl_event_run(ev);
    }
}

// Get any pending data from the UART and send it to NimBLE's HCI buffers.
// Any further processing by NimBLE will be run via its event queue.
void mp_bluetooth_hci_poll(void) {
    if (mp_bluetooth_nimble_ble_state >= MP_BLUETOOTH_NIMBLE_BLE_STATE_WAITING_FOR_SYNC) {
        // DEBUG_printf("mp_bluetooth_hci_poll_uart %d\n", mp_bluetooth_nimble_ble_state);

        // Handle any controller events first
        run_controller_events();

        // Run any timers.
        mp_bluetooth_nimble_os_callout_process();

        // Run any remaining events.
        mp_bluetooth_nimble_os_eventq_run_all();
    }
    if (mp_bluetooth_nimble_ble_state != MP_BLUETOOTH_NIMBLE_BLE_STATE_OFF) {
        // Call this function again in 128ms to check for new events.
        // TODO: improve this by only calling back when needed.
        mp_bluetooth_hci_poll_in_ms(128);
    }
}

// --- Port-specific helpers for the generic NimBLE bindings. -----------------

void mp_bluetooth_nimble_hci_uart_wfi(void) {
    #if defined(__WFI)
    __WFI();
    #endif

    run_controller_events();
}

long jrand48(unsigned short xsubi[3]) {
    ble_ll_rand_data_get((uint8_t *)xsubi, 4);
    return *((long *)xsubi);
}


// The interrupt table (VTOR) on nrf port is not in sram so NVIC_SetVector() doesn't work.
// Capture the required interrupt handler callbacks used in the nimble controller here to run in
// the statically defined interrupt handler functions.

// ref: ports/nrf/drivers/rng.c
extern func nrf_ble_isr_rng;

static func nrf_ble_isr_phy = NULL;
static func nrf_ble_isr_rtc0 = NULL;

void RADIO_IRQHandler(void) {
    if (nrf_ble_isr_phy != NULL) {
        nrf_ble_isr_phy();
        mp_bluetooth_hci_poll_in_ms(5);
    }
}

void RTC0_IRQHandler(void) {
    if (nrf_ble_isr_rtc0 != NULL) {
        nrf_ble_isr_rtc0();
    }
}

void ble_npl_hw_set_isr(int irq_num, func irq_isr) {
    switch (irq_num) {
        case RNG_IRQn:
            nrf_ble_isr_rng = irq_isr;
            break;

        case RADIO_IRQn:
            nrf_ble_isr_phy = irq_isr;
            break;

        case RTC0_IRQn:
            nrf_ble_isr_rtc0 = irq_isr;
            break;

        default:
            // Unknown ISR
            assert(0);
    }
}

#endif // MICROPY_PY_BLUETOOTH && MICROPY_BLUETOOTH_NIMBLE
