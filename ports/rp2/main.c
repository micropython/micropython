/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
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

#include <stdio.h>

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
#include "uart.h"
#include "modmachine.h"
#include "modrp2.h"
#include "mpbthciport.h"
#include "mpnetworkport.h"
#include "genhdr/mpversion.h"
#include "mp_usbd.h"

#include "RP2040.h" // cmsis, for PendSV_IRQn and SCB/SCB_SCR_SEVONPEND_Msk
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "pico/unique_id.h"
#include "hardware/rtc.h"
#include "hardware/structs/rosc.h"
#if MICROPY_PY_LWIP
#include "lwip/init.h"
#include "lwip/apps/mdns.h"
#endif
#if MICROPY_PY_NETWORK_CYW43
#include "lib/cyw43-driver/src/cyw43.h"
#endif

extern uint8_t __StackTop, __StackBottom;
extern uint8_t __GcHeapStart, __GcHeapEnd;

// Embed version info in the binary in machine readable form
bi_decl(bi_program_version_string(MICROPY_GIT_TAG));

// Add a section to the picotool output similar to program features, but for frozen modules
// (it will aggregate BINARY_INFO_ID_MP_FROZEN binary info)
bi_decl(bi_program_feature_group_with_flags(BINARY_INFO_TAG_MICROPYTHON,
    BINARY_INFO_ID_MP_FROZEN, "frozen modules",
    BI_NAMED_GROUP_SEPARATE_COMMAS | BI_NAMED_GROUP_SORT_ALPHA));

int main(int argc, char **argv) {
    // This is a tickless port, interrupts should always trigger SEV.
    SCB->SCR |= SCB_SCR_SEVONPEND_Msk;

    pendsv_init();
    soft_timer_init();

    #if MICROPY_HW_ENABLE_UART_REPL
    bi_decl(bi_program_feature("UART REPL"))
    setup_default_uart();
    mp_uart_init();
    #else
    #ifndef NDEBUG
    stdio_init_all();
    #endif
    #endif

    #if MICROPY_HW_ENABLE_USBDEV && MICROPY_HW_USB_CDC
    bi_decl(bi_program_feature("USB REPL"))
    #endif

    #if MICROPY_PY_THREAD
    bi_decl(bi_program_feature("thread support"))
    mp_thread_init();
    #endif

    // Start and initialise the RTC
    datetime_t t = {
        .year = 2021,
        .month = 1,
        .day = 1,
        .dotw = 4, // 0 is Monday, so 4 is Friday
        .hour = 0,
        .min = 0,
        .sec = 0,
    };
    rtc_init();
    rtc_set_datetime(&t);
    mp_hal_time_ns_set_from_rtc();

    // Initialise stack extents and GC heap.
    mp_stack_set_top(&__StackTop);
    mp_stack_set_limit(&__StackTop - &__StackBottom - 256);
    gc_init(&__GcHeapStart, &__GcHeapEnd);

    #if MICROPY_PY_LWIP
    // lwIP doesn't allow to reinitialise itself by subsequent calls to this function
    // because the system timeout list (next_timeout) is only ever reset by BSS clearing.
    // So for now we only init the lwIP stack once on power-up.
    lwip_init();
    #if LWIP_MDNS_RESPONDER
    mdns_resp_init();
    #endif
    #endif

    #if MICROPY_PY_NETWORK_CYW43 || MICROPY_PY_BLUETOOTH_CYW43
    {
        cyw43_init(&cyw43_state);
        cyw43_irq_init();
        cyw43_post_poll_hook(); // enable the irq
        uint8_t buf[8];
        memcpy(&buf[0], "PICO", 4);

        // MAC isn't loaded from OTP yet, so use unique id to generate the default AP ssid.
        const char hexchr[16] = "0123456789ABCDEF";
        pico_unique_board_id_t pid;
        pico_get_unique_board_id(&pid);
        buf[4] = hexchr[pid.id[7] >> 4];
        buf[5] = hexchr[pid.id[6] & 0xf];
        buf[6] = hexchr[pid.id[5] >> 4];
        buf[7] = hexchr[pid.id[4] & 0xf];
        cyw43_wifi_ap_set_ssid(&cyw43_state, 8, buf);
        cyw43_wifi_ap_set_auth(&cyw43_state, CYW43_AUTH_WPA2_AES_PSK);
        cyw43_wifi_ap_set_password(&cyw43_state, 8, (const uint8_t *)"picoW123");
    }
    #endif

    for (;;) {

        // Initialise MicroPython runtime.
        mp_init();
        mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_lib));

        // Initialise sub-systems.
        readline_init0();
        machine_pin_init();
        rp2_pio_init();
        rp2_dma_init();
        machine_i2s_init0();

        #if MICROPY_PY_BLUETOOTH
        mp_bluetooth_hci_init();
        #endif
        #if MICROPY_PY_NETWORK
        mod_network_init();
        #endif
        #if MICROPY_PY_LWIP
        mod_network_lwip_init();
        #endif

        // Execute _boot.py to set up the filesystem.
        #if MICROPY_VFS_FAT && MICROPY_HW_USB_MSC
        pyexec_frozen_module("_boot_fat.py", false);
        #else
        pyexec_frozen_module("_boot.py", false);
        #endif

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
        #if MICROPY_PY_NETWORK
        mod_network_deinit();
        #endif
        rp2_dma_deinit();
        rp2_pio_deinit();
        #if MICROPY_PY_BLUETOOTH
        mp_bluetooth_deinit();
        #endif
        machine_pwm_deinit_all();
        machine_pin_deinit();
        #if MICROPY_PY_THREAD
        mp_thread_deinit();
        #endif
        soft_timer_deinit();
        #if MICROPY_HW_ENABLE_USB_RUNTIME_DEVICE
        mp_usbd_deinit();
        #endif

        gc_sweep_all();
        mp_deinit();
    }

    return 0;
}

void gc_collect(void) {
    gc_collect_start();
    gc_helper_collect_regs_and_stack();
    #if MICROPY_PY_THREAD
    mp_thread_gc_others();
    #endif
    gc_collect_end();
}

void nlr_jump_fail(void *val) {
    mp_printf(&mp_plat_print, "FATAL: uncaught exception %p\n", val);
    mp_obj_print_exception(&mp_plat_print, MP_OBJ_FROM_PTR(val));
    for (;;) {
        __breakpoint();
    }
}

#ifndef NDEBUG
void MP_WEAK __assert_func(const char *file, int line, const char *func, const char *expr) {
    printf("Assertion '%s' failed, at file %s:%d\n", expr, file, line);
    panic("Assertion failed");
}
#endif

#define POLY (0xD5)

uint8_t rosc_random_u8(size_t cycles) {
    static uint8_t r;
    for (size_t i = 0; i < cycles; ++i) {
        r = ((r << 1) | rosc_hw->randombit) ^ (r & 0x80 ? POLY : 0);
        mp_hal_delay_us_fast(1);
    }
    return r;
}

uint32_t rosc_random_u32(void) {
    uint32_t value = 0;
    for (size_t i = 0; i < 4; ++i) {
        value = value << 8 | rosc_random_u8(32);
    }
    return value;
}
