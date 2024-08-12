/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022-2024 Infineon Technologies AG
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

// std includes
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>


// MTB includes
#include "cybsp.h"
#include "cy_retarget_io.h"
#include "cyhal.h"

// FreeRTOS header file
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>


// micropython includes
#include "genhdr/mpversion.h"
#include "py/gc.h"
#include "py/mphal.h"
#include "py/runtime.h"
#include "py/stackctrl.h"
#include "shared/readline/readline.h"
#include "shared/runtime/pyexec.h"
#include "extmod/modnetwork.h"

#include "modmachine.h"
#include "machine_pin_phy.h"

#if MICROPY_PY_NETWORK
#include "cybsp_wifi.h"
#include "cy_wcm.h"
#endif

#if MICROPY_PY_LWIP
#include "lwip/init.h"
#include "lwip/apps/mdns.h"
#endif

// port-specific includes
#include "mplogger.h"

#define MPY_TASK_STACK_SIZE                    (4096u)
#define MPY_TASK_PRIORITY                      (3u)

typedef enum {
    BOOT_MODE_NORMAL,
    BOOT_MODE_SAFE
} boot_mode_t;

#if MICROPY_ENABLE_GC
extern uint8_t __StackTop, __StackLimit;
__attribute__((section(".bss"))) static char gc_heap[MICROPY_GC_HEAP_SIZE];
#endif

extern void mod_rtc_init(void);
extern void time_init(void);
extern void os_init(void);
extern void network_init(void);
extern void network_deinit(void);

void mpy_task(void *arg);

TaskHandle_t mpy_task_handle;

boot_mode_t check_boot_mode(void) {
    boot_mode_t boot_mode;

    // initialize user LED
    cyhal_gpio_init(CYBSP_USER_LED, CYHAL_GPIO_DIR_OUTPUT,
        CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);

    // initialize user button
    cyhal_gpio_init(CYBSP_USER_BTN, CYHAL_GPIO_DIR_INPUT,
        CYHAL_GPIO_DRIVE_PULLUP, CYBSP_BTN_OFF);

    // Added 5ms delay to allow bypass capacitor connected to the user button without external pull-up to charge.
    cyhal_system_delay_ms(5);

    if (cyhal_gpio_read(CYBSP_USER_BTN) == CYBSP_BTN_PRESSED) {
        // Blink LED twice to indicate safe boot mode was entered
        for (int i = 0; i < 4; i++)
        {
            cyhal_gpio_toggle(CYBSP_USER_LED);
            cyhal_system_delay_ms(500); // delay in millisecond
        }
        boot_mode = BOOT_MODE_SAFE;
        mp_printf(&mp_plat_print, "- DEVICE IS IN SAFE BOOT MODE -\n");
    } else { // normal boot mode
        boot_mode = BOOT_MODE_NORMAL;
    }
    // free the user LED and user button
    cyhal_gpio_free(CYBSP_USER_BTN);
    cyhal_gpio_free(CYBSP_USER_LED);

    return boot_mode;
}

int main(int argc, char **argv) {
    // Initialize the device and board peripherals
    cy_rslt_t result = cybsp_init();
    if (result != CY_RSLT_SUCCESS) {
        mp_raise_ValueError(MP_ERROR_TEXT("cybsp_init failed !\n"));
    }

    // Initialize retarget-io to use the debug UART port
    result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);
    if (result != CY_RSLT_SUCCESS) {
        mp_raise_ValueError(MP_ERROR_TEXT("cy_retarget_io_init failed !\n"));
    }

    xTaskCreate(mpy_task, "MicroPython task", MPY_TASK_STACK_SIZE, NULL, MPY_TASK_PRIORITY, &mpy_task_handle);
    vTaskStartScheduler();

    // Should never get here
    CY_ASSERT(0);
    return 0;
}

void mpy_task(void *arg) {
    #if MICROPY_ENABLE_GC
    mp_stack_set_top(&__StackTop);
    // mp_stack_set_limit((mp_uint_t)&__StackTop - (mp_uint_t)&__StackLimit);
    mp_stack_set_limit((mp_uint_t)&__StackLimit);
    gc_init(&gc_heap[0], &gc_heap[MP_ARRAY_SIZE(gc_heap)]);
    #endif

    // Initialize modules. Or to be redone after a reset and therefore to be placed next to machine_init below ?
    os_init();
    time_init();

soft_reset:
    mod_rtc_init();
    mp_init();

    // ANSI ESC sequence for clear screen. Refer to  https://stackoverflow.com/questions/517970/how-to-clear-the-interpreter-console
    mp_printf(&mp_plat_print, "\033[H\033[2J");

    // indicate in REPL console when debug mode is selected
    mplogger_print("\n...LOGGER DEBUG MODE...\n\n");

    readline_init0();
    machine_init();
    #if MICROPY_PY_NETWORK
    network_init();
    mod_network_init();
    #endif

    #if MICROPY_VFS
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_));
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_lib));

    #if MICROPY_VFS_LFS2
    pyexec_frozen_module("vfs_lfs2.py", false);
    #elif MICROPY_VFS_FAT
    pyexec_frozen_module("vfs_fat.py", false);
    #endif

    #endif

    if (check_boot_mode() == BOOT_MODE_NORMAL) {
        // Execute user scripts.
        int ret = pyexec_file_if_exists("/boot.py");

        if (ret & PYEXEC_FORCED_EXIT) {
            goto soft_reset;
        }

        if (pyexec_mode_kind == PYEXEC_MODE_FRIENDLY_REPL) {
            ret = pyexec_file_if_exists("/main.py");

            if (ret & PYEXEC_FORCED_EXIT) {
                goto soft_reset;
            }
        }
    }

    __enable_irq();

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

    mp_printf(&mp_plat_print, "MPY: soft reboot\n");
    #if MICROPY_PY_NETWORK
    mod_network_deinit();
    network_deinit();
    #endif
    gc_sweep_all();
    mp_deinit();

    goto soft_reset;
}

// TODO: to be implemented
void nlr_jump_fail(void *val) {
    mplogger_print("nlr_jump_fail\n");

    mp_printf(&mp_plat_print, "FATAL: uncaught exception %p\n", val);
    mp_obj_print_exception(&mp_plat_print, MP_OBJ_FROM_PTR(val));

    for (;;) {
        __BKPT(0);
    }
}
