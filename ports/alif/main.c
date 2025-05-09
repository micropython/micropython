/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 OpenMV LLC.
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

#include "py/compile.h"
#include "py/runtime.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/stackctrl.h"
#include "extmod/modbluetooth.h"
#include "extmod/modnetwork.h"
#include "shared/readline/readline.h"
#include "shared/runtime/gchelper.h"
#include "shared/runtime/pyexec.h"
#include "shared/runtime/softtimer.h"
#include "shared/tinyusb/mp_usbd.h"
#include "tusb.h"
#include "mpbthciport.h"
#include "mpuart.h"
#include "ospi_flash.h"
#include "pendsv.h"
#include "se_services.h"
#include "system_tick.h"

#if MICROPY_PY_LWIP
#include "lwip/init.h"
#include "lwip/apps/mdns.h"
#endif
#if MICROPY_PY_NETWORK_CYW43
#include "lib/cyw43-driver/src/cyw43.h"
#endif

extern uint8_t __StackTop, __StackLimit;
extern uint8_t __GcHeapStart, __GcHeapEnd;

NORETURN void panic(const char *msg) {
    mp_hal_stdout_tx_strn("\nFATAL ERROR:\n", 14);
    mp_hal_stdout_tx_strn(msg, strlen(msg));
    for (;;) {
        *(volatile uint32_t *)0x4900C000 ^= 9;
        for (volatile uint delay = 0; delay < 10000000; delay++) {
        }
    }
}

int main(void) {
    system_tick_init();

    MICROPY_BOARD_STARTUP();

    pendsv_init();
    se_services_init();

    MICROPY_BOARD_EARLY_INIT();

    #if MICROPY_HW_ENABLE_UART_REPL
    mp_uart_init_repl();
    #endif
    #if MICROPY_HW_ENABLE_OSPI
    if (ospi_flash_init() != 0) {
        MICROPY_BOARD_FATAL_ERROR("ospi_init failed");
    }
    #endif
    #if MICROPY_HW_ENABLE_USBDEV
    NVIC_ClearPendingIRQ(USB_IRQ_IRQn);
    NVIC_SetPriority(USB_IRQ_IRQn, IRQ_PRI_USB);
    #endif

    // Initialise stack extents and GC heap.
    mp_stack_set_top(&__StackTop);
    mp_stack_set_limit(&__StackTop - &__StackLimit - 1024);
    gc_init(&__GcHeapStart, &__GcHeapEnd);

    #if MICROPY_PY_LWIP
    // lwIP doesn't allow to reinitialise itself by subsequent calls to this function
    // because the system timeout list (next_timeout) is only ever reset by BSS clearing.
    // So for now we only init the lwIP stack once on power-up.
    lwip_init();
    #if LWIP_MDNS_RESPONDER
    mdns_resp_init();
    #endif
    mod_network_lwip_init();
    #endif

    #if MICROPY_PY_BLUETOOTH
    mp_bluetooth_hci_init();
    #endif

    #if MICROPY_PY_NETWORK_CYW43
    {
        cyw43_init(&cyw43_state);
        uint8_t buf[8];
        memcpy(&buf[0], "ALIF", 4);
        mp_hal_get_mac_ascii(MP_HAL_MAC_WLAN0, 8, 4, (char *)&buf[4]);
        cyw43_wifi_ap_set_ssid(&cyw43_state, 8, buf);
        cyw43_wifi_ap_set_auth(&cyw43_state, CYW43_AUTH_WPA2_AES_PSK);
        cyw43_wifi_ap_set_password(&cyw43_state, 8, (const uint8_t *)"alif0123");
    }
    #endif

    for (;;) {
        // Initialise MicroPython runtime.
        mp_init();

        // Initialise sub-systems.
        readline_init0();

        // Execute _boot.py to set up the filesystem.
        pyexec_frozen_module("_boot.py", false);

        // Execute user scripts.
        int ret = pyexec_file_if_exists("boot.py");
        #if MICROPY_HW_ENABLE_USBDEV
        mp_usbd_init();
        #endif
        if (ret & PYEXEC_FORCED_EXIT) {
            goto soft_reset_exit;
        }
        if (pyexec_mode_kind == PYEXEC_MODE_FRIENDLY_REPL && ret != 0) {
            ret = pyexec_file_if_exists("main.py");
            if (ret & PYEXEC_FORCED_EXIT) {
                goto soft_reset_exit;
            }
        }

        for (;;) {
            if (pyexec_mode_kind == PYEXEC_MODE_RAW_REPL) {
                if (pyexec_raw_repl() != 0) {
                    break;
                }
            } else {
                if (pyexec_friendly_repl() != 0) {
                    break;
                }
            }
        }

    soft_reset_exit:
        mp_printf(MP_PYTHON_PRINTER, "MPY: soft reboot\n");
        #if MICROPY_PY_BLUETOOTH
        mp_bluetooth_deinit();
        #endif
        soft_timer_deinit();
        gc_sweep_all();
        mp_deinit();
    }
}

void gc_collect(void) {
    gc_collect_start();
    gc_helper_collect_regs_and_stack();
    gc_collect_end();
}

void nlr_jump_fail(void *val) {
    mp_printf(&mp_plat_print, "FATAL: uncaught exception %p\n", val);
    mp_obj_print_exception(&mp_plat_print, MP_OBJ_FROM_PTR(val));
    for (;;) {
        __WFE();
    }
}

#ifndef NDEBUG
void MP_WEAK __assert_func(const char *file, int line, const char *func, const char *expr) {
    printf("Assertion '%s' failed, at file %s:%d\n", expr, file, line);
    panic("Assertion failed");
}
#endif
